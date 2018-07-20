/**********************************************************************
 Freeciv - Copyright (C) 1996 - A Kjeldberg, L Gregersen, P Unold
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
***********************************************************************/

#ifdef HAVE_CONFIG_H
#include <fc_config.h>
#endif

#include <math.h>

/* utility */
#include "bitvector.h"
#include "log.h"
#include "mem.h"
#include "rand.h"
#include "registry.h"
#include "shared.h"
#include "timing.h"

/* common */
#include "city.h"
#include "combat.h"
#include "game.h"
#include "government.h"
#include "map.h"
#include "movement.h"
#include "packets.h"
#include "player.h"
#include "specialist.h"
#include "traderoutes.h"
#include "unit.h"
#include "unitlist.h"

/* common/aicore */
#include "caravan.h"
#include "pf_tools.h"

/* ai */
#include "advmilitary.h"
#include "aiair.h"
#include "aicity.h"
#include "aidata.h"
#include "aidiplomat.h"
#include "aiferry.h"
#include "aiguard.h"
#include "aihand.h"
#include "aihunt.h"
#include "ailog.h"
#include "aiparatrooper.h"
#include "aiplayer.h"
#include "aitools.h"

/* server */
#include "barbarian.h"
#include "citytools.h"
#include "cityturn.h"
#include "diplomats.h"
#include "maphand.h"
#include "srv_log.h"
#include "unithand.h"
#include "unittools.h"

/* server/advisors */
#include "advgoto.h"
#include "advtools.h"
#include "autoexplorer.h"
#include "autosettlers.h"

#include "raiunit.h"


#define LOGLEVEL_RECOVERY LOG_DEBUG
#define LOG_CARAVAN       LOG_DEBUG
#define LOG_CARAVAN2      LOG_DEBUG
#define LOG_CARAVAN3      LOG_DEBUG

static bool rai_find_boat_for_unit(struct ai_type *ait, struct unit *punit);
static bool rai_caravan_can_trade_cities_diff_cont(struct player *pplayer,
                                                   struct unit *punit);
static void rai_manage_caravan(struct ai_type *ait, struct player *pplayer,
                               struct unit *punit);


static void rai_military_defend(struct ai_type *ait, struct player *pplayer,
                                struct unit *punit);

static void rai_manage_barbarian_leader(struct ai_type *ait,
                                        struct player *pplayer,
                                        struct unit *leader);

/*
 * Cached values. Updated by update_simple_ai_types.
 *
 * This a hack to enable emulation of old loops previously hardwired
 * as 
 *    for (i = U_WARRIORS; i <= U_BATTLESHIP; i++)
 *
 * (Could probably just adjust the loops themselves fairly simply,
 * but this is safer for regression testing.)
 *
 * Not dealing with planes yet.
 *
 * Terminated by NULL.
 */
struct unit_type *simple_ai_types[U_LAST];

/****************************************************************************
  Returns the city with the most need of an airlift.

  To be considerd, a city must have an air field. All cities with an
  urgent need for units are serviced before cities in danger.

  Return value may be NULL, this means no servicable city found.

  parameter pplayer may not be NULL.
****************************************************************************/
static struct city *find_neediest_airlift_city(struct ai_type *ait,
                                               const struct player *pplayer)
{
  struct city *neediest_city = NULL;
  int most_danger = 0;
  int most_urgent = 0;

  city_list_iterate(pplayer->cities, pcity) {
    struct ai_city *city_data = def_ai_city_data(pcity, ait);

    if (pcity->airlift) {
      if (city_data->urgency > most_urgent) {
        most_urgent = city_data->urgency;
        neediest_city = pcity;
      } else if (0 == most_urgent /* urgency trumps danger */
                 && city_data->danger > most_danger) {
        most_danger = city_data->danger;
        neediest_city = pcity;
      }
    }
  } city_list_iterate_end;

  return neediest_city;
}

/**************************************************************************
  Move defenders around with airports. Since this expends all our 
  movement, a valid question is - why don't we do this on turn end?
  That's because we want to avoid emergency actions to protect the city
  during the turn if that isn't necessary.
**************************************************************************/
static void rai_airlift(struct ai_type *ait, struct player *pplayer)
{
  struct city *most_needed;
  int comparison;
  struct unit *transported;

  do {
    most_needed = find_neediest_airlift_city(ait, pplayer);
    comparison = 0;
    transported = NULL;

    if (!most_needed) {
      return;
    }

    unit_list_iterate(pplayer->units, punit) {
      struct tile *ptile = (unit_tile(punit));
      struct city *pcity = tile_city(ptile);

      if (pcity) {
        struct ai_city *city_data = def_ai_city_data(pcity, ait);
        struct unit_ai *unit_data = def_ai_unit_data(punit, ait);

        if (city_data->urgency == 0
            && city_data->danger - DEFENCE_POWER(punit) < comparison
            && unit_can_airlift_to(punit, most_needed)
            && DEFENCE_POWER(punit) > 2
            && (unit_data->task == AIUNIT_NONE
                || unit_data->task == AIUNIT_DEFEND_HOME)
            && IS_ATTACKER(punit)) {
          comparison = city_data->danger;
          transported = punit;
        }
      }
    } unit_list_iterate_end;
    if (!transported) {
      return;
    }
    UNIT_LOG(LOG_DEBUG, transported, "airlifted to defend %s",
             city_name(most_needed));
    do_airline(transported, most_needed);
  } while (TRUE);
}

/*********************************************************************
  In the words of Syela: "Using funky fprime variable instead of f in
  the denom, so that def=1 units are penalized correctly."

  Translation (GB): build_cost_balanced is used in the denominator of
  the want equation (see, e.g.  find_something_to_kill) instead of
  just build_cost to make AI build more balanced units (with def > 1).
*********************************************************************/
int build_cost_balanced(const struct unit_type *punittype)
{
  return 2 * utype_build_shield_cost(punittype) * punittype->attack_strength /
      (punittype->attack_strength + punittype->defense_strength);
}

/****************************************************************************
  Attack rating of this particular unit right now.
****************************************************************************/
static int unit_att_rating_now(const struct unit *punit)
{
  return adv_unittype_att_rating(unit_type(punit), punit->veteran,
                                 punit->moves_left, punit->hp);
}


/**************************************************************************
  Defence rating of def_type unit against att_type unit, squared.
  See get_virtual_defense_power for the arguments att_type, def_type,
  x, y, fortified and veteran.
**************************************************************************/
int unittype_def_rating_sq(const struct unit_type *att_type,
			   const struct unit_type *def_type,
			   const struct player *def_player,
                           struct tile *ptile, bool fortified, int veteran)
{
  int v = get_virtual_defense_power(att_type, def_type, def_player, ptile,
                                    fortified, veteran)
    * def_type->hp * def_type->firepower / POWER_DIVIDER;

  return v * v;
}

/**************************************************************************
Compute how much we want to kill certain victim we've chosen, counted in
SHIELDs.

FIXME?: The equation is not accurate as the other values can vary for other
victims on same tile (we take values from best defender) - however I believe
it's accurate just enough now and lost speed isn't worth that. --pasky

Benefit is something like 'attractiveness' of the victim, how nice it would be
to destroy it. Larger value, worse loss for enemy.

Attack is the total possible attack power we can throw on the victim. Note that
it usually comes squared.

Loss is the possible loss when we would lose the unit we are attacking with (in
SHIELDs).

Vuln is vulnerability of our unit when attacking the enemy. Note that it
usually comes squared as well.

Victim count is number of victims stacked in the target tile. Note that we
shouldn't treat cities as a stack (despite the code using this function) - the
scaling is probably different. (extremely dodgy usage of it -- GB)
**************************************************************************/
int kill_desire(int benefit, int attack, int loss, int vuln, int victim_count)
{
  int desire;

  /*         attractiveness     danger */ 
  desire = ((benefit * attack - loss * vuln) * victim_count * SHIELD_WEIGHTING
            / (attack + vuln * victim_count));

  return desire;
}

/**************************************************************************
  Compute how much we want to kill certain victim we've chosen, counted in
  SHIELDs.  See comment to kill_desire.

  chance -- the probability the action will succeed, 
  benefit -- the benefit (in shields) that we are getting in the case of 
             success
  loss -- the loss (in shields) that we suffer in the case of failure

  Essentially returns the probabilistic average win amount:
      benefit * chance - loss * (1 - chance)
**************************************************************************/
static int avg_benefit(int benefit, int loss, double chance)
{
  return (int)(((benefit + loss) * chance - loss) * SHIELD_WEIGHTING);
}


/*************************************************************************
  Is there another unit which really should be doing this attack? Checks
  all adjacent tiles and the tile we stand on for such units.
**************************************************************************/
static bool is_my_turn(struct unit *punit, struct unit *pdef)
{
  int val = unit_att_rating_now(punit), cur, d;

  CHECK_UNIT(punit);

  square_iterate(unit_tile(pdef), 1, ptile) {
    unit_list_iterate(ptile->units, aunit) {
      if (aunit == punit || unit_owner(aunit) != unit_owner(punit)) {
        continue;
      }
      if (unit_attack_units_at_tile_result(aunit, unit_tile(pdef)) != ATT_OK
          || unit_attack_unit_at_tile_result(aunit, pdef, unit_tile(pdef)) != ATT_OK) {
        continue;
      }
      d = get_virtual_defense_power(unit_type(aunit), unit_type(pdef),
                                    unit_owner(pdef), unit_tile(pdef),
                                    FALSE, 0);
      if (d == 0) {
        return TRUE;            /* Thanks, Markus -- Syela */
      }
      cur = unit_att_rating_now(aunit) *
          get_virtual_defense_power(unit_type(punit), unit_type(pdef),
                                    unit_owner(pdef), unit_tile(pdef),
                                    FALSE, 0) / d;
      if (cur > val && ai_fuzzy(unit_owner(punit), TRUE)) {
        return FALSE;
      }
    }
    unit_list_iterate_end;
  }
  square_iterate_end;
  return TRUE;
}

/*************************************************************************
  This function appraises the location (x, y) for a quick hit-n-run 
  operation.  We do not take into account reinforcements: rampage is for
  loners.

  Returns value as follows:
    -RAMPAGE_FREE_CITY_OR_BETTER    
             means undefended enemy city
    -RAMPAGE_HUT_OR_BETTER    
             means hut
    RAMPAGE_ANYTHING ... RAMPAGE_HUT_OR_BETTER - 1  
             is value of enemy unit weaker than our unit
    0        means nothing found or error
  Here the minus indicates that you need to enter the target tile (as 
  opposed to attacking it, which leaves you where you are).
**************************************************************************/
static int rai_rampage_want(struct unit *punit, struct tile *ptile)
{
  struct player *pplayer = unit_owner(punit);
  struct unit *pdef;

  CHECK_UNIT(punit);

  if (can_unit_attack_tile(punit, ptile)
      && (pdef = get_defender(punit, ptile))) {
    /* See description of kill_desire() about these variables. */
    int attack = unit_att_rating_now(punit);
    int benefit = stack_cost(punit, pdef);
    int loss = unit_build_shield_cost(punit);

    attack *= attack;

    /* If the victim is in the city/fortress, we correct the benefit
     * with our health because there could be reprisal attacks.  We
     * shouldn't send already injured units to useless suicide.
     * Note that we do not specially encourage attacks against
     * cities: rampage is a hit-n-run operation. */
    if (!is_stack_vulnerable(ptile) 
        && unit_list_size(ptile->units) > 1) {
      benefit = (benefit * punit->hp) / unit_type(punit)->hp;
    }

    /* If we have non-zero attack rating... */
    if (attack > 0 && is_my_turn(punit, pdef)) {
      double chance = unit_win_chance(punit, pdef);
      int desire = avg_benefit(benefit, loss, chance);

      /* No need to amortize, our operation takes one turn. */
      UNIT_LOG(LOG_DEBUG, punit, "Rampage: Desire %d to kill %s(%d,%d)",
               desire,
               unit_rule_name(pdef),
               TILE_XY(unit_tile(pdef)));

      return MAX(0, desire);
    }
  } else if (0 == unit_list_size(ptile->units)) {
    /* No defender. */
    struct city *pcity = tile_city(ptile);

    /* ...and free foreign city waiting for us. Who would resist! */
    if (NULL != pcity
        && pplayers_at_war(pplayer, city_owner(pcity))
        && unit_can_take_over(punit)) {
      return -RAMPAGE_FREE_CITY_OR_BETTER;
    }

    /* ...or tiny pleasant hut here! */
    if (tile_has_special(ptile, S_HUT) && !is_barbarian(pplayer)
        && is_native_tile(unit_type(punit), ptile)
        && unit_class(punit)->hut_behavior == HUT_NORMAL) {
      return -RAMPAGE_HUT_OR_BETTER;
    }
  }

  return 0;
}

/*************************************************************************
  Look for worthy targets within a one-turn horizon.
*************************************************************************/
static struct pf_path *find_rampage_target(struct unit *punit,
                                           int thresh_adj, int thresh_move)
{
  struct pf_map *tgt_map;
  struct pf_path *path = NULL;
  struct pf_parameter parameter;
  /* Coordinates of the best target (initialize to silence compiler) */
  struct tile *ptile = unit_tile(punit);
  /* Want of the best target */
  int max_want = 0;
  struct player *pplayer = unit_owner(punit);
 
  pft_fill_unit_attack_param(&parameter, punit);
  /* When trying to find rampage targets we ignore risks such as
   * enemy units because we are looking for trouble!
   * Hence no call ai_avoid_risks()
   */

  tgt_map = pf_map_new(&parameter);
  pf_map_move_costs_iterate(tgt_map, iter_tile, move_cost, FALSE) {
    int want;
    bool move_needed;
    int thresh;
 
    if (move_cost > punit->moves_left) {
      /* This is too far */
      break;
    }

    if (ai_handicap(pplayer, H_TARGETS) 
        && !map_is_known_and_seen(iter_tile, pplayer, V_MAIN)) {
      /* The target is under fog of war */
      continue;
    }
    
    want = rai_rampage_want(punit, iter_tile);

    /* Negative want means move needed even though the tiles are adjacent */
    move_needed = (!is_tiles_adjacent(unit_tile(punit), iter_tile)
                   || want < 0);
    /* Select the relevant threshold */
    thresh = (move_needed ? thresh_move : thresh_adj);
    want = (want < 0 ? -want : want);

    if (want > max_want && want > thresh) {
      /* The new want exceeds both the previous maximum 
       * and the relevant threshold, so it's worth recording */
      max_want = want;
      ptile = iter_tile;
    }
  } pf_map_move_costs_iterate_end;

  if (max_want > 0) {
    /* We found something */
    path = pf_map_path(tgt_map, ptile);
    fc_assert(path != NULL);
  }

  pf_map_destroy(tgt_map);
  
  return path;
}


/*************************************************************************
  Find and kill anything reachable within this turn and worth more than
  the relevant of the given thresholds until we have run out of juicy 
  targets or movement.  The first threshold is for attacking which will 
  leave us where we stand (attacking adjacent units), the second is for 
  attacking distant (but within reach) targets.

  For example, if unit is a bodyguard on duty, it should call
    rai_military_rampage(punit, 100, RAMPAGE_FREE_CITY_OR_BETTER)
  meaning "we will move _only_ to pick up a free city but we are happy to
  attack adjacent squares as long as they are worthy of it".

  Returns TRUE if survived the rampage session.
**************************************************************************/
bool rai_military_rampage(struct unit *punit, int thresh_adj,
                          int thresh_move)
{
  int count = punit->moves_left + 1; /* break any infinite loops */
  struct pf_path *path = NULL;

  TIMING_LOG(AIT_RAMPAGE, TIMER_START);  
  CHECK_UNIT(punit);

  fc_assert_ret_val(thresh_adj <= thresh_move, TRUE);
  /* This teaches the AI about the dangers inherent in occupychance. */
  thresh_adj += ((thresh_move - thresh_adj) * game.server.occupychance / 100);

  while (count-- > 0 && punit->moves_left > 0
         && (path = find_rampage_target(punit, thresh_adj, thresh_move))) {
    if (!adv_unit_execute_path(punit, path)) {
      /* Died */
      count = -1;
    }
    pf_path_destroy(path);
    path = NULL;
  }

  fc_assert(NULL == path);

  TIMING_LOG(AIT_RAMPAGE, TIMER_STOP);
  return (count >= 0);
}


/****************************************************************************
  See if we can find something to defend. Called both by wannabe bodyguards
  and building want estimation code. Returns desirability for using this
  unit as a bodyguard or for defending a city.

  We do not consider units with higher movement than us, or units that have
  different move type than us, as potential charges. Nor do we attempt to
  bodyguard units with higher defence than us, or military units with higher
  attack than us.
****************************************************************************/
int look_for_charge(struct ai_type *ait, struct player *pplayer,
                    struct unit *punit,
                    struct unit **aunit, struct city **acity)
{
  struct pf_parameter parameter;
  struct pf_map *pfm;
  struct city *pcity;
  struct ai_city *data, *best_data = NULL;
  const int toughness = adv_unit_def_rating_basic_sq(punit);
  int def, best_def = -1;
  /* Arbitrary: 3 turns. */
  const int max_move_cost = 3 * unit_move_rate(punit);

  *aunit = NULL;
  *acity = NULL;

  if (0 == toughness) {
    /* useless */
    return 0;
  }

  pft_fill_unit_parameter(&parameter, punit);
  pfm = pf_map_new(&parameter);

  pf_map_move_costs_iterate(pfm, ptile, move_cost, TRUE) {
    if (move_cost > max_move_cost) {
      /* Consider too far. */
      break;
    }

    pcity = tile_city(ptile);

    /* Consider unit bodyguard. */
    unit_list_iterate(ptile->units, buddy) {
      /* TODO: allied unit bodyguard? */
      if (unit_owner(buddy) != pplayer
          || !aiguard_wanted(ait, buddy)
          || unit_move_rate(buddy) > unit_move_rate(punit)
          || DEFENCE_POWER(buddy) >= DEFENCE_POWER(punit)
          || (is_military_unit(buddy)
              && 0 == get_transporter_capacity(buddy)
              && ATTACK_POWER(buddy) <= ATTACK_POWER(punit))
          || (uclass_move_type(unit_class(buddy))
              != uclass_move_type(unit_class(punit)))) {
        continue;
      }

      def = (toughness - adv_unit_def_rating_basic_sq(buddy));
      if (0 >= def) {
        continue;
      }

      if (0 == get_transporter_capacity(buddy)) {
        /* Reduce want based on move cost. We can't do this for
         * transports since they move around all the time, leading
         * to hillarious flip-flops. */
        def >>= move_cost / (2 * unit_move_rate(punit));
      }
      if (def > best_def) {
        *aunit = buddy;
        *acity = NULL;
        best_def = def;
      }
    } unit_list_iterate_end;

    /* City bodyguard. TODO: allied city bodyguard? */
    if (ai_fuzzy(pplayer, TRUE)
        && NULL != pcity
        && city_owner(pcity) == pplayer
        && (data = def_ai_city_data(pcity, ait))
        && 0 < data->urgency) {
      if (NULL != best_data
          && (0 < best_data->grave_danger
              || best_data->urgency > data->urgency
              || ((best_data->danger > data->danger
                   || AIUNIT_DEFEND_HOME == def_ai_unit_data(punit, ait)->task)
                  && 0 == data->grave_danger))) {
        /* Chances are we'll be between cities when we are needed the most!
         * Resuming pf_map_move_costs_iterate()... */
        continue;
      }
      def = (data->danger - assess_defense_quadratic(ait, pcity));
      if (def <= 0) {
        continue;
      }
      /* Reduce want based on move cost. */
      def >>= move_cost / (2 * unit_move_rate(punit));
      if (def > best_def && ai_fuzzy(pplayer, TRUE)) {
       *acity = pcity;
       *aunit = NULL;
       best_def = def;
       best_data = data;
     }
    }
  } pf_map_move_costs_iterate_end;

  pf_map_destroy(pfm);

  UNIT_LOG(LOGLEVEL_BODYGUARD, punit, "%s(), best_def=%d, type=%s (%d, %d)",
           __FUNCTION__, best_def * 100 / toughness,
           (NULL != *acity ? city_name(*acity)
            : (NULL != *aunit ? unit_rule_name(*aunit) : "")),
           (NULL != *acity ? index_to_map_pos_x(tile_index(city_tile(*acity)))
            : (NULL != *aunit ?
               index_to_map_pos_x(tile_index(unit_tile(*aunit))) : -1)),
           (NULL != *acity ? index_to_map_pos_y(tile_index(city_tile(*acity)))
            : (NULL != *aunit ?
               index_to_map_pos_y(tile_index(unit_tile(*aunit))) : -1)));

  return ((best_def * 100) / toughness);
}


/********************************************************************** 
  Send a unit to the city it should defend. If we already have a city
  it should defend, use the punit->server.ai->charge field to denote this.
  Otherwise, it will stay put in the city it is in, or find a city
  to reside in, or travel all the way home.

  TODO: Add make homecity.
  TODO: Add better selection of city to defend.
***********************************************************************/
static void rai_military_defend(struct ai_type *ait, struct player *pplayer,
                                struct unit *punit)
{
  struct city *pcity = aiguard_charge_city(ait, punit);

  CHECK_UNIT(punit);

  if (!pcity || city_owner(pcity) != pplayer) {
    pcity = tile_city(unit_tile(punit));
    /* Do not stay defending an allied city forever */
    aiguard_clear_charge(ait, punit);
  }

  if (!pcity) {
    /* Try to find a place to rest. Sitting duck out in the wilderness
     * is generally a bad idea, since we protect no cities that way, and
     * it looks silly. */
    pcity = find_closest_city(unit_tile(punit), NULL, pplayer,
                              FALSE, FALSE, FALSE, TRUE, FALSE,
                              unit_class(punit));
  }

  if (!pcity) {
    pcity = game_city_by_number(punit->homecity);
  }

  if (rai_military_rampage(punit, BODYGUARD_RAMPAGE_THRESHOLD * 5,
                           RAMPAGE_FREE_CITY_OR_BETTER)) {
    /* ... we survived */
    if (pcity) {
      UNIT_LOG(LOG_DEBUG, punit, "go to defend %s", city_name(pcity));
      if (same_pos(unit_tile(punit), pcity->tile)) {
        UNIT_LOG(LOG_DEBUG, punit, "go defend successful");
        def_ai_unit_data(punit, ait)->done = TRUE;
      } else {
        (void) dai_gothere(ait, pplayer, punit, pcity->tile);
      }
    } else {
      UNIT_LOG(LOG_VERBOSE, punit, "defending nothing...?");
    }
  }
}


/****************************************************************************
  Returns TRUE if a beachhead as been found to reach 'dest_tile'.
****************************************************************************/
bool find_beachhead(const struct player *pplayer, struct pf_map *ferry_map,
                    struct tile *dest_tile,
                    const struct unit_type *cargo_type,
                    struct tile **ferry_dest, struct tile **beachhead_tile)
{
  if (NULL == tile_city(dest_tile)
      || can_attack_from_non_native(cargo_type)) {
    /* Unit can directly go to 'dest_tile'. */
    struct tile *best_tile = NULL;
    int best_cost = PF_IMPOSSIBLE_MC, cost;

    if (NULL != beachhead_tile) {
      *beachhead_tile = dest_tile;
    }

    adjc_iterate(dest_tile, ptile) {
      cost = pf_map_move_cost(ferry_map, ptile);
      if (cost != PF_IMPOSSIBLE_MC
          && (NULL == best_tile || cost < best_cost)) {
        best_tile = ptile;
        best_cost = cost;
      }
    } adjc_iterate_end;

    if (NULL != ferry_dest) {
      *ferry_dest = best_tile;
    }

    return (PF_IMPOSSIBLE_MC != best_cost);
  } else {
    /* We need to find a beach around 'dest_tile'. */
    struct tile *best_tile = NULL, *best_beach = NULL;
    struct tile_list *checked_tiles = tile_list_new();
    int best_cost = PF_IMPOSSIBLE_MC, cost;

    tile_list_append(checked_tiles, dest_tile);
    adjc_iterate(dest_tile, beach) {
      if (is_native_tile(cargo_type, beach)) {
        /* Can land there. */
        adjc_iterate(beach, ptile) {
          if (!tile_list_search(checked_tiles, ptile)
              && !is_non_allied_unit_tile(ptile, pplayer)) {
            tile_list_append(checked_tiles, ptile);
            cost = pf_map_move_cost(ferry_map, ptile);
            if (cost != PF_IMPOSSIBLE_MC
                && (NULL == best_tile || cost < best_cost)) {
              best_beach = beach;
              best_tile = ptile;
              best_cost = cost;
            }
          }
        } adjc_iterate_end;
      }
    } adjc_iterate_end;

    tile_list_destroy(checked_tiles);

    if (NULL != beachhead_tile) {
      *beachhead_tile = best_beach;
    }
    if (NULL != ferry_dest) {
      *ferry_dest = best_tile;
    }
    return (PF_IMPOSSIBLE_MC != best_cost);
  }
}


/****************************************************************************
  Find safe city to recover in. An allied player's city is just as good as
  one of our own, since both replenish our hitpoints and reduce unhappiness.

  TODO: Actually check how safe the city is. This is a difficult decision
  not easily taken, since we also want to protect unsafe cities, at least
  most of the time.
****************************************************************************/
struct city *find_nearest_safe_city(struct unit *punit)
{
  struct pf_parameter parameter;
  struct pf_map *pfm;
  struct player *pplayer = unit_owner(punit);
  struct city *pcity, *best_city = NULL;
  int best = FC_INFINITY, cur;

  pft_fill_unit_parameter(&parameter, punit);
  pfm = pf_map_new(&parameter);

  pf_map_move_costs_iterate(pfm, ptile, move_cost, TRUE) {
    if (move_cost > best) {
      /* We already found a better city. No need to continue. */
      break;
    }

    pcity = tile_city(ptile);
    if (NULL == pcity || !pplayers_allied(pplayer, city_owner(pcity))) {
      continue;
    }

    /* Score based on the move cost. */
    cur = move_cost;

    /* Note the unit owner may be different from the city owner. */
    if (0 == get_unittype_bonus(unit_owner(punit), ptile, unit_type(punit),
                               EFT_HP_REGEN)) {
      /* If we cannot regen fast our hit points here, let's make some
       * penalty. */
      cur *= 3;
    }

    if (cur < best) {
      best_city = pcity;
      best = cur;
    }
  } pf_map_move_costs_iterate_end;

  pf_map_destroy(pfm);
  return best_city;
}



/*************************************************************************
  Request a boat for a unit to transport it to another continent.
  Return wheter is alive or not
*************************************************************************/
static bool rai_find_boat_for_unit(struct ai_type *ait, struct unit *punit)
{
  bool alive = TRUE;
  int ferryboat = 0;
  struct pf_path *path_to_ferry = NULL;

  UNIT_LOG(LOG_CARAVAN, punit, "requesting a boat!");
  ferryboat = aiferry_find_boat(ait, punit, 1, &path_to_ferry);
  /* going to meet the boat */
  if ((ferryboat <= 0)) {
    UNIT_LOG(LOG_CARAVAN, punit, 
             "in find_boat_for_unit cannot find any boats.");
    /* if we are undefended on the country side go to a city */
    struct city *current_city = tile_city(unit_tile(punit));
    if (current_city == NULL) {
      struct city *city_near = find_nearest_safe_city(punit);
      if (city_near != NULL) {
        alive = dai_unit_goto(ait, punit, city_near->tile);
      }
    }
  } else {
    if (path_to_ferry != NULL) {
      if (!adv_unit_execute_path(punit, path_to_ferry)) { 
        /* Died. */
        pf_path_destroy(path_to_ferry);
        path_to_ferry = NULL;
        alive = FALSE;
      } else {
        pf_path_destroy(path_to_ferry);
        path_to_ferry = NULL;
        alive = TRUE;
      }
    }
  }
  return alive;
}

/*************************************************************************
  Send the caravan to the specified city, or make it help the wonder /
  trade, if it's already there.  After this call, the unit may no longer
  exist (it might have been used up, or may have died travelling).
  It uses the ferry system to trade among continents.
**************************************************************************/
static void rai_caravan_goto(struct ai_type *ait, struct player *pplayer,
                             struct unit *punit,
                             const struct city *dest_city,
                             bool help_wonder,
                             bool required_boat, bool request_boat)
{
  bool alive = TRUE;
  struct unit_ai *unit_data = def_ai_unit_data(punit, ait);

  fc_assert_ret(NULL != dest_city);

  /* if we're not there yet, and we can move, move... */
  if (!same_pos(dest_city->tile, unit_tile(punit))
      && punit->moves_left != 0) {
    log_base(LOG_CARAVAN, "%s %s[%d](%d,%d) task %s going to %s in %s %s",
             nation_rule_name(nation_of_unit(punit)),
             unit_rule_name(punit), punit->id, TILE_XY(unit_tile(punit)),
             dai_unit_task_rule_name(unit_data->task),
             help_wonder ? "help a wonder" : "trade", city_name(dest_city),
             required_boat ? "with a boat" : "");
    if (required_boat) {
      /* to trade with boat */
      if (request_boat) {
        /* Try to find new boat */
        alive = rai_find_boat_for_unit(ait, punit);
      } else {
        /* if we are not being transported then ask for a boat again */
        alive = TRUE;
        if (!unit_transported(punit) &&
            (tile_continent(unit_tile(punit))
             != tile_continent(dest_city->tile))) {
          alive = rai_find_boat_for_unit(ait, punit);
        }
      }
      if (alive)  {
	alive = dai_gothere(ait, pplayer, punit, dest_city->tile);
      }
    } else {
      /* to trade without boat */
      alive = dai_unit_goto(ait, punit, dest_city->tile);
    }
  }

  /* if moving didn't kill us, and we got to the destination, handle it. */
  if (alive && same_pos(dest_city->tile, unit_tile(punit))) {
    /* release the boat! */
    if (unit_transported(punit)) {
      aiferry_clear_boat(ait, punit);
    }
    if (help_wonder) {
        /*
         * We really don't want to just drop all caravans in immediately.
         * Instead, we want to aggregate enough caravans to build instantly.
         * -AJS, 990704
         */
      log_base(LOG_CARAVAN, "%s %s[%d](%d,%d) helps build wonder in %s",
               nation_rule_name(nation_of_unit(punit)),
               unit_rule_name(punit),
               punit->id,
               TILE_XY(unit_tile(punit)),
               city_name(dest_city));
      handle_unit_help_build_wonder(pplayer, punit->id);
    } else {
      log_base(LOG_CARAVAN, "%s %s[%d](%d,%d) creates trade route in %s",
               nation_rule_name(nation_of_unit(punit)),
               unit_rule_name(punit),
               punit->id,
               TILE_XY(unit_tile(punit)),
               city_name(dest_city));
      handle_unit_establish_trade(pplayer, punit->id);
    }
  }
}

/*************************************************************************
  For debugging, print out information about every city we come to when
  optimizing the caravan.
 *************************************************************************/
static void caravan_optimize_callback(const struct caravan_result *result,
                                      void *data)
{
  const struct unit *caravan = data;

  log_base(LOG_CARAVAN3, "%s %s[%d](%d,%d) %s: %s %s worth %g",
           nation_rule_name(nation_of_unit(caravan)),
           unit_rule_name(caravan),
           caravan->id,
           TILE_XY(unit_tile(caravan)),
           city_name(result->src),
           result->help_wonder ? "wonder in" : "trade to",
           city_name(result->dest),
           result->value);
}

/*****************************************************************************
  Evaluate if a unit is tired of waiting for a boat at home continent
*****************************************************************************/
static bool rai_is_unit_tired_waiting_boat(struct ai_type *ait,
                                           struct unit *punit)
{
  struct tile *src = NULL, *dest = NULL, *src_home_city = NULL;
  struct city *phome_city = NULL;
  struct unit_ai *unit_data = def_ai_unit_data(punit, ait);
  bool required_boat = FALSE;
  
  if ((unit_data->task != AIUNIT_NONE)) {
    src = unit_tile(punit);
    phome_city = game_city_by_number(punit->homecity);
    if (phome_city != NULL) {
      src_home_city = city_tile(phome_city);
    }
    dest = punit->goto_tile;

    if (src == NULL || dest == NULL) {
      return FALSE;
    }
    /* if we're not at home continent */
    if (tile_continent(src) != tile_continent(src_home_city)) {
      return FALSE;
    }

    required_boat = (tile_continent(src) == 
                     tile_continent(dest)) ? FALSE : TRUE;
    if (utype_move_type(unit_type(punit)) != UMT_LAND) {
      /* Can travel on ocean itself */
      required_boat = FALSE;
    }

    if (required_boat) {
      if (unit_transported(punit)) {
        /* if we're being transported */
        return FALSE;
      }
      if ((punit->server.birth_turn + 15 < game.info.turn)) {
        /* we are tired of waiting */
        int ferrys = aiferry_avail_boats(ait, punit->owner);

        if (ferrys <= 0) {
          /* there are no ferrys available... give up */
          return TRUE;
        } else {
          if (punit->server.birth_turn + 20 < game.info.turn) {
            /* we are feed up! */
            return TRUE;
          }
        }
      }
    }
  }

  return FALSE;
}

/*****************************************************************************
  Check if a caravan can make a trade route to a city on a different
  continent.
*****************************************************************************/
static bool rai_caravan_can_trade_cities_diff_cont(struct player *pplayer,
                                                   struct unit *punit) {
  struct city *pcity = game_city_by_number(punit->homecity);
  Continent_id continent;

  fc_assert(pcity != NULL);

  continent = tile_continent(pcity->tile);

  /* Look for proper destination city at different continent. */
  city_list_iterate(pplayer->cities, acity) {
    if (can_cities_trade(pcity, acity)) {
      if (tile_continent(acity->tile) != continent) {
        return TRUE;
      }
    }
  } city_list_iterate_end;

  players_iterate(aplayer) {
    if (aplayer == pplayer || !aplayer->is_alive) {
      continue;
    }
    if (pplayers_allied(pplayer, aplayer)) {      
      city_list_iterate(aplayer->cities, acity) {
        if (can_cities_trade(pcity, acity)) {
          if (tile_continent(acity->tile) != continent) {
            return TRUE;
          }
        }
      } city_list_iterate_end;
    } players_iterate_end;
  }

  return FALSE;
}

/*************************************************************************
  Try to move caravan to suitable city and to make it caravan's homecity.
  Returns FALSE iff caravan dies.
**************************************************************************/
static bool search_homecity_for_caravan(struct ai_type *ait, struct unit *punit)
{
  struct city *nearest = NULL;
  int min_dist = FC_INFINITY;
  struct tile *current_loc = unit_tile(punit);
  Continent_id continent = tile_continent(current_loc);
  bool alive = TRUE;

  city_list_iterate(punit->owner->cities, pcity) {
    struct tile *ctile = city_tile(pcity);

    if (tile_continent(ctile) == continent) {
      int this_dist = map_distance(current_loc, ctile);

      if (this_dist < min_dist) {
        min_dist = this_dist;
        nearest = pcity;
      }
    }
  } city_list_iterate_end;

  if (nearest != NULL) {
    alive = dai_unit_goto(ait, punit, nearest->tile);
    if (alive && same_pos(unit_tile(punit), nearest->tile)) {
      dai_unit_make_homecity(punit, nearest);
    }
  }

  return alive;
}

/*************************************************************************
  Use caravans for building wonders, or send caravans to establish
  trade with a city, owned by yourself or an ally.

  We use ferries for trade across the sea.
**************************************************************************/
static void rai_manage_caravan(struct ai_type *ait, struct player *pplayer,
                               struct unit *punit)
{
  struct caravan_parameter parameter;
  struct caravan_result result;
  const struct city *homecity;
  const struct city *dest = NULL;
  struct unit_ai *unit_data;
  bool help_wonder = FALSE;
  bool required_boat = FALSE;
  bool request_boat = FALSE;
  bool tired_of_waiting_boat = FALSE;

  CHECK_UNIT(punit);

  if (!unit_has_type_flag(punit, UTYF_TRADE_ROUTE) &&
      !unit_has_type_flag(punit, UTYF_HELP_WONDER)) {
    /* we only want units that can establish trade or help build wonders */
    return;
  }

  unit_data = def_ai_unit_data(punit, ait);

  log_base(LOG_CARAVAN2, "%s %s[%d](%d,%d) task %s to (%d,%d)",
           nation_rule_name(nation_of_unit(punit)),
           unit_rule_name(punit), punit->id, TILE_XY(unit_tile(punit)),
           dai_unit_task_rule_name(unit_data->task),
           TILE_XY(punit->goto_tile));

  homecity = game_city_by_number(punit->homecity);
  if (homecity == NULL && unit_data->task == AIUNIT_TRADE) {
    if (!search_homecity_for_caravan(ait, punit)) {
      return;
    }
    homecity = game_city_by_number(punit->homecity);
    if (homecity == NULL) {
      return;
    }
  }

  if ((unit_data->task == AIUNIT_TRADE || 
       unit_data->task == AIUNIT_WONDER)) {
    /* we are moving to our destination */
    /* we check to see if our current goal is feasible */
    struct city *city_dest = tile_city(punit->goto_tile);

    if ((city_dest == NULL) 
        || !pplayers_allied(unit_owner(punit), city_dest->owner)
        || (unit_data->task == AIUNIT_TRADE
            && !(can_cities_trade(homecity, city_dest)
                 && can_establish_trade_route(homecity, city_dest)))
        || (unit_data->task == AIUNIT_WONDER
            && city_dest->production.kind == VUT_IMPROVEMENT
            && !is_wonder(city_dest->production.value.building))) {
      /* destination invalid! */
      dai_unit_new_task(ait, punit, AIUNIT_NONE, NULL);
      log_base(LOG_CARAVAN2, "%s %s[%d](%d,%d) destination invalid!",
               nation_rule_name(nation_of_unit(punit)),
               unit_rule_name(punit), punit->id, TILE_XY(unit_tile(punit)));
    } else {
      /* destination valid, are we tired of waiting for a boat? */
      if (rai_is_unit_tired_waiting_boat(ait, punit)) {
        aiferry_clear_boat(ait, punit);
        dai_unit_new_task(ait, punit, AIUNIT_NONE, NULL);
        log_base(LOG_CARAVAN2, "%s %s[%d](%d,%d) unit tired of waiting!",
                 nation_rule_name(nation_of_unit(punit)),
                 unit_rule_name(punit), punit->id, TILE_XY(unit_tile(punit)));
        tired_of_waiting_boat = TRUE;
      } else {
        dest = city_dest;
        help_wonder = (unit_data->task == AIUNIT_WONDER) ? TRUE : FALSE;
        required_boat = (tile_continent(unit_tile(punit)) == 
                         tile_continent(dest->tile)) ? FALSE : TRUE;
        request_boat = FALSE;
      }
    }
  }

  if (unit_data->task == AIUNIT_NONE) {
    if (homecity == NULL) {
      /* FIXME: We shouldn't bother in getting homecity for
       * caravan that will then be used for wonder building. */
      if (!search_homecity_for_caravan(ait, punit)) {
        return;
      }
      homecity = game_city_by_number(punit->homecity);
      if (homecity == NULL) {
        return;
      }
    }

    caravan_parameter_init_from_unit(&parameter, punit);
    parameter.allow_foreign_trade = TRUE;

    if (log_do_output_for_level(LOG_CARAVAN2)) {
      parameter.callback = caravan_optimize_callback;
      parameter.callback_data = punit;
    }
    if (rai_caravan_can_trade_cities_diff_cont(pplayer, punit)) {
      parameter.ignore_transit_time = TRUE;
    }
    if (tired_of_waiting_boat) {
      parameter.allow_foreign_trade = FALSE;
      parameter.ignore_transit_time = FALSE;
    }
    caravan_find_best_destination(punit, &parameter, &result);
    if (result.dest != NULL) {
      /* we did find a new destination for the unit */
      dest = result.dest;
      help_wonder = result.help_wonder;
      required_boat = (tile_continent(unit_tile(punit)) ==
                       tile_continent(dest->tile)) ? FALSE : TRUE;
      request_boat = required_boat;
      dai_unit_new_task(ait, punit,
                        (help_wonder) ? AIUNIT_WONDER : AIUNIT_TRADE,
                        dest->tile);
    } else {
      dest = NULL;
    }
  }

  if (dest != NULL) {
    rai_caravan_goto(ait, pplayer, punit, dest, help_wonder,
                     required_boat, request_boat);
    return; /* that may have clobbered the unit */
  } else {
    /* We have nowhere to go! */
     log_base(LOG_CARAVAN2, "%s %s[%d](%d,%d), nothing to do!",
              nation_rule_name(nation_of_unit(punit)),
              unit_rule_name(punit), punit->id,
              TILE_XY(unit_tile(punit)));
     /* Should we become a defensive unit? */
  }
}




/**************************************************************************
  Barbarian units may disband spontaneously if their age is more than
  BARBARIAN_MIN_LIFESPAN, they are not in cities, and they are far from
  any enemy units. It is to remove barbarians that do not engage into any
  activity for a long time.
**************************************************************************/
static bool unit_can_be_retired(struct unit *punit)
{
  if (punit->server.birth_turn + BARBARIAN_MIN_LIFESPAN > game.info.turn) {
    return FALSE;
  }

  if (is_allied_city_tile
      ((unit_tile(punit)), unit_owner(punit))) return FALSE;

  /* check if there is enemy nearby */
  square_iterate(unit_tile(punit), 3, ptile) {
    if (is_enemy_city_tile(ptile, unit_owner(punit)) ||
	is_enemy_unit_tile(ptile, unit_owner(punit)))
      return FALSE;
  }
  square_iterate_end;

  return TRUE;
}

/**************************************************************************
  Manages settlers.
**************************************************************************/
static void rai_manage_settler(struct ai_type *ait, struct player *pplayer,
                               struct unit *punit)
{
  struct unit_ai *unit_data = def_ai_unit_data(punit, ait);

  punit->ai_controlled = TRUE;
  unit_data->done = TRUE; /* we will manage this unit later... ugh */
  /* if BUILD_CITY must remain BUILD_CITY, otherwise turn into autosettler */
  if (unit_data->task == AIUNIT_NONE) {
    adv_unit_new_task(punit, AUT_AUTO_SETTLER, NULL);
  }
  return;
}

/****************************************************************************
  Barbarian leader tries to stack with other barbarian units, and if it's
  not possible it runs away. When on coast, it may disappear with 33%
  chance.
****************************************************************************/
static void rai_manage_barbarian_leader(struct ai_type *ait,
                                        struct player *pplayer,
                                        struct unit *leader)
{
  struct tile *leader_tile = unit_tile(leader), *safest_tile;
  Continent_id leader_cont = tile_continent(leader_tile);
  struct pf_parameter parameter;
  struct pf_map *pfm;
  struct pf_reverse_map *pfrm;
  struct unit *worst_danger;
  int move_cost, best_move_cost;
  int body_guards;
  bool alive = TRUE;

  CHECK_UNIT(leader);

  if (0 == leader->moves_left
      || (can_unit_survive_at_tile(leader, leader_tile)
          && 1 < unit_list_size(leader_tile->units))) {
    unit_activity_handling(leader, ACTIVITY_SENTRY);
    return;
  }

  if (is_boss_of_boat(ait, leader)) {
    /* We are in charge. Of course, since we are the leader...
     * But maybe somebody more militaristic should lead our ship to battle! */

    /* First release boat from leaders lead */
    aiferry_clear_boat(ait, leader);

    unit_list_iterate(leader_tile->units, warrior) {
      if (!unit_has_type_role(warrior, L_BARBARIAN_LEADER)
          && get_transporter_capacity(warrior) == 0
          && warrior->moves_left > 0) {
        /* This seems like a good warrior to lead us in to conquest! */
        dai_manage_unit(ait, pplayer, warrior);

        /* If we reached our destination, ferryboat already called
         * ai_manage_unit() for leader. So no need to continue here.
         * Leader might even be dead.
         * If this return is removed, surronding unit_list_iterate()
         * has to be replaced with unit_list_iterate_safe()*/
        return;
      }
    } unit_list_iterate_end;
  }

  /* If we are not in charge of the boat, continue as if we
   * were not in a boat - we may want to leave the ship now. */

  /* Check the total number of units able to protect our leader. */
  body_guards = 0;
  unit_list_iterate(pplayer->units, punit) {
    if (!unit_has_type_role(punit, L_BARBARIAN_LEADER)
        && is_ground_unit(punit)
        && tile_continent(unit_tile(punit)) == leader_cont) {
      body_guards++;
    }
  } unit_list_iterate_end;

  if (0 < body_guards) {
    pft_fill_unit_parameter(&parameter, leader);
    pfm = pf_map_new(&parameter);

    /* Find the closest body guard. FIXME: maybe choose the strongest too? */
    pf_map_tiles_iterate(pfm, ptile, FALSE) {
      unit_list_iterate(ptile->units, punit) {
        if (unit_owner(punit) == pplayer
            && !unit_has_type_role(punit, L_BARBARIAN_LEADER)
            && is_ground_unit(punit)
            && tile_continent(unit_tile(punit)) == leader_cont) {
          struct pf_path *path = pf_map_path(pfm, ptile);

          adv_follow_path(leader, path, ptile);
          pf_path_destroy(path);
          pf_map_destroy(pfm);
          return;
        }
      } unit_list_iterate_end;
    } pf_map_tiles_iterate_end;

    pf_map_destroy(pfm);
  }

  UNIT_LOG(LOG_DEBUG, leader, "Barbarian leader needs to flee");

  /* Disappearance - 33% chance on coast, when older than barbarian life
   * span. */
  if (is_terrain_class_near_tile(unit_tile(leader), TC_OCEAN)
      && (leader->server.birth_turn + BARBARIAN_MIN_LIFESPAN
          < game.info.turn)) {
    if (fc_rand(3) == 0) {
      UNIT_LOG(LOG_DEBUG, leader, "barbarian leader disappearing...");
      wipe_unit(leader, ULR_RETIRED, NULL);
      return;
    }
  }

  /* Check for units we could fear. */
  pfrm = pf_reverse_map_new_for_unit(leader, 3);
  worst_danger = NULL;
  best_move_cost = FC_INFINITY;

  players_iterate(other_player) {
    if (other_player == pplayer) {
      continue;
    }

    unit_list_iterate(other_player->units, punit) {
      if (!is_ground_unit(punit)
          || tile_continent(unit_tile(punit)) != leader_cont) {
        continue;
      }

      move_cost = pf_reverse_map_unit_move_cost(pfrm, punit);
      if (PF_IMPOSSIBLE_MC != move_cost && move_cost < best_move_cost) {
        best_move_cost = move_cost;
        worst_danger = punit;
      }
    } unit_list_iterate_end;
  } players_iterate_end;

  pf_reverse_map_destroy(pfrm);

  if (NULL == worst_danger) {
    unit_activity_handling(leader, ACTIVITY_IDLE);
    UNIT_LOG(LOG_DEBUG, leader, "Barbarian leader: no close enemy.");
    return;
  }

  pft_fill_unit_parameter(&parameter, worst_danger);
  pfm = pf_map_new(&parameter);
  best_move_cost = pf_map_move_cost(pfm, leader_tile);

  /* Try to escape. */
  do {
    safest_tile = leader_tile;

    UNIT_LOG(LOG_DEBUG, leader, "Barbarian leader: moves left: %d.",
             leader->moves_left);

    adjc_iterate(leader_tile, near_tile) {
      if (adv_could_unit_move_to_tile(leader, near_tile) != 1) {
        continue;
      }

      move_cost = pf_map_move_cost(pfm, near_tile);
      if (PF_IMPOSSIBLE_MC != move_cost
          && move_cost > best_move_cost) {
        UNIT_LOG(LOG_DEBUG, leader,
                 "Barbarian leader: safest is (%d, %d), safeness %d",
                 TILE_XY(near_tile), best_move_cost);
        best_move_cost = move_cost;
        safest_tile = near_tile;
      }
    } adjc_iterate_end;

    UNIT_LOG(LOG_DEBUG, leader, "Barbarian leader: fleeing to (%d, %d).",
             TILE_XY(safest_tile));
    if (same_pos(unit_tile(leader), safest_tile)) {
      UNIT_LOG(LOG_DEBUG, leader,
               "Barbarian leader: reached the safest position.");
      unit_activity_handling(leader, ACTIVITY_IDLE);
      pf_map_destroy(pfm);
      return;
    }

    alive = dai_unit_goto(ait, leader, safest_tile);
    if (alive) {
      if (same_pos(unit_tile(leader), leader_tile)) {
        /* Didn't move. No point to retry. */
        pf_map_destroy(pfm);
        return;
      }
      leader_tile = unit_tile(leader);
    }
  } while (alive && 0 < leader->moves_left);

  pf_map_destroy(pfm);
}


void rai_manage_military(struct ai_type *ait, struct player *pplayer,
		struct unit *punit) {
	//printf("RANDOM MILITARY");

	CHECK_UNIT(punit);

	struct genlist* actionList = genlist_new();

	//unit_activity_handling(punit, ACTIVITY_IDLE);

	collect_random_explorer_moves(punit, actionList);

	if(punit->activity!=ACTIVITY_FORTIFYING &&
			can_unit_do_activity(punit, ACTIVITY_FORTIFYING)){
		struct potentialMove *pMove = malloc(sizeof(struct potentialMove));
		pMove->type = fortify;
		pMove->moveInfo = NULL;
		genlist_append(actionList, pMove);
	}

	if(punit->activity!=ACTIVITY_SENTRY &&
			can_unit_do_activity(punit, ACTIVITY_SENTRY)){

		struct potentialMove *pMove = malloc(sizeof(struct potentialMove));
		pMove->type = sentry;
		pMove->moveInfo = NULL;
		genlist_append(actionList, pMove);
	}

	if (can_unit_do_activity(punit, ACTIVITY_PILLAGE)) {
		struct potentialMove *pMove = malloc(sizeof(struct potentialMove));
		pMove->type = pillage;
		pMove->moveInfo = NULL;
		genlist_append(actionList, pMove);
	}

	//find_all_rampage_targets(punit, RAMPAGE_ANYTHING, RAMPAGE_ANYTHING, actionList);
	//	if (!dai_military_rampage(punit, RAMPAGE_ANYTHING, RAMPAGE_ANYTHING)) {
	//		return; /* we died */
	//	}

	// Add improve health function? This shouldn't be needed
	// Defend city? Probably not needed also
	// Should be achieved by random moves

	struct potentialMove *chosen_action = genlist_get(actionList, fc_rand(genlist_size(actionList)));

	switch(chosen_action->type){
	case explore:
		switch (move_random_auto_explorer(punit, chosen_action->moveInfo)) {
		case MR_DEATH:
			//don't use punit!
			break;
		case MR_OK:
			UNIT_LOG(LOG_DEBUG, punit, "more exploring");
			break;
		default:
			UNIT_LOG(LOG_DEBUG, punit, "no more exploring either");
			break;
		};
		break;
		case sentry:
			unit_activity_handling(punit, ACTIVITY_SENTRY);
			break;
		case fortify:
			unit_activity_handling(punit, ACTIVITY_FORTIFYING);
			break;
		case pillage:
			unit_activity_handling(punit, ACTIVITY_PILLAGE);
			break;
		case rage:
			break;
		default:
			break;
	}


	// Destroy the list and elements within
	for(int i = 0; i < genlist_size(actionList); i++ ){
		void *toRemove = genlist_back(actionList);
		genlist_pop_back(actionList);
		free(toRemove);
	}
	genlist_destroy(actionList);

	//dai_unit_new_task(ait, punit, AIUNIT_NONE, NULL);

	def_ai_unit_data(punit, ait)->done = TRUE;
}



/**************************************************************************
 manage one unit
 Careful: punit may have been destroyed upon return from this routine!

 Gregor: This function is a very limited approach because if a unit has
 several flags the first one in order of appearance in this function
 will be used.
**************************************************************************/
void rai_manage_unit(struct ai_type *ait, struct player *pplayer,
                     struct unit *punit)
{
  struct unit_ai *unit_data;
  struct unit *bodyguard = aiguard_guard_of(ait, punit);
  bool is_ferry = FALSE;

  CHECK_UNIT(punit);

  /* Don't manage the unit if it is under human orders. */
  if (unit_has_orders(punit)) {
    struct unit_ai *unit_data = def_ai_unit_data(punit, ait);

    UNIT_LOG(LOG_VERBOSE, punit, "is under human orders, aborting AI control.");
    dai_unit_new_task(ait, punit, AIUNIT_NONE, NULL);
    unit_data->done = TRUE;
    return;
  }

  /* retire useless barbarian units here, before calling the management
     function */
  if( is_barbarian(pplayer) ) {
    /* Todo: should be configurable */
    if (unit_can_be_retired(punit) && fc_rand(100) > 90) {
      wipe_unit(punit, ULR_RETIRED, NULL);
      return;
    }
  }

  /* Check if we have lost our bodyguard. If we never had one, all
   * fine. If we had one and lost it, ask for a new one. */
  if (!bodyguard && aiguard_has_guard(ait, punit)) {
    UNIT_LOG(LOGLEVEL_BODYGUARD, punit, "lost bodyguard, asking for new");
    aiguard_request_guard(ait, punit);
  }

  unit_data = def_ai_unit_data(punit, ait);

  if (punit->moves_left <= 0) {
    /* Can do nothing */
    unit_data->done = TRUE;
    return;
  }

  is_ferry = dai_is_ferry(punit);

  if ((unit_has_type_flag(punit, UTYF_DIPLOMAT))
      || (unit_has_type_flag(punit, UTYF_SPY))) {
    TIMING_LOG(AIT_DIPLOMAT, TIMER_START);
    dai_manage_diplomat(ait, pplayer, punit);
    TIMING_LOG(AIT_DIPLOMAT, TIMER_STOP);
    return;
  } else if (unit_has_type_flag(punit, UTYF_SETTLERS)
	     ||unit_has_type_flag(punit, UTYF_CITIES)) {
    rai_manage_settler(ait, pplayer, punit);
    return;
  } else if (unit_has_type_flag(punit, UTYF_TRADE_ROUTE)
             || unit_has_type_flag(punit, UTYF_HELP_WONDER)) {
    TIMING_LOG(AIT_CARAVAN, TIMER_START);
    rai_manage_caravan(ait, pplayer, punit);
    TIMING_LOG(AIT_CARAVAN, TIMER_STOP);
    return;
  } else if (unit_has_type_role(punit, L_BARBARIAN_LEADER)) {
    rai_manage_barbarian_leader(ait, pplayer, punit);
    return;
  } else if (unit_has_type_flag(punit, UTYF_PARATROOPERS)) {
    dai_manage_paratrooper(ait, pplayer, punit);
    return;
  } else if (is_ferry && unit_data->task != AIUNIT_HUNTER) {
    TIMING_LOG(AIT_FERRY, TIMER_START);
    dai_manage_ferryboat(ait, pplayer, punit);
    TIMING_LOG(AIT_FERRY, TIMER_STOP);
    return;
  } else if (utype_fuel(unit_type(punit))
             && unit_data->task != AIUNIT_ESCORT) {
    TIMING_LOG(AIT_AIRUNIT, TIMER_START);
    dai_manage_airunit(ait, pplayer, punit);
    TIMING_LOG(AIT_AIRUNIT, TIMER_STOP);
    return;
  } else if (is_losing_hp(punit)) {
    /* This unit is losing hitpoints over time */

    /* TODO: We can try using air-unit code for helicopters, just
     * pretend they have fuel = HP / 3 or something. */
    unit_data->done = TRUE; /* we did our best, which was ...
                                             nothing */
    return;
  } else if (is_military_unit(punit)) {
    TIMING_LOG(AIT_MILITARY, TIMER_START);
    UNIT_LOG(LOG_DEBUG, punit, "recruit unit for the military");
    rai_manage_military(ait, pplayer, punit);
    TIMING_LOG(AIT_MILITARY, TIMER_STOP);
    return;
  } else {
    /* what else could this be? -- Syela */

    switch (manage_random_auto_explorer2(punit)) {
    case MR_DEATH:
      /* don't use punit! */
      break;
    case MR_OK:
      UNIT_LOG(LOG_DEBUG, punit, "now exploring");
      break;
    default:
      UNIT_LOG(LOG_DEBUG, punit, "fell through all unit tasks, defending");
      dai_unit_new_task(ait, punit, AIUNIT_DEFEND_HOME, NULL);
      rai_military_defend(ait, pplayer, punit);
      break;
    };
    return;
  }
}

/**************************************************************************
  Master city defense function.  We try to pick up the best available
  defenders, and not disrupt existing roles.

  TODO: Make homecity, respect homecity.
**************************************************************************/
static void rai_set_defenders(struct ai_type *ait, struct player *pplayer)
{
  city_list_iterate(pplayer->cities, pcity) {
    /* The idea here is that we should never keep more than two
     * units in permanent defense. */
    int total_defense = 0;
    int total_attack = def_ai_city_data(pcity, ait)->danger;
    bool emergency = FALSE;
    int count = 0;
    int mart_max = get_city_bonus(pcity, EFT_MARTIAL_LAW_MAX);
    int mart_each = get_city_bonus(pcity, EFT_MARTIAL_LAW_EACH);
    int martless_unhappy = pcity->feel[CITIZEN_UNHAPPY][FEELING_NATIONALITY]
      + pcity->feel[CITIZEN_ANGRY][FEELING_NATIONALITY];
    int entertainers = 0;
    bool enough = FALSE;

    specialist_type_iterate(sp) {
      if (get_specialist_output(pcity, sp, O_LUXURY) > 0) {
        entertainers += pcity->specialists[sp];
      }
    } specialist_type_iterate_end;

    martless_unhappy += entertainers; /* We want to use martial law instead
                                       * of entertainers. */

    while (!enough
           && (total_defense <= total_attack
               || (count < mart_max && mart_each > 0
                   && martless_unhappy > mart_each * count))) {
      int best_want = 0;
      struct unit *best = NULL;
      bool defense_needed = total_defense <= total_attack; /* Defense or martial */

      unit_list_iterate(pcity->tile->units, punit) {
        struct unit_ai *unit_data = def_ai_unit_data(punit, ait);

        if ((unit_data->task == AIUNIT_NONE || emergency)
            && unit_data->task != AIUNIT_DEFEND_HOME
            && unit_owner(punit) == pplayer) {
          int want = assess_defense_unit(ait, pcity, punit, FALSE);

          if (want > best_want) {
            best_want = want;
            best = punit;
          }
        }
      } unit_list_iterate_end;
      
      if (best == NULL) {
        if (defense_needed) {
          /* Ooops - try to grab any unit as defender! */
          if (emergency) {
            CITY_LOG(LOG_DEBUG, pcity, "Not defended properly");
            break;
          }
          emergency = TRUE;
        } else {
          break;
        }
      } else {
        struct unit_type *btype = unit_type(best);

        if ((martless_unhappy < mart_each * count
             || count >= mart_max || mart_each <= 0)
            && ((count >= 2
                 && btype->attack_strength > btype->defense_strength)
                || (count >= 4
                    && btype->attack_strength == btype->defense_strength))) {
          /* In this case attack would be better defense than fortifying
           * to city. */
          enough = TRUE;
        } else {
          int loglevel = pcity->server.debug ? LOG_AI_TEST : LOG_DEBUG;

          total_defense += best_want;
          UNIT_LOG(loglevel, best, "Defending city");
          dai_unit_new_task(ait, best, AIUNIT_DEFEND_HOME, pcity->tile);
          count++;
        }
      }
    }
    CITY_LOG(LOG_DEBUG, pcity, "Evaluating defense: %d defense, %d incoming"
             ", %d defenders (out of %d)", total_defense, total_attack, count,
             unit_list_size(pcity->tile->units));
  } city_list_iterate_end;
}

/**************************************************************************
  Master manage unit function.

  A manage function should set the unit to 'done' when it should no
  longer be touched by this code, and its role should be reset to IDLE
  when its role has accomplished its mission or the manage function
  fails to have or no longer has any use for the unit.
**************************************************************************/
void rai_manage_units(struct ai_type *ait, struct player *pplayer)
{
  TIMING_LOG(AIT_AIRLIFT, TIMER_START);
  rai_airlift(ait, pplayer);
  TIMING_LOG(AIT_AIRLIFT, TIMER_STOP);

  /* Clear previous orders, if desirable, here. */
  unit_list_iterate(pplayer->units, punit) {
    struct unit_ai *unit_data = def_ai_unit_data(punit, ait);

    unit_data->done = FALSE;
    if (unit_data->task == AIUNIT_DEFEND_HOME) {
      dai_unit_new_task(ait, punit, AIUNIT_NONE, NULL);
    }
  } unit_list_iterate_end;

  /* Find and set city defenders first - figure out which units are
   * allowed to leave home. */
  rai_set_defenders(ait, pplayer);

  unit_list_iterate_safe(pplayer->units, punit) {
    if ((!unit_transported(punit) || unit_owner(unit_transport_get(punit)) != pplayer)
         && !def_ai_unit_data(punit, ait)->done) {
      /* Though it is usually the passenger who drives the transport,
       * the transporter is responsible for managing its passengers. */
      rai_manage_unit(ait, pplayer, punit);
    }
  } unit_list_iterate_safe_end;
}




/********************************************************************** 
 Freeciv - Copyright (C) 2004 - The Freeciv Team
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

#include <stdio.h>
#include <string.h>

/* utility */
#include "mem.h"
#include "log.h"
#include "support.h" 
#include "timing.h"

/* common */
#include "city.h"
#include "game.h"
#include "government.h"
#include "map.h"
#include "movement.h"
#include "packets.h"
#include "player.h"

/* common/aicore */
#include "pf_tools.h"

/* server */
#include "citytools.h"
#include "maphand.h"
#include "srv_log.h"
#include "unithand.h"
#include "unittools.h"

/* server/advisors */
#include "advdata.h"
#include "advgoto.h"
#include "advtools.h"
#include "autosettlers.h"
#include "infracache.h"

/* ai */
#include "aidata.h"
#include "aicity.h"
#include "aiferry.h"
#include "aiplayer.h"
#include "aitools.h"
#include "aiunit.h"
#include "citymap.h"

#include "raisettler.h"

#include "rand.h"
#include "player.h"


/* COMMENTS */
/* 
   This code tries hard to do the right thing, including looking
   into the future (wrt to government), and also doing this in a
   modpack friendly manner. However, there are some pieces missing.

   A tighter integration into the city management code would 
   give more optimal city placements, since existing cities could
   move their workers around to give a new city better placement.
   Occasionally you will see cities being placed sub-optimally
   because the best city center tile is taken when another tile
   could have been worked instead by the city that took it.

   The code is not generic enough. It assumes smallpox too much,
   and should calculate with a future city of a bigger size.

   We need to stop the build code from running this code all the
   time and instead try to complete what it has started.
*/

/* Stop looking too hard for better tiles to found a new city at when
 * settler finds location with at least RESULT_IS_ENOUGH want
 * points. See city_desirability() for how base want is computed
 * before amortizing it.
 *
 * This is a big WAG to save a big amount of CPU. */
#define RESULT_IS_ENOUGH 250

#define FERRY_TECH_WANT 500

#define GROWTH_PRIORITY 15

/* Perfection gives us an idea of how long to search for optimal
 * solutions, instead of going for quick and dirty solutions that
 * waste valuable time. Decrease for maps where good city placements
 * are hard to find. Lower means more perfection. */
#define PERFECTION 3

/* How much to deemphasise the potential for city growth for city
 * placements. Decrease this value if large cities are important
 * or planned. Increase if running strict smallpox. */
#define GROWTH_POTENTIAL_DEEMPHASIS 8

/* Percentage bonus to city locations near an ocean. */
#define NAVAL_EMPHASIS 20

/* Modifier for defense bonus that is applied to city location want. 
 * This is % of defense % to increase want by. */
#define DEFENSE_EMPHASIS 20

struct tile_data_cache {
  char food;    /* food output of the tile */
  char trade;   /* trade output of the tile */
  char shield;  /* shield output of the tile */

  int sum;      /* weighted sum of the tile output (used by AI) */

  int reserved; /* reservation for this tile; used by print_citymap() */

  int turn;     /* the turn the values were calculated */
};

struct potentialImprovement{
	enum unit_activity activity;
	struct act_tgt target;
};

struct tile_data_cache *tile_data_cache_new(void);
struct tile_data_cache *
  tile_data_cache_copy(const struct tile_data_cache *ptdc);

struct cityresult {
  struct tile *tile;
  int total;              /* total value of position */
  int result;             /* amortized and adjusted total value */
  int corruption, waste;
  bool overseas;          /* have to use boat to get there */
  bool virt_boat;         /* virtual boat was used in search, 
                           * so need to build one */

  struct {
    struct tile_data_cache *tdc;  /* values of city center; link to the data
                                   * in tdc_hash. */
  } city_center;

  struct {
    struct tile *tile;            /* best other tile */
    int cindex;                   /* city-relative index for other tile */
    struct tile_data_cache *tdc;  /* value of best other tile; link to the
                                   * data in tdc_hash. */
  } best_other;

  int remaining;          /* value of all other tiles */

  /* Save the result for print_citymap(). */
  struct tile_data_cache_hash *tdc_hash;

  int city_radius_sq;     /* current squared radius of the city */
};

static bool rai_do_build_city(struct ai_type *ait, struct player *pplayer,
                              struct unit *punit);


/*****************************************************************************
  Allocate tile data cache
*****************************************************************************/
struct tile_data_cache *tile_data_cache_new(void)
{
  struct tile_data_cache *ptdc_copy = fc_calloc(1, sizeof(*ptdc_copy));

  /* Set the turn the tile data cache was created. */
  ptdc_copy->turn = game.info.turn;

  return ptdc_copy;
}

/*****************************************************************************
  Make copy of tile data cache
*****************************************************************************/
struct tile_data_cache *
  tile_data_cache_copy(const struct tile_data_cache *ptdc)
{
  struct tile_data_cache *ptdc_copy = tile_data_cache_new();

  fc_assert_ret_val(ptdc, NULL);

  ptdc_copy->shield = ptdc->shield;
  ptdc_copy->trade = ptdc->trade;
  ptdc_copy->food = ptdc->food;

  ptdc_copy->sum = ptdc->sum;
  ptdc_copy->reserved = ptdc->reserved;
  ptdc_copy->turn = ptdc->turn;

  return ptdc_copy;
}


/**************************************************************************
  Auto RANDOM settler that can also build cities.
**************************************************************************/
void rai_settler_run(struct ai_type *ait, struct player *pplayer,
		struct unit *punit, struct settlermap *state) {
	CHECK_UNIT(punit);

	struct tile *init_tile = unit_tile(punit);
	struct potentialImprovement *action;
	struct worker_task *chosenTask;

	struct genlist* actionList = genlist_new();

	if (unit_has_type_flag(punit, UTYF_CITIES)) {
		if (city_can_be_built_here(init_tile, punit)) {
			struct potentialMove *pMove = malloc(sizeof(struct potentialMove));
			pMove->type = build_city;
			pMove->moveInfo = NULL;
			genlist_append(actionList, pMove);
		}
	}

	collect_explorer_moves(punit, actionList);

	bool consider = TRUE;
	// Improving the tile currently standing on
	if (!adv_settler_safe_tile(pplayer, punit, init_tile)) {
		/* Too dangerous place */
		consider = FALSE;
	}
	/* Do not work on tiles that already have workers there. */
	unit_list_iterate(init_tile->units, aunit) {
		if (unit_owner(aunit) == pplayer
				&& aunit->id != punit->id
				&& unit_has_type_flag(aunit, UTYF_SETTLERS)) {
			consider = FALSE;
		}
	} unit_list_iterate_end;

	if (consider) {
		activity_type_iterate(act) {
			struct act_tgt target = { .type = ATT_SPECIAL, .obj.spe = S_LAST };

			if (act != ACTIVITY_BASE
					&& act != ACTIVITY_GEN_ROAD
					&& can_unit_do_activity_targeted_at(punit, act, &target,
							init_tile)){
				if(act != ACTIVITY_EXPLORE){
					//Add the action to a list to choose from
					struct potentialMove *pMove = malloc(sizeof(struct potentialMove));
					pMove->type = improvement;

					struct potentialImprovement *action=malloc(sizeof(struct potentialImprovement));
					action->activity = act;
					action->target = target;

					pMove->moveInfo = action;
					genlist_append(actionList, pMove);
				}
			}
		} activity_type_iterate_end;

		road_type_iterate(proad) {
			struct act_tgt target = { .type = ATT_ROAD, .obj.road = road_number(proad) };

			if (can_unit_do_activity_targeted_at(punit, ACTIVITY_GEN_ROAD, &target, init_tile)) {
				struct potentialMove *pMove = malloc(sizeof(struct potentialMove));
				pMove->type = improvement;

				struct potentialImprovement *action=malloc(sizeof(struct potentialImprovement));
				action->activity = ACTIVITY_GEN_ROAD;
				action->target = target;

				pMove->moveInfo = action;
				genlist_append(actionList, pMove);
			}
			else {
				road_deps_iterate(&(proad->reqs), pdep) {
					struct act_tgt dep_tgt = { .type = ATT_ROAD, .obj.road = road_number(pdep) };
					if (can_unit_do_activity_targeted_at(punit, ACTIVITY_GEN_ROAD,
							&dep_tgt, init_tile)) {
						//Add the action to a list to choose from
						struct potentialMove *pMove = malloc(sizeof(struct potentialMove));
						pMove->type = improvement;

						struct potentialImprovement *action=malloc(sizeof(struct potentialImprovement));
						action->activity = ACTIVITY_GEN_ROAD;
						action->target = target;

						pMove->moveInfo = action;
						genlist_append(actionList, pMove);
					}
				} road_deps_iterate_end;
			}
		} road_type_iterate_end;


		base_type_iterate(pbase) {
			struct act_tgt target = { .type = ATT_BASE, .obj.base = base_number(pbase) };
			if (can_unit_do_activity_targeted_at(punit, ACTIVITY_BASE, &target,
					init_tile)) {
				//Add the action to a list to choose from
				struct potentialMove *pMove = malloc(sizeof(struct potentialMove));
				pMove->type = improvement;

				struct potentialImprovement *action=malloc(sizeof(struct potentialImprovement));
				action->activity = ACTIVITY_BASE;
				action->target = target;

				pMove->moveInfo = action;
				genlist_append(actionList, pMove);
			} else {
				base_deps_iterate(&(pbase->reqs), pdep) {
					struct act_tgt dep_tgt = { .type = ATT_BASE, .obj.base = base_number(pdep) };
					if (can_unit_do_activity_targeted_at(punit, ACTIVITY_BASE,
							&dep_tgt, init_tile)) {
						//Add the action to a list to choose from
						struct potentialMove *pMove = malloc(sizeof(struct potentialMove));
						pMove->type = improvement;

						struct potentialImprovement *action=malloc(sizeof(struct potentialImprovement));
						action->activity = ACTIVITY_BASE;
						action->target = target;

						pMove->moveInfo = action;
						genlist_append(actionList, pMove);
					}
				} base_deps_iterate_end;
			}
		} base_type_iterate_end;
	}

	city_list_iterate(pplayer->cities, pcity) {
		struct worker_task *ptask = &pcity->server.task_req;
		if (ptask->ptile != NULL) {
			bool consider = TRUE;

			/* Do not go to tiles that already have workers there. */
			unit_list_iterate(ptask->ptile->units, aunit) {
				if (unit_owner(aunit) == pplayer
						&& aunit->id != punit->id
						&& unit_has_type_flag(aunit, UTYF_SETTLERS)) {
					consider = FALSE;
				}
			} unit_list_iterate_end;

			if (consider
					&& can_unit_do_activity_targeted_at(punit, ptask->act, &ptask->tgt,
							ptask->ptile)) {
				struct potentialMove *pMove = malloc(sizeof(struct potentialMove));
				pMove->type = request;

				struct worker_task_load_compatible *ptask_save = malloc(sizeof(struct worker_task_load_compatible));
				ptask_save->act = ptask->act;
				ptask_save->tgt = ptask->tgt;
				index_to_native_pos(&ptask_save->ptile_x,&ptask_save->ptile_y, tile_index(ptask->ptile));
				pMove->moveInfo = ptask_save;
				genlist_append(actionList, pMove);
			}
		}
	} city_list_iterate_end;



	struct potentialMove *chosen_action = genlist_get(actionList, fc_rand(genlist_size(actionList)));
	struct worker_task_load_compatible *chosen_req_task;
	switch(chosen_action->type){
	case explore:
		make_explorer_move(punit, chosen_action->moveInfo);
		break;
	case build_city:
		adv_unit_new_task(punit, AUT_BUILD_CITY, init_tile);
		if (def_ai_unit_data(punit, ait)->task == AIUNIT_BUILD_CITY) {
			if (!rai_do_build_city(ait, pplayer, punit)) {
				UNIT_LOG(LOG_DEBUG, punit, "could not make city on %s",
						tile_get_info_text(unit_tile(punit), TRUE, 0));
				dai_unit_new_task(ait, punit, AIUNIT_NONE, NULL);
				/* Only known way to end in here is that hut turned in to a city
				 * when settler entered tile. So this is not going to lead in any
				 * serious recursion. */
				rai_settler_run(ait, pplayer, punit, state);
			}
		}
		break;
	case improvement:
		action = chosen_action->moveInfo;
		if (punit->server.adv->task == AUT_AUTO_SETTLER) {
			if(punit->moves_left > 0){
				if (activity_requires_target(action->activity)) {
					unit_activity_handling_targeted(punit, action->activity, &action->target);
				} else {
					unit_activity_handling(punit, action->activity);
				}
				send_unit_info(NULL, punit); /* FIXME: probably duplicate */
			}
		}
		break;
	case request:
		chosen_req_task = chosen_action->moveInfo;
		if (chosen_req_task != NULL) {
			struct pf_path *path = NULL;
			int completion_time = 0;

			auto_settler_setup_work(pplayer, punit, state, 0, path,
					native_pos_to_tile(chosen_req_task->ptile_x, chosen_req_task->ptile_y), chosen_req_task->act, &chosen_req_task->tgt,
					completion_time);
		}
		break;
	default:
		break;
	}

	for(int i = 0; i < genlist_size(actionList); i++ ){
		struct potentialMove *toRemove = genlist_back(actionList);
		if(toRemove->type == explore){
			free(toRemove->moveInfo);
		}
		if(toRemove->type == improvement){
			free(toRemove->moveInfo);
		}
		if(toRemove->type == request){
			free(toRemove->moveInfo);
		}
		genlist_pop_back(actionList);
		free(toRemove);
	}
	genlist_destroy(actionList);

	return;
}



/**************************************************************************
  Build a city and initialize AI infrastructure cache.
**************************************************************************/
static bool rai_do_build_city(struct ai_type *ait, struct player *pplayer,
                              struct unit *punit)
{
  struct tile *ptile = unit_tile(punit);
  struct city *pcity;

  fc_assert_ret_val(pplayer == unit_owner(punit), FALSE);
  unit_activity_handling(punit, ACTIVITY_IDLE);

  /* Free city reservations */
  dai_unit_new_task(ait, punit, AIUNIT_NONE, NULL);

  pcity = tile_city(ptile);
  if (pcity) {
    /* This can happen for instance when there was hut at this tile
     * and it turned in to a city when settler entered tile. */
    log_debug("%s: There is already a city at (%d, %d)!",
              player_name(pplayer), TILE_XY(ptile));
    return FALSE;
  }
  if (!unit_build_city(pplayer, punit, city_name_suggestion(pplayer, ptile))) {
    /* It's an error when unit_build_city() says that request was illegal to begin with. */
    log_error("%s: Failed to build city at (%d, %d)",
              player_name(pplayer), TILE_XY(ptile));
    return FALSE;
  }

  pcity = tile_city(ptile);
  if (!pcity) {
    /* Write just debug log when city building didn't success for acceptable reason. */
    log_debug("%s: Failed to build city at (%d, %d)",
              player_name(pplayer), TILE_XY(ptile));
    return FALSE;
  }

  /* We have to rebuild at least the cache for this city.  This event is
   * rare enough we might as well build the whole thing.  Who knows what
   * else might be cached in the future? */
  fc_assert_ret_val(pplayer == city_owner(pcity), FALSE);
  initialize_infrastructure_cache(pplayer);

  /* Init ai.choice. Handling ferryboats might use it. */
  init_choice(&def_ai_city_data(pcity, ait)->choice);

  return TRUE;
}


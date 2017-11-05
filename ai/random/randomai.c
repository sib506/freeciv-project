/***********************************************************************
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

/* common */
#include "ai.h"
#include "player.h"

/* server */
#include "srv_log.h"
#include "sernet.h"

/* server/advisors */
#include "advdata.h"
#include "autosettlers.h"

/* ai/default */
#include "advdiplomacy.h"
#include "advdomestic.h"
#include "advmilitary.h"
#include "aicity.h"
#include "aidata.h"
#include "aiferry.h"
#include "aihand.h"
#include "ailog.h"
#include "aiplayer.h"
#include "aisettler.h"
#include "aitools.h"

/* ai/random */
#include "raiunit.h"
#include "raisettler.h"

#include <randomai.h>

/* This function is only needed as symbol in module. Nevertheless
   gcc requires that there is prior prototype. */
const char *fc_ai_random_capstr(void);

static struct ai_type *self = NULL;

/**************************************************************************
  Set pointer to ai type of the r ai.
**************************************************************************/
static void random_ai_set_self(struct ai_type *ai)
{
  self = ai;
}

/**************************************************************************
  Get pointer to ai type of the random ai.
**************************************************************************/
static struct ai_type *random_ai_get_self(void)
{
  return self;
}

/**************************************************************************
  Return module capability string
**************************************************************************/
const char *fc_ai_random_capstr(void)
{
  return FC_AI_MOD_CAPSTR;
}

/**************************************************************************
  Call default ai with random ai type as parameter.
**************************************************************************/
static void rai_player_alloc(struct player *pplayer)
{
  struct ai_type *deftype = random_ai_get_self();

  dai_player_alloc(deftype, pplayer);
}

/**************************************************************************
  Call default ai with random ai type as parameter.
**************************************************************************/
static void rai_player_free(struct player *pplayer)
{
  struct ai_type *deftype = random_ai_get_self();

  dai_player_free(deftype, pplayer);
}

/**************************************************************************
  Call default ai with random ai type as parameter.
**************************************************************************/
static void rai_player_save(struct player *pplayer, struct section_file *file,
                     int plrno)
{
  struct ai_type *deftype = random_ai_get_self();

  dai_player_save(deftype, "ai", pplayer, file, plrno);
}

/**************************************************************************
  Call default ai with random ai type as parameter.
**************************************************************************/
static void rai_player_load(struct player *pplayer,
                            const struct section_file *file, int plrno)
{
  struct ai_type *deftype = random_ai_get_self();

  dai_player_load(deftype, "ai", pplayer, file, plrno);
}

/**************************************************************************
  Call default ai with random ai type as parameter.
**************************************************************************/
static void rai_assess_danger_player(struct player *pplayer)
{
  struct ai_type *deftype = random_ai_get_self();

  dai_assess_danger_player(deftype, pplayer);
}

/**************************************************************************
  Call default ai with random ai type as parameter.
**************************************************************************/
static void rai_data_phase_begin(struct player *pplayer, bool is_new_phase)
{
  struct ai_type *deftype = random_ai_get_self();

  dai_data_phase_begin(deftype, pplayer, is_new_phase);
}

/**************************************************************************
  Call default ai with random ai type as parameter.
**************************************************************************/
static void rai_data_phase_finished(struct player *pplayer)
{
  struct ai_type *deftype = random_ai_get_self();

  dai_data_phase_finished(deftype, pplayer);
}

/**************************************************************************
  Call default ai with random ai type as parameter.
**************************************************************************/
static void rai_city_alloc(struct city *pcity)
{
  struct ai_type *deftype = random_ai_get_self();

  dai_city_alloc(deftype, pcity);
}

/**************************************************************************
  Call default ai with random ai type as parameter.
**************************************************************************/
static void rai_city_free(struct city *pcity)
{
  struct ai_type *deftype = random_ai_get_self();

  dai_city_free(deftype, pcity);
}

/**************************************************************************
  Call default ai with random ai type as parameter.
**************************************************************************/
static void rai_city_save(struct section_file *file, const struct city *pcity,
                          const char *citystr)
{
  struct ai_type *deftype = random_ai_get_self();

  dai_city_save(deftype, "ai", file, pcity, citystr);
}

/**************************************************************************
  Call default ai with random ai type as parameter.
**************************************************************************/
static void rai_city_load(const struct section_file *file, struct city *pcity,
                          const char *citystr)
{
  struct ai_type *deftype = random_ai_get_self();

  dai_city_load(deftype, "ai", file, pcity, citystr);
}

/**************************************************************************
  Call default ai with random ai type as parameter.
**************************************************************************/
static void rai_build_adv_override(struct city *pcity, struct adv_choice *choice)
{
  struct ai_type *deftype = random_ai_get_self();

  dai_build_adv_override(deftype, pcity, choice);
}

/**************************************************************************
  Call default ai with random ai type as parameter.
**************************************************************************/
static void rai_wonder_city_distance(struct player *pplayer, struct adv_data *adv)
{
  struct ai_type *deftype = random_ai_get_self();

  dai_wonder_city_distance(deftype, pplayer, adv);
}

/**************************************************************************
  Call default ai with random ai type as parameter.
**************************************************************************/
static void rai_build_adv_init(struct player *pplayer)
{
  struct ai_type *deftype = random_ai_get_self();

  dai_build_adv_init(deftype, pplayer);
}

/**************************************************************************
  Call default ai with random ai type as parameter.
**************************************************************************/
static void rai_build_adv_adjust(struct player *pplayer, struct city *wonder_city)
{
  struct ai_type *deftype = random_ai_get_self();

  dai_build_adv_adjust(deftype, pplayer, wonder_city);
}

/**************************************************************************
  Call default ai with random ai type as parameter.
**************************************************************************/
static void rai_units_ruleset_init(void)
{
  struct ai_type *deftype = random_ai_get_self();

  dai_units_ruleset_init(deftype);
}

/**************************************************************************
  Call default ai with random ai type as parameter.
**************************************************************************/
static void rai_units_ruleset_close(void)
{
  struct ai_type *deftype = random_ai_get_self();

  dai_units_ruleset_close(deftype);
}

/**************************************************************************
  Call default ai with random ai type as parameter.
**************************************************************************/
static void rai_unit_init(struct unit *punit)
{
  struct ai_type *deftype = random_ai_get_self();

  dai_unit_init(deftype, punit);
}

/**************************************************************************
  Call default ai with random ai type as parameter.
**************************************************************************/
static void rai_unit_close(struct unit *punit)
{
  struct ai_type *deftype = random_ai_get_self();

  dai_unit_close(deftype, punit);
}

/**************************************************************************
  Call default ai with random ai type as parameter.
**************************************************************************/
static void rai_ferry_init_ferry(struct unit *ferry)
{
  struct ai_type *deftype = random_ai_get_self();

  dai_ferry_init_ferry(deftype, ferry);
}

/**************************************************************************
  Call default ai with random ai type as parameter.
**************************************************************************/
static void rai_ferry_transformed(struct unit *ferry, struct unit_type *old)
{
  struct ai_type *deftype = random_ai_get_self();

  dai_ferry_transformed(deftype, ferry, old);
}

/**************************************************************************
  Call default ai with random ai type as parameter.
**************************************************************************/
static void rai_ferry_lost(struct unit *punit)
{
  struct ai_type *deftype = random_ai_get_self();

  dai_ferry_lost(deftype, punit);
}

/**************************************************************************
  Call default ai with random ai type as parameter.
**************************************************************************/
static void rai_unit_turn_end(struct unit *punit)
{
  struct ai_type *deftype = random_ai_get_self();

  dai_unit_turn_end(deftype, punit);
}

/**************************************************************************
  Call default ai with random ai type as parameter.
**************************************************************************/
static void rai_unit_move_or_attack(struct unit *punit, struct tile *ptile,
                                    struct pf_path *path, int step)
{
  struct ai_type *deftype = random_ai_get_self();

  dai_unit_move_or_attack(deftype, punit, ptile, path, step);
}

/**************************************************************************
  Call default ai with random ai type as parameter.
**************************************************************************/
static void rai_unit_new_adv_task(struct unit *punit, enum adv_unit_task task,
                                  struct tile *ptile)
{
  struct ai_type *deftype = random_ai_get_self();

  dai_unit_new_adv_task(deftype, punit, task, ptile);
}

/**************************************************************************
  Call default ai with random ai type as parameter.
**************************************************************************/
static void rai_unit_save(struct section_file *file, const struct unit *punit,
                          const char *unitstr)
{
  struct ai_type *deftype = random_ai_get_self();

  dai_unit_save(deftype, "", file, punit, unitstr);
}

/**************************************************************************
  Call default ai with random ai type as parameter.
**************************************************************************/
static void rai_unit_load(const struct section_file *file, struct unit *punit,
                          const char *unitstr)
{
  struct ai_type *deftype = random_ai_get_self();

  dai_unit_load(deftype, "", file, punit, unitstr);
}

/**************************************************************************
  Call default ai with random ai type as parameter.
**************************************************************************/
static void rai_auto_settler_reset(struct player *pplayer)
{
  struct ai_type *deftype = random_ai_get_self();

  dai_auto_settler_reset(deftype, pplayer);
}





/**************************************************************************
  Call default ai with random ai type as parameter.
**************************************************************************/
static void rai_auto_settler_run(struct player *pplayer, struct unit *punit,
                                 struct settlermap *state)
{
  struct ai_type *deftype = random_ai_get_self();

  rai_settler_run(deftype, pplayer, punit, state);
}

/**************************************************************************
  Call default ai with random ai type as parameter.
**************************************************************************/
static void rai_auto_settler_cont(struct player *pplayer, struct unit *punit,
                                  struct settlermap *state)
{
  struct ai_type *deftype = random_ai_get_self();

  dai_auto_settler_cont(deftype, pplayer, punit, state);
}

/**************************************************************************
  Call default ai with random ai type as parameter.
**************************************************************************/
static void rai_do_first_activities(struct player *pplayer)
{
  struct ai_type *deftype = random_ai_get_self();

  TIMING_LOG(AIT_ALL, TIMER_START);
  dai_assess_danger_player(deftype, pplayer);
  /* TODO: Make assess_danger save information on what is threatening
   * us and make dai_manage_units and Co act upon this information, trying
   * to eliminate the source of danger */

  TIMING_LOG(AIT_UNITS, TIMER_START);
  rai_manage_units(deftype, pplayer);
  TIMING_LOG(AIT_UNITS, TIMER_STOP);
  /* STOP.  Everything else is at end of turn. */

  TIMING_LOG(AIT_ALL, TIMER_STOP);

  flush_packets(); /* AIs can be such spammers... */

}

/**************************************************************************
  Call default ai with random ai type as parameter.
**************************************************************************/
static void rai_diplomacy_actions(struct player *pplayer)
{
  struct ai_type *deftype = random_ai_get_self();

  dai_diplomacy_actions(deftype, pplayer);
}

/**************************************************************************
  Call default ai with random ai type as parameter.
**************************************************************************/
static void rai_do_last_activities(struct player *pplayer)
{
  struct ai_type *deftype = random_ai_get_self();

  dai_do_last_activities(deftype, pplayer);
}

/**************************************************************************
  Call default ai with random ai type as parameter.
**************************************************************************/
static void rai_treaty_evaluate(struct player *pplayer, struct player *aplayer,
                                struct Treaty *ptreaty)
{
  struct ai_type *deftype = random_ai_get_self();

  dai_treaty_evaluate(deftype, pplayer, aplayer, ptreaty);
}

/**************************************************************************
  Call default ai with random ai type as parameter.
**************************************************************************/
static void rai_treaty_accepted(struct player *pplayer, struct player *aplayer,
                                struct Treaty *ptreaty)
{
  struct ai_type *deftype = random_ai_get_self();

  dai_treaty_accepted(deftype, pplayer, aplayer, ptreaty);
}

/**************************************************************************
  Call default ai with random ai type as parameter.
**************************************************************************/
static void rai_diplomacy_first_contact(struct player *pplayer,
                                        struct player *aplayer)
{
  struct ai_type *deftype = random_ai_get_self();

  dai_diplomacy_first_contact(deftype, pplayer, aplayer);
}

/**************************************************************************
  Call default ai with random ai type as parameter.
**************************************************************************/
static void rai_incident(enum incident_type type, struct player *violator,
                         struct player *victim)
{
  struct ai_type *deftype = random_ai_get_self();

  dai_incident(deftype, type, violator, victim);
}

/**************************************************************************
  Call default ai with random ai type as parameter.
**************************************************************************/
static void rai_city_log(char *buffer, int buflength, const struct city *pcity)
{
  struct ai_type *deftype = random_ai_get_self();

  dai_city_log(deftype, buffer, buflength, pcity);
}

/**************************************************************************
  Call default ai with random ai type as parameter.
**************************************************************************/
static void rai_unit_log(char *buffer, int buflength, const struct unit *punit)
{
  struct ai_type *deftype = random_ai_get_self();

  dai_unit_log(deftype, buffer, buflength, punit);
}

/**************************************************************************
  Call default ai with random ai type as parameter.
**************************************************************************/
static void rai_consider_plr_dangerous(struct player *plr1, struct player *plr2,
                                       enum danger_consideration *result)
{
  struct ai_type *deftype = random_ai_get_self();

  dai_consider_plr_dangerous(deftype, plr1, plr2, result);
}

/**************************************************************************
  Call default ai with random ai type as parameter.
**************************************************************************/
static void rai_consider_tile_dangerous(struct tile *ptile, struct unit *punit,
                                        enum danger_consideration *result)
{
  struct ai_type *deftype = random_ai_get_self();

  dai_consider_tile_dangerous(deftype, ptile, punit, result);
}

/**************************************************************************
  Call default ai with random ai type as parameter.
**************************************************************************/
static void rai_consider_wonder_city(struct city *pcity, bool *result)
{
  struct ai_type *deftype = random_ai_get_self();

  dai_consider_wonder_city(deftype, pcity, result);
}

/**************************************************************************
  Setup player ai_funcs function pointers.
**************************************************************************/
bool fc_ai_random_setup(struct ai_type *ai)
{
  random_ai_set_self(ai);

  strncpy(ai->name, "random", sizeof(ai->name));

  /* ai->funcs.game_free = NULL; */

  ai->funcs.player_alloc = rai_player_alloc;
  ai->funcs.player_free = rai_player_free;
  ai->funcs.player_save = rai_player_save;
  ai->funcs.player_load = rai_player_load;
  ai->funcs.gained_control = rai_assess_danger_player;
  /* ai->funcs.lost_control = NULL; */
  ai->funcs.split_by_civil_war = rai_assess_danger_player;

  ai->funcs.phase_begin = rai_data_phase_begin;
  ai->funcs.phase_finished = rai_data_phase_finished;

  ai->funcs.city_alloc = rai_city_alloc;
  ai->funcs.city_free = rai_city_free;
  /*
    ai->funcs.city_got = NULL;
    ai->funcs.city_lost = NULL;
  */
  ai->funcs.city_save = rai_city_save;
  ai->funcs.city_load = rai_city_load;
  ai->funcs.choose_building = rai_build_adv_override;
  ai->funcs.build_adv_prepare = rai_wonder_city_distance;
  ai->funcs.build_adv_init = rai_build_adv_init;
  ai->funcs.build_adv_adjust_want = rai_build_adv_adjust;

  ai->funcs.units_ruleset_init = rai_units_ruleset_init;
  ai->funcs.units_ruleset_close = rai_units_ruleset_close;

  /* FIXME: We should allocate memory only for units owned by
     default ai in unit_got. We track no data
     about enemy units.
     But advisors code still depends on some default ai data (role) to be
     always allocated. */
  /*
    ai->funcs.unit_alloc = NULL;
    ai->funcs.unit_free = NULL;
    ai->funcs.unit_got = dai_unit_init;
    ai->funcs.unit_lost = dai_unit_close;
  */
  ai->funcs.unit_alloc = rai_unit_init;
  ai->funcs.unit_free = rai_unit_close;
  ai->funcs.unit_got = rai_ferry_init_ferry;
  ai->funcs.unit_lost = rai_ferry_lost;
  ai->funcs.unit_transformed = rai_ferry_transformed;

  ai->funcs.unit_turn_end = rai_unit_turn_end;
  ai->funcs.unit_move = rai_unit_move_or_attack;
  ai->funcs.unit_task = rai_unit_new_adv_task;

  ai->funcs.unit_save = rai_unit_save;
  ai->funcs.unit_load = rai_unit_load;

  ai->funcs.settler_reset = rai_auto_settler_reset;
  ai->funcs.settler_run = rai_auto_settler_run;
  ai->funcs.settler_cont = rai_auto_settler_cont;

  ai->funcs.first_activities = rai_do_first_activities;
  ai->funcs.diplomacy_actions = rai_diplomacy_actions;
  ai->funcs.last_activities = rai_do_last_activities;
  ai->funcs.treaty_evaluate = rai_treaty_evaluate;
  ai->funcs.treaty_accepted = rai_treaty_accepted;
  ai->funcs.first_contact = rai_diplomacy_first_contact;
  ai->funcs.incident = rai_incident;

  ai->funcs.log_fragment_city = rai_city_log;
  ai->funcs.log_fragment_unit = rai_unit_log;

  ai->funcs.consider_plr_dangerous = rai_consider_plr_dangerous;
  ai->funcs.consider_tile_dangerous = rai_consider_tile_dangerous;
  ai->funcs.consider_wonder_city = rai_consider_wonder_city;

  ai->funcs.refresh = NULL;

  return TRUE;
}

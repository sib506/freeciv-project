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
#ifndef FC__RAIUNIT_H
#define FC__RAIUNIT_H

/* common */
#include "combat.h"
#include "fc_types.h"
#include "unittype.h"
#include "autoexplorer.h"

struct pf_map;
struct pf_path;

struct section_file;

/* Simple military macros */

extern struct unit_type *simple_ai_types[U_LAST];

#define RAMPAGE_ANYTHING                 1
#define RAMPAGE_HUT_OR_BETTER        99998
#define RAMPAGE_FREE_CITY_OR_BETTER  99999
#define BODYGUARD_RAMPAGE_THRESHOLD (SHIELD_WEIGHTING * 4)

void rai_manage_units(struct ai_type *ait, struct player *pplayer);
void rai_manage_unit(struct ai_type *ait, struct player *pplayer,
                     struct unit *punit);
void rai_manage_military(struct ai_type *ait, struct player *pplayer,
                         struct unit *punit);
void rai_manage_military_random(struct ai_type *ait, struct player *pplayer,
        struct unit *punit);
struct city *find_nearest_safe_city(struct unit *punit);
int look_for_charge(struct ai_type *ait, struct player *pplayer,
                    struct unit *punit,
                    struct unit **aunit, struct city **acity);

bool rai_military_rampage(struct unit *punit, int thresh_adj,
                          int thresh_move);


bool find_beachhead(const struct player *pplayer, struct pf_map *ferry_map,
                    struct tile *dest_tile,
                    const struct unit_type *cargo_type,
                    struct tile **ferry_dest, struct tile **beachhead_tile);
int find_something_to_kill(struct ai_type *ait, struct player *pplayer,
                           struct unit *punit,
                           struct tile **pdest_tile, struct pf_path **ppath,
                           struct pf_map **pferrymap,
                           struct unit **pferryboat,
                           struct unit_type **pboattype,
                           int *pmove_time);

int build_cost_balanced(const struct unit_type *punittype);
int unittype_def_rating_sq(const struct unit_type *att_type,
			   const struct unit_type *def_type,
			   const struct player *def_player,
                           struct tile *ptile, bool fortified, int veteran);
int kill_desire(int benefit, int attack, int loss, int vuln, int attack_count);

bool is_on_unit_upgrade_path(const struct unit_type *test,
			     const struct unit_type *base);

enum unit_move_type rai_uclass_move_type(const struct unit_class *pclass);

#define simple_ai_unit_type_iterate(_ut)				\
{									\
  struct unit_type *_ut;						\
  int _ut##_index = 0;							\
  while (NULL != (_ut = simple_ai_types[_ut##_index++])) {

#define simple_ai_unit_type_iterate_end					\
  }									\
}

struct unit_type *rai_role_utype_for_move_type(struct city *pcity, int role,
                                               enum unit_move_type mt);

#endif  /* FC__AIUNIT_H */

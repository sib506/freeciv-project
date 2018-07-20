/********************************************************************** 
 Freeciv - Copyright (C) 2002 - The Freeciv Team
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
***********************************************************************/
#ifndef FC__RAISETTLER_H
#define FC__RAISETTLER_H

/* utility */
#include "support.h"            /* bool type */

/* common */
#include "city.h"
#include "fc_types.h"
#include "autoexplorer.h"

struct ai_data;
struct ai_plr;
struct tile_data_cache;

void rai_auto_settler_init(struct ai_plr *ai);
void rai_auto_settler_free(struct ai_plr *ai);

void rai_settler_run(struct ai_type *ait, struct player *pplayer,
                          struct unit *punit, struct settlermap *state);




#endif /* FC__AISETTLER_H */

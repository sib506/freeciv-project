/********************************************************************** 
 Freeciv - Copyright (C) 2004 - The Freeciv Project
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
***********************************************************************/
#ifndef FC__AUTOEXPLORER_H
#define FC__AUTOEXPLORER_H

struct unit;

enum moveType{
	explore, sentry, fortify, pillage, rage, build_city, improvement, request
};

struct potentialMove {
	enum moveType type;
	void * moveInfo;
};

struct move_tile_natcoord {
	int x;
	int y;
};

enum unit_move_result manage_auto_explorer(struct unit *punit);
enum unit_move_result manage_random_auto_explorer(struct unit *punit);
enum unit_move_result manage_random_auto_explorer2(struct unit *punit);

void collect_explorer_moves(struct unit *punit, struct genlist *moveList);
enum unit_move_result make_explorer_move(struct unit *punit, struct move_tile_natcoord *move_coord);
void free_explorer_moves(struct genlist *moveList);

#endif /* FC__AUTOEXPLORER_H */

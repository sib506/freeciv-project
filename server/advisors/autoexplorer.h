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
	explore, sentry, fortify, pillage, rage
};

struct potentialMove {
	enum moveType type;
	void * moveInfo;
};

enum unit_move_result manage_auto_explorer(struct unit *punit);
enum unit_move_result manage_random_auto_explorer(struct unit *punit);
enum unit_move_result manage_random_auto_explorer2(struct unit *punit);
enum unit_move_result move_randm_auto_explorer(struct unit *punit, struct tile *move_tile);
void collect_random_explorer_moves(struct unit *punit, struct genlist *moveList);

#endif /* FC__AUTOEXPLORER_H */

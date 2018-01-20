#ifndef FC__MCTS_H__
#define FC__MCTS_H__

#ifdef HAVE_CONFIG_H
#include <fc_config.h>
#endif

/* common */
#include "player.h"

void mcts_best_move(struct player *pplayer);
int find_index_of_unit(struct unit punit, struct genlist *player_moves);
struct potentialMove* return_unit_move(int move_no, int unit_list_index,
		struct genlist *player_moves);

#endif

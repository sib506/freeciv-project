#ifndef FC__MCTS_H__
#define FC__MCTS_H__

#ifdef HAVE_CONFIG_H
#include <fc_config.h>
#endif

/* common */
#include "player.h"

#include <stdbool.h>
#include "score.h"

extern bool mcts_mode;
extern int rollout_depth;
extern bool move_chosen;
extern enum mcts_stage current_mcts_stage;
extern bool end_of_turn;

enum mcts_stage{
	selection, expansion, simulation, backpropagation
};

void mcts_best_move(struct player *pplayer);
int find_index_of_unit(struct unit *punit, struct genlist *player_moves);
struct potentialMove* return_unit_index_move(int move_no, int unit_list_index,
		struct genlist *player_moves);
struct potentialMove* return_punit_move(struct unit *punit);
void backpropagate(bool interrupt);

#endif

#ifndef FC__MCTS_H__
#define FC__MCTS_H__

#ifdef HAVE_CONFIG_H
#include <fc_config.h>
#endif

/* common */
#include "player.h"

#include <stdbool.h>
#include "score.h"

extern bool game_over;
extern bool mcts_mode;
extern int rollout_depth;
extern bool move_chosen;
extern enum mcts_stage current_mcts_stage;
extern bool reset;
extern bool pending_game_move;

enum mcts_stage{
	selection, expansion, simulation, backpropagation
};

enum pruning_level{
	no_pruning, random_pruning
};

/**
 * MCTS logic - selects the correct mcts phase
 * for the given player and invokes the required logic
 *
 * @param pplayer the current player
 */
void mcts_best_move(struct player *pplayer);

/**
 * Returns the index of a given unit within the move list
 * enabling the moves for that unit to be found
 *
 * @param punit the unit we wish to find the index for
 * @param player_moves the move list which we wish to find the index in
 * @return Integer index where that units moves are stored
 */
int find_index_of_unit(struct unit *punit, struct genlist *player_moves);
struct potentialMove* return_unit_index_move(int move_no, int unit_list_index,
		struct genlist *player_moves);

/**
 * Returns a move for a unit based on the current position in
 * the MCTS tree
 *
 * @param punit the unit to return a move for
 * @return Boolean value of whether we are at the root node
 */
struct potentialMove* return_punit_move(struct unit *punit);

/**
 * Backpropogates the terminal score to the required MCTS nodes
 * after a simulation has been performed
 *
 * @param interrupt denotes whether the backprop was caused by the game being interrupted
 * 	i.e. we didn't rollout to our maximum depth
 * @return Boolean value of whether we are at the root node
 */
void backpropagate(bool interrupt);


/**
 * Returns if we are currently at the root node of
 * the MCTS tree
 *
 * @return Boolean value of whether we are at the root node
 */
bool at_root_of_tree();

void log_time_to_file(char* text);

#endif

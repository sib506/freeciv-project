#include "mcts_pruning.h"

struct genlist* random_mcts_pruning(struct genlist* all_moves, bool * need_to_free){
	if(genlist_size(all_moves) <= MAX_NO_UNIT_MOVES){
		*need_to_free = FALSE;
		return all_moves;
	}

	//Create new genlist
	struct genlist* pruned_moves = genlist_new();

	//Pick N moves for unit
	for(int i = 0; i < MAX_NO_UNIT_MOVES; i++){
		// Get random number
		int rand_index = rand() % genlist_size(all_moves);
		// Append index item to new list
		struct genlist_link * action_link = genlist_link(all_moves, rand_index);
		genlist_append(pruned_moves, action_link->dataptr);
		genlist_erase(all_moves, action_link);
		// Remove all moves from list
		*need_to_free = TRUE;
	}

	//Return new list
	return pruned_moves;
}


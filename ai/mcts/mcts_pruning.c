#include "mcts_pruning.h"

#define MAX_NO_UNIT_MOVES 3

struct genlist* random_mcts_general_pruning(struct genlist* all_moves, bool * need_to_free){
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


struct genlist* random_mcts_settler_pruning(struct genlist* all_moves, bool * need_to_free,
		struct unit * punit){
	if(genlist_size(all_moves) <= MAX_NO_UNIT_MOVES){
		*need_to_free = FALSE;
		return all_moves;
	}

	//Create new genlist
	struct genlist* pruned_moves = genlist_new();

	// If we are a building city unit then should always be able to build a city
	if (unit_has_type_flag(punit, UTYF_CITIES)) {
		int can_build_city = 0;

		for(int i=0; i < genlist_size(all_moves); i++) {
			struct genlist_link * action_link = genlist_link(all_moves,	i);
			struct potentialMove *pMove = action_link->dataptr;
			if(pMove->type == build_city) {
				genlist_append(pruned_moves, action_link->dataptr);
				genlist_erase(all_moves, action_link);
				can_build_city++;
			}
		}

//		//Fill remaining N-1 moves for unit
		for (int i = 0; i < MAX_NO_UNIT_MOVES-can_build_city; i++) {
			// Get random number
			int rand_index = rand() % genlist_size(all_moves);
			// Append index item to new list
			struct genlist_link * action_link = genlist_link(all_moves,
					rand_index);
			genlist_append(pruned_moves, action_link->dataptr);
			genlist_erase(all_moves, action_link);
		}

	} else {
		//Pick N moves for unit
		for (int i = 0; i < MAX_NO_UNIT_MOVES; i++) {
			// Get random number
			int rand_index = rand() % genlist_size(all_moves);
			// Append index item to new list
			struct genlist_link * action_link = genlist_link(all_moves,
					rand_index);
			genlist_append(pruned_moves, action_link->dataptr);
			genlist_erase(all_moves, action_link);
		}
	}

	// Remove all moves from old list
	*need_to_free = TRUE;

	//Return new list
	return pruned_moves;
}


#include "mcts_node.h"
#include "aisettler.h"
#include "idex.h"
#include <string.h>

#define BRANCH_LIMITED	1
#define BRANCH_LIMIT	30

mcts_node* create_node(int p_index, struct genlist *possible_moves, int move,
		mcts_node *parent) {
	mcts_node* node = (mcts_node*) malloc(sizeof(mcts_node));

	node->uninitialised = TRUE;
	node->player_index = p_index;
	node->move_no = move;

	node->wins = 0;
	node->visits = 0;

	node->parent = parent;
	node->children = genlist_new();

	node->all_moves = possible_moves;
	node->total_no_moves = calc_number_moves(possible_moves);
	node->untried_moves = init_untried_moves(node->total_no_moves);

	return node;
}

mcts_node* create_root_node(int p_index, struct genlist *all_possible_moves) {
	return create_node(p_index, all_possible_moves, 0, NULL);
}

mcts_node* add_child_node(mcts_node* parent, int move_no) {
	// How do we know what the next player will be?
	mcts_node *child_node = create_node(-1, NULL,move_no, parent);
	genlist_append(parent->children, child_node);
	return child_node;
}

void free_node(mcts_node *node) {
	// Can assume that all child nodes have already been removed
	// Need to clear: children, all_moves, untried_moves
	// Then clear yourself

	//Child nodes:
	genlist_destroy(node->children);

	//All_Moves
	int no_of_units = genlist_size(node->all_moves);
	for(int i = 0; i < no_of_units; i++){
		struct unit_moves *tmp = genlist_get(node->all_moves,i);
		struct unit *punit = idex_lookup_unit(tmp->id);
		if(tmp->type == settler){
			free_settler_moves(tmp->moves);
		} else if (tmp->type == military){
			free_military_moves(tmp->moves);
		} else if (tmp->type == explorer){
			free_explorer_moves(tmp->moves);
		}
		free(tmp);
	}

	genlist_destroy(node->all_moves);

	// Untried moves
	genlist_destroy(node->untried_moves);

	//This node
	free(node);

}

void update_node(int32_t result, mcts_node* node) {
	node->visits++;
	node->wins += result;
}

int calc_number_moves(struct genlist* all_moves){
	if(all_moves == NULL){
		return 0;
	} else {
		int no_of_units = genlist_size(all_moves);
		int no_of_moves = 1;
		for(int i = 0; i < no_of_units; i++){
			struct unit_moves *tmp = genlist_get(all_moves,i);
			no_of_moves *= genlist_size(tmp->moves);
		}
		return no_of_moves;
	}

}

struct genlist* init_untried_moves(int total_no_moves){
	struct genlist* available_moves = genlist_new();

	if(BRANCH_LIMITED && (total_no_moves != 0)){
		int limit = 0;

		if(total_no_moves < BRANCH_LIMIT){
			limit = total_no_moves;
		} else {
			limit = BRANCH_LIMIT;
		}

		for(int i=0; i<limit; i++){
			bool seen = FALSE;
			int rand_move_no = rand() % total_no_moves;

			for(int j = 0; j < genlist_size(available_moves); j++){
				if(rand_move_no == (int) genlist_get(available_moves, j)){
					seen = TRUE;
					i--;
					break;
				}
			}
			if(seen == FALSE){
				genlist_append(available_moves, (void*) rand_move_no);
			}
		}
	} else {
		for(int i=0; i<total_no_moves; i++){
			genlist_append(available_moves, (void*)i);
		}
	}

	return available_moves;
}







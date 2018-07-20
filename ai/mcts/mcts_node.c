#include "mcts_node.h"
<<<<<<< HEAD
#include "genlist.h"

mcts_node* create_node(int player, struct genlist *possible_moves, void * move,
		mcts_node *parent) {
=======
#include "aisettler.h"
#include "idex.h"
#include <string.h>

#define BRANCH_LIMITED	1
#define BRANCH_LIMIT	30
>>>>>>> experimental-loading

mcts_node* create_node(int p_index, struct genlist *possible_moves, int move,
		mcts_node *parent) {
	mcts_node* node = (mcts_node*) malloc(sizeof(mcts_node));

	node->uninitialised = TRUE;
	node->player_index = p_index; // Index of the player that just moved
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

<<<<<<< HEAD
mcts_node* add_child_node(mcts_node* parent, fc_game_state* state) {
	int rand_index = fc_rand(genlist_size(parent->untried_moves));
	void* rnd_child_move = genlist_get(parent->untried_moves, rand_index);
	genlist_erase(parent->untried_moves, genlist_link(parent->untried_moves, rand_index));

	//TODO: *** Perform chosen move ***

	struct genlist* untried_child_moves = genlist_new();
	mcts_node *child_node = create_node(parent->player, untried_child_moves,
			rnd_child_move, parent);
=======
mcts_node* add_child_node(mcts_node* parent, int p_index, int move_no) {
	mcts_node *child_node = create_node(p_index, NULL,move_no, parent);
>>>>>>> experimental-loading
	genlist_append(parent->children, child_node);
	return child_node;
}

<<<<<<< HEAD
void destruct_tree(mcts_node *root_node) {
	if (root_node == NULL) {
		return;
=======
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
>>>>>>> experimental-loading
	}

	genlist_destroy(node->all_moves);

<<<<<<< HEAD
	while(genlist_size(root_node->children) != 0){
//		child = genlist_pop_front(root_node->children);
//		destruct_tree(child);
	}

	// free + destroy move lists data
	for(int i = 0; i < genlist_size(root_node->untried_moves); i++){
//		free(genlist_pop_front(root_node->untried_moves));
	}
	genlist_destroy(root_node->untried_moves);

	genlist_destroy(root_node->children);

	if(root_node != NULL){
		free(root_node);
	}
=======
	// Untried moves
	genlist_destroy(node->untried_moves);

	//This node
	free(node);
>>>>>>> experimental-loading

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
	struct genlist* available_moves = NULL;

	if(total_no_moves != 0){
		available_moves = genlist_new();
		for (int i = 0; i < total_no_moves; i++) {
			//TODO: Could improve this - potential issue with overflow
			// (although should never have large enough moves due to pruning)
			genlist_append(available_moves, (void*) i);
		}
	}

	return available_moves;
}







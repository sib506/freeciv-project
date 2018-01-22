#include "mcts_node.h"
#include <string.h>

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
	printf("\t\tUntriedMove: %d\n", node->total_no_moves);
	node->untried_moves = init_untried_moves(node->total_no_moves);

	return node;
}

mcts_node* create_root_node(int p_index, struct genlist *all_possible_moves) {
	return create_node(p_index, all_possible_moves, NULL, NULL);
}

mcts_node* add_child_node(mcts_node* parent, int move_no) {
	// How do we know what the next player will be?
	mcts_node *child_node = create_node(-1, NULL,move_no, parent);

	return child_node;
}

void free_node(mcts_node node) {
	/*if (root_node == NULL) {
		return;
	}

	mcts_node* child;

	while(genlist_size(root_node.children) != 0){
		child = genlist_front(root_node.children);
		genlist_pop_front(root_node.children);
		free_search_tree(child);
	}

	// free + destroy move lists data
	for(int i = 0; i < genlist_size(root_node.untried_moves); i++){
		free(genlist_pop_front(root_node.untried_moves));
	}
	genlist_destroy(root_node.untried_moves);

	genlist_destroy(root_node.children);

	if(root_node != NULL){
		free(root_node);
	}*/

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
	for(int i=0; i<total_no_moves; i++){
		genlist_append(available_moves, (void*)i);
	}

	return available_moves;
}







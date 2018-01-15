#include "mcts_node.h"
#include "genlist.h"

mcts_node* create_node(int player, struct genlist *possible_moves, void * move,
		mcts_node *parent) {

	mcts_node* node = (mcts_node*) malloc(sizeof(mcts_node));
	node->parent = parent;
	node->children = genlist_new();
	node->move = move;
	node->untried_moves = possible_moves;
	node->player = player;

	return node;
}

mcts_node* create_root_node(int player, struct genlist *possible_moves) {
	return create_node(player, possible_moves, NULL, NULL);
}

mcts_node* add_child_node(mcts_node* parent, fc_game_state* state) {
	int rand_index = fc_rand(genlist_size(parent->untried_moves));
	void* rnd_child_move = genlist_get(parent->untried_moves, rand_index);
	genlist_erase(parent->untried_moves, genlist_link(parent->untried_moves, rand_index));

	//TODO: *** Perform chosen move ***

	struct genlist* untried_child_moves = genlist_new();
	mcts_node *child_node = create_node(parent->player, untried_child_moves,
			rnd_child_move, parent);
	genlist_append(parent->children, child_node);

	return child_node;
}

void destruct_tree(mcts_node *root_node) {
	if (root_node == NULL) {
		return;
	}

	mcts_node* child;

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

}

void update_node(int32_t result, mcts_node* node) {
	node->visits++;
	node->wins += result;
}

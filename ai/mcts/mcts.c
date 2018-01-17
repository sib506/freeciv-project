#include "mcts.h"
#include "mcts_node.h"
#include <math.h>

static mcts_node* select(const mcts_node* const root, const double c);
static mcts_node* UCT_select_child(const mcts_node* const root, const double c);
static double UCT(const mcts_node* const node, const int rootPlays, const double c);

void* bestMove(fc_game_state* state, const int duration, const double c) {

	mcts_node *root = create_root_node(NULL, NULL);

	int elapsed_time = 0;

	while( elapsed_time < duration){

		mcts_node *current_node = root;
		// TODO: Take a copy of current state

		// Selection
		// Traverse Nodes that have all their children generated
		while((genlist_size(current_node->untried_moves) == 0) &&
				(genlist_size(current_node->children) != 0)){
			//current_node = UCT_select_child();
			//TODO: Now perform the move

		}

		// Expansion
		// Simulation
		// Backpropagation
	}

	// choose best move out of possible

	// destroy mcts tree

	return NULL;
}


static mcts_node* UCT_select_child(const mcts_node* const root, const double c){
	int t = root->visits;

	mcts_node *best_node;
	mcts_node *tmp;
	double best_weight;
	double tmp_weight;

	int no_children = genlist_size(root->children);

	for(int i = 0; i < no_children; i++){
		tmp = (mcts_node*) genlist_get(root->children, i);
		tmp_weight = UCT(tmp, t, c);
		if (tmp_weight > best_weight){
			best_node = tmp;
			best_weight = tmp_weight;
		}
	}

	return best_node;
}


static double UCT(const mcts_node *node, const int rootPlays, const double c){
	double left = (node->wins / (double) node->visits);
	double right = c * pow(log((double) rootPlays) / (double) node->visits, 0.5);

	return left + right;
}

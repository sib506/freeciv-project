#include "mcts.h"
#include "mcts_node.h"

#define MAXDEPTH 20

static mcts_node* UCT_select_child(const mcts_node* const root, const double c);
static double UCT(const mcts_node* const node, const int rootPlays, const double c);

mcts_node *mcts_root; //Permanent root of the tree
int mcts_mode = 0; // Bool: True/False
int depth = 0;

void mcts_best_move(struct player *pplayer) {

	if(mcts_mode != 1){
		mcts_mode = 1;
		//Need to get the available moves of that player
		mcts_root = create_root_node(pplayer->username,NULL);
		//Create save point
	}

	if(depth < MAXDEPTH){
		//Keep going - select a move to make from the tree
		mcts_node *current_node = mcts_root;

		// Selection
		// Traverse Nodes that have all their children generated
		while((genlist_size(current_node->untried_moves) == 0) &&
				(genlist_size(current_node->children) != 0)){
			current_node = UCT_select_child();
			//TODO: Now perform the move

		}

		// Expansion
		// Simulation
		// Backpropagation


	} else {
		//Choose best move + return or make it
		//Turn mcts mode off
		//Free MCTS Tree
		//Continue game with other players as normal
	}

	return;
}


static mcts_node* UCT_select_child(const mcts_node* const root, const double c){
	uint32_t t = root->visits;

	mcts_node *best_node;
	mcts_node *tmp;
	double best_weight;
	double tmp_weight;

	int no_children = genlist_size(root->children);

	for(int i = 0; i < no_children; i++){
		tmp = genlist_get(root->children, i);
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

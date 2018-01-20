#include "mcts.h"
#include "mcts_node.h"
#include <math.h>
#include "srv_main.h"
#include <stdbool.h>

#define MAXDEPTH 20

static mcts_node* UCT_select_child(const mcts_node* const root, const double c);
static double UCT(const mcts_node* const node, const int rootPlays, const double c);
static void free_mcts_tree();

enum mcts_stage{
	selection, expansion, simulation, backpropagation
};

mcts_node *mcts_root; //Permanent root of the tree
mcts_node *current_mcts_node;

bool mcts_mode = false;
int rollout_depth = 0;

bool move_already_chosen = false;
int chosen_move_set = -1;
enum mcts_stage current_mcts_stage = selection;

char *mcts_save_filename = "mcts-root";

void mcts_best_move(struct player *pplayer) {
	// i.e. this is the first time - we need actually create the tree
	if(!mcts_mode){
		mcts_mode = true;
		save_game(mcts_save_filename, "Root of MCTS tree", FALSE);

		// Collect all available moves for player
		struct genlist *all_unit_moves = player_available_moves(pplayer);
		mcts_root = create_root_node(pplayer->name,all_unit_moves);

		current_mcts_node = mcts_root;
	}

	// Now tree has been created, we need to get an action for player units
	// Via - Selection, Expansion, Simulation

	//Selection - no untried moves, need to navigate to children
	if((genlist_size(current_mcts_node->untried_moves) == 0) &&
			(genlist_size(current_mcts_node->children) != 0)){
		current_mcts_stage = selection;
		current_mcts_node = UCT_select_child(current_mcts_node, 0.9);
		// Need to perform move associated with travelling to this node
	}

	//Expansion - If we can expand then we need to
	if(genlist_size(current_mcts_node->untried_moves) != 0){
		current_mcts_stage = expansion;
		// Make a random choice of the untried moves
		// + store as selected move
		// Add a new node for that move + descend the tree
	}

	//Rollout - Now perform rollouts
	current_mcts_stage = simulation;
	rollout_depth = 0;


	if(rollout_depth < MAXDEPTH){
		//Keep performing random moves until depth is reached
	} else {
		current_mcts_stage = backpropagation;
		// Update all nodes above current node
		// restore game back to root of tree + start process again
		load_command(NULL, mcts_save_filename, FALSE, TRUE);
	}

	// If need to return an actual move now i.e. time-out
	if(0){
		//Choose best move + return or make it
		chosen_move_set = 0;
		//Turn mcts mode off
		mcts_mode = false;
		//Free MCTS Tree
		free_mcts_tree();
		//Continue game with other players as normal
		load_command(NULL, mcts_save_filename, FALSE, TRUE);
	}

	return;
}


static mcts_node* UCT_select_child(const mcts_node* const root, const double c){
	int t = root->visits;

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

static void free_mcts_tree(){
	//TODO: Need to implement

	return;
}

int find_index_of_unit(struct unit *punit, struct genlist *player_moves) {
	int target_id = punit->id;
	for (int i = 0; i < genlist_size(player_moves); i++) {
		struct unit_moves *tmp = genlist_get(player_moves, i);

		fc_assert(tmp != NULL);

		if (tmp->id == target_id) {
			return i;
		}
	}
	return -1;
}

struct potentialMove* return_unit_index_move(int move_no, int unit_list_index,
		struct genlist *player_moves){
	int no_of_moves_higher_in_list = 1;

	for(int i=unit_list_index+1; i<genlist_size(player_moves); i++){
		no_of_moves_higher_in_list *= genlist_size(genlist_get(player_moves,i));
	}

	int move_index = (move_no/no_of_moves_higher_in_list) % genlist_size(genlist_get(player_moves,unit_list_index));

	return genlist_get(genlist_get(player_moves, unit_list_index), move_index);
}

struct potentialMove* return_punit_move(struct unit *punit){
	struct genlist *player_moves = current_mcts_node->parent->all_moves;
	int unit_list_index = find_index_of_unit(punit, player_moves);
	int move_no = current_mcts_node->move_no;

	int no_of_moves_higher_in_list = 1;

	for(int i=unit_list_index+1; i<genlist_size(player_moves); i++){
		no_of_moves_higher_in_list *= genlist_size(genlist_get(player_moves,i));
	}

	int move_index = (move_no/no_of_moves_higher_in_list) % genlist_size(genlist_get(player_moves,unit_list_index));

	return genlist_get(genlist_get(player_moves, unit_list_index), move_index);
}

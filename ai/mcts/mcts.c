#include "mcts.h"
#include "mcts_node.h"
#include <math.h>
#include "srv_main.h"
#include <stdbool.h>
#include "stdinhand.h"
#include "rand.h"

#define MAXDEPTH 20
#define MAX_ITER_DEPTH 100
#define UCT_CONST 1.41421356237 //sqrt(2)

static mcts_node* UCT_select_child(mcts_node* root);
static double UCT(mcts_node* child_node, int rootPlays);
static void free_mcts_tree(mcts_node* node);
static int mcts_choose_final_move();

mcts_node *mcts_root = NULL; //Permanent root of the tree
mcts_node *current_mcts_node = NULL;

bool mcts_mode = FALSE;
int rollout_depth = 0;
int iterations = 0;
bool reset = FALSE;

bool move_chosen = FALSE;
int chosen_move_set = -1;
enum mcts_stage current_mcts_stage = selection;

char *mcts_save_filename = "mcts-root";

void mcts_best_move(struct player *pplayer) {
	// i.e. this is the first time - we need actually create the tree
	if(!mcts_mode){
		printf("MCTS MODE ON\n");
		mcts_mode = true;
		save_game(mcts_save_filename, "Root of MCTS tree", FALSE);

		// Free the previous tree
		if(mcts_root != NULL){
			free_mcts_tree(mcts_root);
		}

		// Collect all available moves for player
		struct genlist *all_unit_moves = player_available_moves(pplayer);
		mcts_root = create_root_node(player_index(pplayer),all_unit_moves);
		mcts_root->uninitialised = FALSE;
		current_mcts_node = mcts_root;
	}

	if((current_mcts_stage == simulation)){
		printf("CONTINUE SIMULATION\n");
		if(rollout_depth >= MAXDEPTH)
			backpropagate(TRUE);
	} else {
		//Selection - no untried moves, need to navigate to children
		if(((genlist_size(current_mcts_node->untried_moves) == 0) &&
				(genlist_size(current_mcts_node->children) != 0)) || current_mcts_node->uninitialised){
			current_mcts_stage = selection;
			printf("SELECTION\n");

			// If need to return an actual move now i.e. time-out
			if(iterations >= MAX_ITER_DEPTH){
				//Choose best move i.e. most visited
				chosen_move_set = mcts_choose_final_move();
				move_chosen = TRUE;
				//Turn mcts mode off
				mcts_mode = FALSE;
				printf("Final move has been chosen. Now returning to game.\n");
				//Continue game with other players as normal
				load_command(NULL, mcts_save_filename, FALSE, TRUE);
			}

			if(current_mcts_node == mcts_root){
				iterations++;
			}

			if(current_mcts_node->uninitialised){
				struct genlist *all_unit_moves = player_available_moves(pplayer);
				current_mcts_node->player_index = player_index(pplayer);
				current_mcts_node->all_moves = all_unit_moves;
				current_mcts_node->total_no_moves = calc_number_moves(all_unit_moves);
				current_mcts_node->untried_moves = init_untried_moves(current_mcts_node->total_no_moves);
				current_mcts_node->uninitialised = FALSE;
			} else {
				current_mcts_node = UCT_select_child(current_mcts_node);
				return;
			}
		}

		printf("\t\t%d\n", genlist_size(current_mcts_node->untried_moves));
		//Expansion - If we have untried moves then need to expand
		if(genlist_size(current_mcts_node->untried_moves) != 0){
			printf("EXPANSION\n");
			current_mcts_stage = expansion;

			// Lookup size of untried moves list
			int untried_size = genlist_size(current_mcts_node->untried_moves);
			printf("\tuntried_size: %d", untried_size);

			// Retrieve move number + remove from untried list
			int random_index = fc_rand(untried_size);
			int move_no = (int) genlist_get(current_mcts_node->untried_moves,
					random_index);
			genlist_remove(current_mcts_node->untried_moves,
					(void *)move_no);

			printf("\tRandNo: %d\n", random_index);

			// Create a new node for that move + set as current node
			current_mcts_node = add_child_node(current_mcts_node, move_no);
		}

		//Rollout - Now perform rollouts
		if(current_mcts_stage == expansion){
			printf("SIMULATION\n");
			current_mcts_stage = simulation;
			rollout_depth = 0;
		}

	}

	return;
}


static mcts_node* UCT_select_child(mcts_node* root){
	int self_visits = root->visits;

	mcts_node *best_node = NULL;
	mcts_node *tmp = NULL;
	double best_weight = 0;
	double tmp_weight = 0;

	int no_children = genlist_size(root->children);

	for(int i = 0; i < no_children; i++){
		tmp = genlist_get(root->children, i);
		tmp_weight = UCT(tmp, self_visits);
		if (tmp_weight > best_weight){
			best_node = tmp;
			best_weight = tmp_weight;
		}
	}

	return best_node;
}


static double UCT(mcts_node *child_node, int rootPlays){
	double left = (child_node->wins / (double) child_node->visits);
	double right = UCT_CONST * pow(log((double) rootPlays) / (double) child_node->visits, 0.5);

	return left + right;
}

static void free_mcts_tree(mcts_node *node){
	int no_children = genlist_size(node->children);

	// Free all child nodes recursively
	for(int i = 0; i < no_children; i++){
		mcts_node *child = genlist_get(node->children, i);
		free_mcts_tree(child);
	}

	//Free yourself now
	free_node(node);

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
	struct genlist *player_moves;
	int unit_list_index;
	int move_no;

	if(move_chosen){
		player_moves = mcts_root->all_moves;
		unit_list_index = find_index_of_unit(punit, player_moves);
		move_no = chosen_move_set;
	} else {
		player_moves = current_mcts_node->parent->all_moves;
		unit_list_index = find_index_of_unit(punit, player_moves);
		move_no = current_mcts_node->move_no;
	}

	int no_of_moves_higher_in_list = 1;

	for(int i=unit_list_index+1; i<genlist_size(player_moves); i++){
		no_of_moves_higher_in_list *= genlist_size(genlist_get(player_moves,i));
	}

	int move_index = (move_no/no_of_moves_higher_in_list) % genlist_size(genlist_get(player_moves,unit_list_index));

	return genlist_get(genlist_get(player_moves, unit_list_index), move_index);
}

void backpropagate(bool interrupt){
	if(current_mcts_stage != simulation){
		return;
	}
	current_mcts_stage = backpropagation;
	printf("BACKPROP\n");

	mcts_node *node = current_mcts_node;
	enum victory_state plr_state[player_slot_count()];
	rank_mcts_users(interrupt, plr_state);

	if(plr_state[node->player_index] == VS_WINNER){
		update_node(1, node);
	} else if(plr_state[node->player_index] == VS_LOSER){
		update_node(-1, node);
	} else {
		update_node(0, node);
	}

	while(node->parent != NULL){
		node = node->parent;
		if(plr_state[node->player_index] == VS_WINNER){
			update_node(1, node);
		} else if(plr_state[node->player_index] == VS_LOSER){
			update_node(-1, node);
		} else {
			update_node(0, node);
		}
	}
	current_mcts_stage = selection;
	current_mcts_node = mcts_root;
	load_command(NULL, mcts_save_filename, FALSE, TRUE);
	return;
}

static int mcts_choose_final_move(){
	int most_visits = 0;
	int chosen_move;

	for(int i = 0; i < genlist_size(mcts_root->children); i++){
		mcts_node *child_node = genlist_get(mcts_root->children, i);
		if (child_node->visits > most_visits){
			most_visits = child_node->visits;
			chosen_move = child_node->move_no;
		}
	}

	return chosen_move;
}

bool at_root_of_tree(){
	return current_mcts_node == mcts_root;
}

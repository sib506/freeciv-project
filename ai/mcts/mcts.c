#include "mcts.h"
#include "mcts_node.h"
#include <math.h>
#include "srv_main.h"
#include <stdbool.h>
#include "stdinhand.h"
#include "rand.h"
#include "featured_text.h"
#include "notify.h"
#include "idex.c"
#include "mcts_config.h"

static mcts_node* UCT_select_child(mcts_node* root);
static double UCT(mcts_node* child_node, int rootPlays);
static void free_mcts_tree(mcts_node* node);
static int mcts_choose_final_move();
void print_mcts_tree_layer1();

mcts_node *mcts_root = NULL; //Permanent root of the tree
mcts_node *current_mcts_node = NULL;

bool game_over = TRUE;
bool mcts_mode = FALSE;
int rollout_depth = 0;
int iterations = 0;
bool reset = FALSE;
bool pending_game_move = FALSE;

bool move_chosen = FALSE;
int chosen_move_set = -1;
enum mcts_stage current_mcts_stage = selection;

char *mcts_save_filename = "mcts-root";

void mcts_best_move(struct player *pplayer) {
	// i.e. this is the first time - we need actually create the tree
	if(!mcts_mode & !pending_game_move){
		printf("MCTS MODE ON\n");

		mcts_mode = TRUE;
		game_over = FALSE;

		//Reset some variables
		move_chosen = FALSE;
		iterations = 0;
		chosen_move_set = -1;

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

	if(move_chosen){
			printf("Final moves are being attached\n");
			attach_chosen_move(pplayer);
			pending_game_move = FALSE;
			return;
		}

	if(current_mcts_node->uninitialised){
		struct genlist *all_unit_moves = player_available_moves(pplayer);
		current_mcts_node->all_moves = all_unit_moves;
		current_mcts_node->total_no_moves = calc_number_moves(all_unit_moves);
		current_mcts_node->untried_moves = init_untried_moves(current_mcts_node->total_no_moves);
		current_mcts_node->uninitialised = FALSE;
	}

	//Rollout - Now perform rollouts
	if(current_mcts_stage == expansion){
		printf("SIMULATION\n");
		current_mcts_stage = simulation;
		rollout_depth = 0;
	}

	if(current_mcts_node == mcts_root){
		printf("Game Turn: %d\n", game.info.turn);
		print_mcts_tree_layer1();
		iterations++;

		// If need to return an actual move now i.e. time-out
		if(iterations >= MAX_ITER_DEPTH){
			//Choose best move i.e. most visited
			move_chosen = TRUE;
			chosen_move_set = mcts_choose_final_move();
			//Turn mcts mode off
			mcts_mode = FALSE;
			pending_game_move = TRUE;
			current_mcts_node = NULL; //So settler doesn't detect the root note
			printf("Final move has been chosen. Now returning to game.\n");
			//Continue game with other players as normal
			reset = TRUE;
			printf("Reset\n");
			return;
			//mcts_end_command();
			//load_command(NULL, mcts_save_filename, FALSE, TRUE);
		}
		printf("Current MCTS Iterations: %d\n", iterations);
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

			current_mcts_node = UCT_select_child(current_mcts_node);
			attach_chosen_move(pplayer);
			return;
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
			int random_index = rand() % untried_size;
			int move_no = (int) genlist_get(current_mcts_node->untried_moves,
					random_index);
			genlist_remove(current_mcts_node->untried_moves,
					(void *)move_no);

			printf("\tRandNo: %d\n", random_index);

			// Create a new node for that move + set as current node
			current_mcts_node = add_child_node(current_mcts_node, player_index(pplayer),move_no);

			attach_chosen_move(pplayer);
		}

	}

	return;
}


static mcts_node* UCT_select_child(mcts_node* root){
	int self_visits = root->visits;

	int no_children = genlist_size(root->children);

	mcts_node *best_node = genlist_get(root->children, 0);
	double best_weight = UCT(best_node, self_visits);

	for(int i = 1; i < no_children; i++){
		mcts_node *tmp = genlist_get(root->children, i);
		double tmp_weight = UCT(tmp, self_visits);
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

	printf("No ID found\n");

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

	if(move_chosen){ //Should this also check that it is the MCTS player?
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
		struct unit_moves * tmp_unit = genlist_get(player_moves,i);
		no_of_moves_higher_in_list *= genlist_size(tmp_unit->moves);
		printf("Unit moves:%d -- %d\n", genlist_size(tmp_unit->moves), no_of_moves_higher_in_list);
	}

	struct unit_moves * unit = genlist_get(player_moves, unit_list_index);
	int no_unit_moves = genlist_size(unit->moves);

	int move_index = (move_no/no_of_moves_higher_in_list) % no_unit_moves;

	printf("unit index: %d\n", unit_list_index);
	printf("\tmove_number: %d\n", move_no);
	printf("\thigher_moves: %d\n", no_of_moves_higher_in_list);
	printf("\tmove_index: %d\n", move_index);
	printf("\tmodulo: %d\n", no_unit_moves);
	printf("\tMove mem: %d\n", (int) genlist_get(unit->moves, move_index));

	return genlist_get(unit->moves, move_index);
}

void backpropagate(bool interrupt){
	if ((current_mcts_stage == simulation)
			|| (game.info.turn >= game.server.end_turn)) {

		current_mcts_stage = backpropagation;
		printf("BACKPROP\n");

		mcts_node *node = current_mcts_node;
		enum victory_state plr_state[player_slot_count()];
		rank_mcts_users(interrupt, plr_state);

		if (plr_state[node->player_index] == VS_WINNER) {
			update_node(1, node);
		} else if (plr_state[node->player_index] == VS_LOSER) {
			update_node(0, node);
		} else {
			update_node(0, node);
		}

		while (node->parent != NULL) {
			node = node->parent;
			if (plr_state[node->player_index] == VS_WINNER) {
				update_node(1, node);
			} else if (plr_state[node->player_index] == VS_LOSER) {
				update_node(0, node);
			} else {
				update_node(0, node);
			}
		}
		current_mcts_stage = selection;
		current_mcts_node = mcts_root;
		reset = TRUE;
		printf("Reset\n");
	}
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

void print_mcts_tree_layer1(){
	printf("-------------------------\n");
	printf("# Visits: %d \t Score: %d\n", mcts_root->visits, mcts_root->wins);
	for(int i=0; i < genlist_size(mcts_root->children); i++){
		mcts_node *child_node = genlist_get(mcts_root->children, i);
		printf("\t# Visits: %d \t Score: %d\n", child_node->visits, child_node->wins);
	}
	printf("-------------------------\n");
}



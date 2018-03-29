#include "mcts.h"
#include "mcts_node.h"
#include "mcts_config.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "srv_main.h"
#include "stdinhand.h"
#include "rand.h"
#include "featured_text.h"
#include "notify.h"
#include "idex.c"
#include "time.h"

void mcts_init(struct player *pplayer);
void mcts_selection(struct player *pplayer);
void mcts_expansion(struct player *pplayer);
void mcts_simulation();

static mcts_node* UCT_select_child(mcts_node* root);
static double UCT(mcts_node* child_node, int rootPlays);
static void free_mcts_tree(mcts_node* node);
static int mcts_choose_final_move();
static double current_free_memory();

void print_mcts_tree_layer1();

mcts_node *mcts_root = NULL; 			// Root of the MCTS tree
mcts_node *current_mcts_node = NULL;	// Current MCTS node being considered

bool game_over = TRUE;	// Is the game over i.e. are we waiting for it to be reset
bool mcts_mode = FALSE;	// Are we currently running MCTS
int rollout_depth = 0;	// Current depth of the rollouts
int iterations = 0;		// Current MCTS iteration
bool reset = FALSE;		// Is the game being reset?
bool pending_game_move = FALSE;	//Are we waiting for the actual game move to be performed

bool move_chosen = FALSE;	// Have we chosen the move we are going to perform?
int chosen_move_set = -1;
enum mcts_stage current_mcts_stage = selection;

char *mcts_save_filename = "mcts-root";

int memory_reset = 0;

FILE *time_fp;

unsigned long long millisecondsSinceEpoch = 0;

/*
 * Returns the memory currently available to the system
 */
static double current_free_memory(){
    FILE *fptr = fopen( "/proc/meminfo", "r" );
	int total_memory = 0;
	int free_memory = 0;
	int available_memory = 0;

    if (fptr == NULL)
    {
        printf("Cannot open file \n");
        exit(0);
    }

    int scan_return = fscanf(fptr, "MemTotal:%d kB\nMemFree:%d kB\nMemAvailable:%d kB",
			&total_memory, &free_memory, &available_memory);
	fclose(fptr);

	if (scan_return < 1){
		return 100;
	} else {
		return ((float) available_memory/ (float) total_memory)*100;
	}

}


/*
 * Initalise the MCTS tree
 */
void mcts_init(struct player *pplayer) {


	printf("MCTS MODE ON\n");

	mcts_mode = TRUE;
	game_over = FALSE;

	//Reset some variables
	move_chosen = FALSE;
	iterations = 0;
	chosen_move_set = -1;
	log_time_to_file("Start MCTS_Save");
	save_game("mcts-root", "Root of MCTS tree", FALSE);
	log_time_to_file("End MCTS_Save");

	// Check Freeciv isn't using too much memory
	if(current_free_memory() <= MEM_FREE_THRESHOLD){
		// Reset Freeciv if memory usage too large
		printf("%f\n", current_free_memory());
		printf("restarting freeciv server due to lack of memory\n");
		server_clear();

		// Must rename score log file otherwise stops logging on restart
		memory_reset++;
		char new_filename[80];
		sprintf(new_filename, "freeciv-score%d.log", memory_reset);
		int ret = rename(GAME_DEFAULT_SCOREFILE, new_filename);

		if (ret == 0) {
			printf("File renamed successfully");
		} else {
			printf("Error: unable to rename the file");
		}

		execl("./fcser","fcser", "--read", "reload_mcts.serv", NULL);
	}
	log_time_to_file("Start MCTS_Iteration");
	// Free the previous tree
	if (mcts_root != NULL) {
		free_mcts_tree(mcts_root);
	}

	// Collect all available moves for player
	struct genlist *all_unit_moves = player_available_moves(pplayer);
	mcts_root = create_root_node(player_index(pplayer), all_unit_moves);
	mcts_root->uninitialised = FALSE;
	current_mcts_node = mcts_root;
}

void mcts_selection(struct player *pplayer){
	log_time_to_file("Start selection");
	current_mcts_stage = selection;

	current_mcts_node = UCT_select_child(current_mcts_node);
	attach_chosen_move(pplayer);
	log_time_to_file("End selection");
}

void mcts_expansion(struct player *pplayer) {
	log_time_to_file("Start expansion");
	current_mcts_stage = expansion;

	// Lookup size of untried moves list
	int untried_size = genlist_size(current_mcts_node->untried_moves);
	printf("\tuntried_size: %d", untried_size);

	// Retrieve move number + remove from untried list
	int random_index = rand() % untried_size;
	int move_no = (int) genlist_get(current_mcts_node->untried_moves,
			random_index);
	genlist_remove(current_mcts_node->untried_moves, (void *) move_no);

	printf("\tRandNo: %d\n", random_index);

	// Create a new node for that move + set as current node
	current_mcts_node = add_child_node(current_mcts_node, player_index(pplayer),
			move_no);

	attach_chosen_move(pplayer);
	log_time_to_file("End expansion");
}

void mcts_simulation(){
	log_time_to_file("Start simulation");
	current_mcts_stage = simulation;
	rollout_depth = 0;
}

void mcts_best_move(struct player *pplayer) {
	// If this is the first time - we need actually create the tree
	if(!mcts_mode & !pending_game_move){
		mcts_init(pplayer);
	}

	// Attach final moves to the player's units
	if (move_chosen) {
		printf("Final moves are being attached\n");
		attach_chosen_move(pplayer);
		pending_game_move = FALSE;
		log_time_to_file("End MCTS_Iteration");
		return;
	}

	// If the current node is uninitalised - need to populate its move information
	if(current_mcts_node->uninitialised){
		log_time_to_file("Start init node");
		log_time_to_file("Start get player moves");
		struct genlist *all_unit_moves = player_available_moves(pplayer);
		log_time_to_file("End get player moves");
		current_mcts_node->all_moves = all_unit_moves;
		log_time_to_file("Start calc move combinatons");
		current_mcts_node->total_no_moves = calc_number_moves(all_unit_moves);
		current_mcts_node->untried_moves = init_untried_moves(current_mcts_node->total_no_moves);
		log_time_to_file("End calc move combinatons");
		current_mcts_node->uninitialised = FALSE;
		log_time_to_file("End init node");
	}

	// If we have performed our expansion stage already, move onto random rollouts
	if(current_mcts_stage == expansion){
		printf("SIMULATION\n");
		mcts_simulation();
	}

	// If we have returned to the root, must have completed an iteration
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

	// If we are simulating then must continue
	if((current_mcts_stage == simulation)){
		printf("CONTINUE SIMULATION\n");
		if(rollout_depth >= MAXDEPTH){
			log_time_to_file("End simulation");
			backpropagate(TRUE);
		}
	} else {
		//Selection - no untried moves, need to navigate to children
		if(((genlist_size(current_mcts_node->untried_moves) == 0) &&
				(genlist_size(current_mcts_node->children) != 0)) || current_mcts_node->uninitialised){
			printf("SELECTION\n");
			return mcts_selection(pplayer);
		}

		printf("\t\t%d\n", genlist_size(current_mcts_node->untried_moves));
		//Expansion - If we have untried moves then need to expand
		if(genlist_size(current_mcts_node->untried_moves) != 0){
			printf("EXPANSION\n");
			mcts_expansion(pplayer);
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
	log_time_to_file("Start return_move");
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
	log_time_to_file("End return_move");
	return genlist_get(unit->moves, move_index);
}

void backpropagate(bool interrupt){
	if ((current_mcts_stage == simulation)
			|| (game.info.turn >= game.server.end_turn)) {
		log_time_to_file("Start backprop");
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
		log_time_to_file("End backprop");
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

void log_time_to_file(char* text){
    struct timeval tv;
    gettimeofday(&tv, NULL);
	time_fp = fopen("./time_log.txt", "a");

    unsigned long long time =
        ((unsigned long long)(tv.tv_sec) * 1000 +
        (unsigned long long)(tv.tv_usec) / 1000);

	fprintf(time_fp, "%s:%llu\n", text, time);

	fclose(time_fp);
}


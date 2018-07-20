#ifndef FC__MCTS_NODE_H__
#define FC__MCTS_NODE_H__

#ifdef HAVE_CONFIG_H
#include <fc_config.h>
#endif

/* utility */
#include "fcintl.h"
#include "genlist.h"
<<<<<<< HEAD
#include "mcts.h"

typedef struct mcts_node {
	int32_t wins;
	int32_t visits;
	int32_t player;
=======
#include "log.h"
#include "mem.h"
#include "support.h"

#include "fc_types.h"
#include "stdbool.h"
#include "aiunit.h"

typedef struct mcts_node {
	bool uninitialised;
	int player_index;
	int move_no;

	int wins;
	int visits;

>>>>>>> experimental-loading
	struct mcts_node *parent;
	struct genlist *children;

	struct genlist *all_moves;
	int total_no_moves;
	struct genlist *untried_moves;
} mcts_node;


/**
 * Creates a new mcts node.
 *
 * @param p_index the id of the last player to have moved
 * @param all_possible_moves the list of moves the node can perform
 * @param move the move that caused the state of the node
 * @param parent the node that created this node
 * @return a pointer to a new mcts node
 */
<<<<<<< HEAD
mcts_node* create_node(int player, struct genlist *possible_moves,
		void * move, mcts_node *parent);
=======
mcts_node* create_node(int p_index, struct genlist *all_possible_moves,
		int move, mcts_node *parent);
>>>>>>> experimental-loading

/**
 * Creates a new root node. parent and move set to NULL.
 *
 * @param p_index the current player
 * @param all_possible_moves the list of moves the node can perform
 * @return a pointer to a new root mcts_node
 */
mcts_node* create_root_node(int p_index, struct genlist *all_possible_moves);


/**
 * Creates a new MCTS node from a move and adds it to the parent.
 *
 * @param parent the parent node the child is to be added to
 * @param p_index the id of the last player to have moved
 * @param move the move that caused the state of the node
 * @return the child mcts_node that was created
 */
mcts_node* add_child_node(mcts_node* parent, int p_index, int move_no);


/**
 * Recursively destruct the mcts nodes from the root node
 *
 * @param root the root node to destruct it's subtree
 */
<<<<<<< HEAD
void destruct_tree(mcts_node *root_node);
=======
void free_node(mcts_node *node);
>>>>>>> experimental-loading

/**
 * Recursively backpropagates up the MCTS tree. Adds the result and
 * increases a node's play count by 1.  Stops when a NULL value for parent
 * is reached.
 *
 * @param result the value to be added to the win value of the node.
 * @param node the node that the backpropagate starts from
 */
void update_node(int32_t result, mcts_node* node);

/**
 * Returns the total number of moves a node has based on
 * the moves for all the different units
 *
 * @param all_moves the list of moves that can be performed for each unit
 * @return total number of moves that can be performed
 */
int calc_number_moves(struct genlist* all_moves);

/**
 * Creates a list of the untried moves
 *
 * @param the number of possible moves for that node
 * @return a list of all possible moves
 */
struct genlist* init_untried_moves(int total_no_moves);

#endif

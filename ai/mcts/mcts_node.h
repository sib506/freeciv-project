#ifndef FC__MCTS_NODE_H__
#define FC__MCTS_NODE_H__

#ifdef HAVE_CONFIG_H
#include <fc_config.h>
#endif

/* utility */
#include "fcintl.h"
#include "genlist.h"
#include "log.h"
#include "mem.h"
#include "support.h"

#include "fc_types.h"
#include "stdbool.h"
#include "aiunit.h"

typedef struct mcts_node {
	int wins;
	int visits;
	char player_username[MAX_LEN_NAME];
	struct mcts_node *parent;
	struct genlist *children;

	struct genlist *all_moves;
	int total_moves;
	struct genlist *untried_moves;
	//bool *untried_moves[];
	void * move; //The move that caused this node

} mcts_node;


/**
 * Creates a new mcts node.
 *
 * @param newLplayer the last player to have moved
 * @param movesList the list of moves the node has left to perform
 * @param newMove the move that caused the state of the node
 * @param newParent the node that created this node
 * @return a pointer to a new MctsNode_s
 * @pre newLplayer is 0 or 1
 */
mcts_node* create_node(char *username, struct genlist *all_possible_moves,
		void * move, mcts_node *parent);

/**
 * Creates a new root node. parent and move set to NULL.
 *
 * @param newLplayer the last player to have moved
 * @param movesList the remaining moves the node can perform
 * @return a pointer to a new MctsNode_s root
 * @pre newLplayer is 0 or 1
 */
mcts_node* create_root_node(char *username, struct genlist *all_possible_moves);


/**
 * Creates a new MCTS node from a remaining move and adds it to the parent.
 *
 * @param parent the parent node in which to add a new child
 * @param state used to perform the move on, and generate a new
 *        moves list from
 * @param childMovesGen a callback function pointer to generate and populate
 *        the moves list for the child.
 * @return the child that was created and added to the parent's child list
 * @pre there are remaining moves in the parent node
 */
mcts_node* add_child_node(mcts_node* parent);


/**
 * Recursively destruct the mcts nodes from the root node
 *
 * @param root the root node to destruct it's subtree
 */
void free_search_tree(mcts_node root_node);

/**
 * Recursively backpropagates up the tree adding the win value and always
 * increasing a node's play count by 1.  Halts when a NULL value for parent
 * is reached.
 *
 * @param gameResult is the value to be added to the win value of the node.
 * @param node the node that was simulated from and to backpropagate from
 */
void update_node(int32_t result, mcts_node* node);

int calc_number_moves(struct genlist* all_moves);

#endif

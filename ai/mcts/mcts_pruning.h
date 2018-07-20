#ifndef FC__MCTS_PRUNING_H__
#define FC__MCTS_PRUNING_H__

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

/**
 * Returns a new list of pruned moves for a unit. Basic pruning by randomly
 * selecting moves for the unit to keep.
 *
 * @param all__moves the list of moves the node can perform
 * @param need_to_free whether on return the origional move list needs to be freed
 * @return a pointer to the new list of pruned moves
 */
struct genlist* random_mcts_general_pruning(struct genlist *all_moves, bool *need_to_free);

/**
 * Returns a new list of pruned moves for a unit. Basic pruning by randomly
 * selecting moves for the unit to keep. Settlers who can build a city
 * must have this in their pruned moves list
 *
 * @param all__moves the list of moves the node can perform
 * @param need_to_free whether on return the origional move list needs to be freed
 * @param punit the unit on which we are pruning moves for
 * @return a pointer to the new list of pruned moves
 */
struct genlist* random_mcts_settler_pruning(struct genlist* all_moves, bool * need_to_free,
		struct unit * punit);

#endif

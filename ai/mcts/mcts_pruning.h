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

struct genlist* random_mcts_general_pruning(struct genlist *all_moves, bool *need_to_free);
struct genlist* random_mcts_settler_pruning(struct genlist* all_moves, bool * need_to_free,
		struct unit * punit);

#endif

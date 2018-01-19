#ifndef FC__MCTS_H__
#define FC__MCTS_H__

#ifdef HAVE_CONFIG_H
#include <fc_config.h>
#endif

/* common */
#include "player.h"

void mcts_best_move(struct player *pplayer);

#endif

#ifndef FC__MCTS_H__
#define FC__MCTS_H__

#include "mcts_state.h"
#include <stdio.h>

/**
 * Using the 4 cycles of the Monte Carlo tree search:
 * 1) Selection
 * 2) Expansion
 * 3) Simulation
 * 4) Backpropagation
 * bestMove returns the most visited move.
 *
 * @param currState the current state to calculate the best move to make
 * @param duration the number of milliseconds to spend on the calculation
 * @param c the exploration parameter in the UCT
 * @return a pointer to the calculated best move
 */
void* bestMove(fc_game_state* state, int duration, double c);

#endif

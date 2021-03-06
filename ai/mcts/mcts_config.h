
#define MEM_FREE_THRESHOLD 15.0

// MCTS
#define MAXDEPTH 20				// Maximum rollout depth
#define MAX_ITER_DEPTH 600		// Number of MCTS iterations before making a move
#define UCT_CONST 1.41421356237 // UCT constant - currently: sqrt(2)


// Pruning
#define PRU_LEVEL random_pruning
#define MAX_NO_UNIT_MOVES 3 	// The maximum number of moves each unit can have

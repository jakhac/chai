#pragma once

//#include <chrono>
#include <iomanip> // setprecision

#include "eval.h"
#include "moveOrdering.h"
#include "tt.h"

#define DO_NULL true
#define NO_NULL false

#define IS_PV true
#define NO_PV false

#define R_2 2
#define R_3 3

#define F1_MARGIN 125
#define F2_MARGIN 550
#define F3_MARGIN 800

#define RAZOR_DEPTH 3

/**
 * Maximum ply reached in alphaBeta and quiescence search.
 */
extern int selDepth;

/**
 * Maximum score before rescale in history heuristic. Max history score needs to be less than 10000
 * because move ordering scores "QUIET_SCORES=5000 + hist/10" before COUNTER_SCORE=6000.
 */
const int HISTORY_MAX = 1000 - 1;

/**
  Principal variation line used for root alphaBeta call. Contains main line after search finished.
*/
//static pv_line_t pvLine[1];

// Search stack used for all searches
static searchStack_t sStack[MAX_GAME_MOVES];

static const int moveCountPruningDepth = 5;

static int moveCountPruning[moveCountPruningDepth];

/**
 * Remember checks given in quiescence to detect checkmates. Quiescence depths are stored
 * as absolute values.
 */
static int quiescenceChecks[MAX_DEPTH];

//int alphaBetaRoot(board_t* b, search_t* s, int depth, move_t* move);

typedef enum nodeType_t {
	PV,
	NoPV,
	AllNode
} nodeType_t;

/**
 * Alpha beta algorithm root. Searches current board for best move and score.
 *
 * @param  alpha  Lower bound, minimum score the side is assured of.
 * @param  beta   Upper bound, maximum score the opponent is assured of.
 * @param  depth  Current depth.
 * @param  b	  Current board.
 * @param  s	  Search info containing search parameters.
 * @param  nullOk Enables null move pruning in node.
 * @param  pvNode Determines the current node type.
 * @param  pvLine The pv line.
 *
 * @returns Best score found in search.
 */
template <nodeType_t nodeType>
value_t alphaBeta(value_t alpha, value_t beta, int depth, board_t* b, search_t* s);

/**
 * Quiescence search pushes all captures to evaluate a stable and quiet position. AlphaBeta
 * always drops into quiescence with depth 0.
 *
 * @param  alpha Lower bound, minimum score the side is assured of.
 * @param  beta  Upper bound, maximum score the opponent is assured of.
 * @param  b	 Current board.
 * @param  s	 Search info containing search parameters.
 *
 * @returns Best score found in quiescences search.
 */
template <nodeType_t nodeType>
value_t quiescence(value_t alpha, value_t beta, int depth, board_t* b, search_t* s);

/**
 * Root function that starts alphaBeta search in iterative deepening framework.
 *
 * @param  b Reference to current board.
 * @param  s Search info containing search parameters.
 *
 * @returns value_t Best value found search.
 */
value_t search(board_t* b, search_t* s);

/**
 * Apply offset of aspiration windows in alphaBeta call and try to score inside alpha and beta.
 * If score is outside of bounds, do a research with infinite bounds.
 *
 * @param  b		 Current board.
 * @param  s		 Search info containing search parameters.
 * @param  depth	 Current depth for alphaBeta call.
 * @param  bestScore Best score from previous search to determine aspiration windows.
 *
 * @returns Best score found in alphaBeta search.
 */
int search_aspiration(board_t* b, search_t* s, int depth, int bestScore);

/**
 * Tries to find a single(!) repetition in undoPly array. Used in search to find upcoming draws.
 *
 * @param  b Current board.
 *
 * @returns True if repetition is found in undoPly array, else false.
 */
bool isRepetition(board_t* b);

/**
 * Counts repetitions of current board (excluding current position).
 * Return value greater equal 2 means 3-fold-repetition.
 *
 * @param  b Current board.
 *
 * @returns True if three fold repetition is found, else false.
 */
int getRepetitions(board_t* b);

/**
 * Checks if current position might be a zugzwang. Considers endgame
 *
 * @param  b	  Current board.
 *
 * @returns True if current position is probably azugzwang, else false.
 */
bool zugzwang(board_t* b);

/**
 * Swaps the best move found in moveList with move at current index.
 *
 * @param  b	  Current board.
 * @param  move_s Move struct with all moves and scores.
 * @param  curIdx Current index in moveList.
 */
void getNextMove(moveList_t* move_s, int curIdx);

/**
 * Swap to moves and their score in given MOVE_S struct. Returns if id1 is equal to id2.
 *
 * @param  move_s Move struct with all moves and scores.
 * @param  id1    First index.
 * @param  id2    Second index.
 */
void swapMove(moveList_t* move_s, int id1, int id2);

/**
 * Print search information to console.
 *
 * @param  b Current board.
 * @param  s This search info printed to console.
 */
void printSearchInfo(board_t* b, search_t* s);

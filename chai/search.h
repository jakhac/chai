#pragma once

//#include <chrono>
#include <iomanip> // setprecision

#include "eval.h"
#include "moveOrdering.h"
#include "tt.h"
#include "thread.h"
#include "syzygy.h"

const value_t aspiration = 18;

const unsigned long long aspirationWindows[] = {
	100, 2500, 32000, 64000
};

/**
 * Initialize some search parameters like MCP values.
 */
void initSearch();

/**
 * Alpha beta algorithm root. Searches current board for best move and score.
 *
 * @param  alpha  Lower bound, minimum score the side is assured of.
 * @param  beta   Upper bound, maximum score the opponent is assured of.
 * @param  depth  Current depth.
 * @param  b	  Current board.
 * @param  s	  Search info containing search parameters.
 *
 * @returns Best score found in search.
 */
template <chai::nodeType_t nodeType>
value_t alphaBeta(Thread thread, value_t alpha, value_t beta, int depth);

/**
 * Quiescence search pushes all captures to evaluate a stable and quiet position. AlphaBeta
 * always drops into quiescence with depth 0.
 *
 * @param  alpha Lower bound, minimum score the side is assured of.
 * @param  beta  Upper bound, maximum score the opponent is assured of.
 * @param  depth  Current depth. Initial calls have depth=0
 * @param  b	 Current board.
 * @param  s	 Search info containing search parameters.
 *
 * @returns Best score found in quiescences search.
 */
template <chai::nodeType_t nodeType>
value_t quiescence(Thread thread, value_t alpha, value_t beta, int depth);

/**
 * Root function that starts alphaBeta search in iterative deepening framework.
 *
 * @param  b Reference to current board.
 * @param  s Search info containing search parameters.
 *
 * @returns value_t Best value found search.
 */
value_t search(board_t* b, search_t* s);

// TODO doc
// void iid(ThreadWrapper* thread);

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
value_t aspirationSearch(Thread thread, int depth, value_t bestScore);

/**
 * Tries to find a single(!) repetition in undoPly array. Used in search to find upcoming draws.
 *
 * @param  b Current board.
 *
 * @returns True if repetition is found in undoPly array, else false.
 */
bool isRepetition(board_t* b);

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
 * @param  s This search info printed to console.
 */
void printSearchInfo(search_t* s);
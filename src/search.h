#pragma once

#include <iomanip> // setprecision

#include "eval.h"
#include "moveOrdering.h"
#include "tt.h"
#include "thread.h"
#include "syzygy.h"
#include "timeMan.h"


enum nodeType_t {
	PV, NoPV
};


namespace Search {

/**
 * Initialize some search parameters, e.g. MCP values.
 */
void init();

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
template <nodeType_t nodeType>
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
template <nodeType_t nodeType>
value_t quiescence(Thread thread, value_t alpha, value_t beta, int depth);

/**
 * Root function that starts alphaBeta search in iterative deepening framework.
 *
 * @param  b Reference to current board.
 *
 * @returns value_t Best value found search.
 */
value_t search(board_t* b, stats_t* s, instr_t* i);

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
 * Print search information to console.
 *
 * @param  s This search info printed to console.
 */
void printSearchInfo(stats_t* s);


} // namespace Search

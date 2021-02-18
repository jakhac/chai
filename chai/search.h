#pragma once
#pragma warning(push)
#pragma warning(disable:6386)
#pragma warning(disable:6385)

#include <chrono>
#include <iomanip>

#include "tt.h"
#include "eval.h"
#include "moveOrdering.h"

#define DO_NULL true
#define NO_NULL false

#define IS_PV true
#define NO_PV false

#define R_2 2
#define R_3 3

/**
 * Maximum ply reached in alphaBeta and quiescence search.
 */
extern int selDepth;

/**
 * Maximum score before rescale in history heuristic.
 */
const int HISTORY_MAX = 0x4000;

/**
  Principal variation line used for root alphaBeta call. Contains main line after search finished.
 */
static pv_line_t pvLine[1];

/**
 * Remember checks given in quiescence to detect checkmates. Quiescence depths are stored
 * as absolute values.
 */
static int quiescenceChecks[MAX_DEPTH];

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
int alphaBeta(int alpha, int beta, int depth, Board* b, search_t* s, bool nullOk, bool pvNode, pv_line_t* pvLine);

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
int quiescence(int alpha, int beta, int depth, Board* b, search_t* s, pv_line_t* pvLine);

/**
 * Root function that starts alphaBeta search in iterative deepening framework.
 *
 * @param  b Reference to current board.
 * @param  s Search info containing search parameters.
 *
 * @returns An int.
 */
int search(Board* b, search_t* s);

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
int search_aspiration(Board* b, search_t* s, int depth, int bestScore);

/**
 * Tries to find a single(!) repetition in undoPly array. Used in search to find upcoming draws.
 *
 * @param  b Current board.
 *
 * @returns True if repetition is found in undoPly array, else false.
 */
bool isRepetition(Board* b);

/**
 * Searches the complete undoPly array for a position (including current position)
 * that occurred three times.
 *
 * @param  b Current board.
 *
 * @returns True if three fold repetition is found, else false.
 */
bool isThreeFoldRepetition(Board* b);

/**
 * Checks if current position might be a zugzwang. Considers endgame
 *
 * @param  b	  Current board.
 *
 * @returns True if current position is probably azugzwang, else false.
 */
bool zugzwang(Board* b);

/**
 * Swaps the best move found in moveList with move at current index.
 *
 * @param  b	  Current board.
 * @param  move_s Move struct with all moves and scores.
 * @param  curIdx Current index in moveList.
 */
void getNextMove(Board* b, moveList_t* move_s, int curIdx);

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
void printSearchInfo(Board* b, search_t* s);


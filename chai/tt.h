#pragma once

#include "board.h"
#include "moveGenerator.h"

/**
 * Size of the transposition table.
 */
const int ttSize = 0x100000 * 128;

/**
 * Size of the pawn table.
 */
const int pawnTableSize = 0x100000 * 8;

/**
 * Initialize transposition table: Clear used or allocated memory and re-allocate.
 *
 * @param  pvTable_s Transposition table.
 */
void initTT(ttable_t* pvTable_s);

/**
 * Reset all variables in transposition table. Only used in initialization or new game.
 *
 * @param  pvTable_s Transposition table.
 */
void clearTT(ttable_t* pvTable_s);

/**
 * Store a transposition entry containing score, score flag, move and depth in in transposition
 * table if possible.
 *
 * @param  b	 Current board.
 * @param  move  Best move found for current board.
 * @param  score Best score for current board.
 * @param  flag  Flag for score type.
 * @param  depth Depth used in search for move and score.
 */
void storeTT(Board* b, int move, int score, int flag, int depth);

/** TODO rework
 * Probe pv
 *
 * @param  b A Board to process.
 *
 * @returns An int.
 */
int probePV(Board* b);

/**
 * Probe the transposition table. Always set the pv move, even if depth is not deep enough. If
 * stored depth is greater equal to the current depth, overwrite score reference if current
 * score cannot beat alpha/beta anymore.
 *
 * @param  b	 Current board.
 * @param  move  Reference to move variable used in search.
 * @param  score Reference to score variable used in search.
 * @param  alpha Current lower bound found in search.
 * @param  beta  Current upper bound found in search.
 * @param  depth Current depth of search.
 *
 * @returns True if probing was successfull and alpha/beta/score was improved.
 */
bool probeTT(Board* b, int* move, int* score, int alpha, int beta, int depth);

/**
 * Walk through best move stored in transposition table to collect principal variation line.
 *
 * @param  b	    Current board.
 * @param  maxDepth Depth of principal variation.
 *
 * @returns Length of principal variation found in ttable.
 */
int getPVLine(Board* b, const int maxDepth);

/**
 * Initialize pawn table. Re-allocate memory.
 *
 * @param  pawnTable Pawn table declared in board.
 */
void initPawnTable(pawntable_t* pawnTable);

/**
 * Reset all variables used in pawn table. Only used before new game.
 *
 * @param  pawnTable Pawn table to clear.
 */
void clearPawnTable(pawntable_t* pawnTable);

/**
 * Store a pawn table entry with pawn evaluation score.
 *
 * @param  b    Current board.
 * @param  eval Score for pawn structure.
 */
void storePawnEntry(Board* b, const int eval);

/**
 * Probe pawn table and return score if found.
 *
 * @param  b Current board.
 *
 * @returns An int.
 */
int probePawnEntry(Board* b);
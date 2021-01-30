#pragma once

#include <stdint.h>

#include "board.h"
#include "moveGenerator.h"

/*
* Determines the number of buckets used in ttable.
*/
#define BUCKETS 1

/**
 * Size of the transposition table.
 */
const int ttSize = 0x100000 * 128;

/**
 * Size of the pawn table.
 */
const int pawnTableSize = 0x100000 * 8;

/*
* Determines the number of buckets used in ttable.
*/
const int BUCKETS = 3;

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
 * Prefetch first tt entry of bucket into cache using assembly instructions.
 *
 * @param  key The zobrist hash of current board.
 */
void prefetchTTEntry(Board* b);

/**
 * Store a transposition entry containing score, score flag, move and depth in in transposition
 * table if possible. Entries are always written to empty bucket-entries if possible. If all
 * full, the entry with lowest depth (longest to root) is replaced.
 *
 * @param  b	 Current board.
 * @param  move  Best move found for current board.
 * @param  score Best score for current board.
 * @param  flag  Flag for score type.
 * @param  depth Depth used in search for move and score.
 */
void storeTT(Board* b, int move, int score, int flag, int depth);

/**
 * Probe PV move from ttable.
 *
 * @param  b A Board to process.
 *
 * @returns The hash move if found, else NO_MOVE.
 */
move_t probePV(Board* b);

/**
 * Checks if score from ttable is mate score. Update score according with ply to make engine visible to
 * checkmate paths.
 *
 * @param  b	 The current board.
 * @param  score The score to check.
 */
void ttableScoreChecker(Board* b, move_t* score);

/**
 * Probe the transposition table. If a hash entry with equal zobristKey is found, all
 * information are written to referenced parameters. AlphaBeta search then decides if and
 * how to use these. Iterates over all entries inside a bucket.
 *
 * @param  b		 Current board.
 * @param  move		 Move of best entry.
 * @param  hashScore Score of stored entry.
 * @param  hashFlag  Flag of stored entry.
 * @param  hashDepth Depth of stored entry.
 *
 * @returns True if hash entry was found, else false.
 */
bool probeTT(Board* b, move_t* move, int* hashScore, int* hashFlag, int* hashDepth);

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
 * Prefetch pawn entry in cache line using assembly instruction.
 *
 * @param  b The current board.
 */
void prefetchPawnEntry(Board* b);

/**
 * Probe pawn table and return score if found.
 *
 * @param  b		 Current board.
 * @param  hashScore The pointer to hash score.
 *
 * @returns True if hash was found and score is assigned.
 */
bool probePawnEntry(Board* b, int* hashScore);

/**
 * Free memory allocated for ttable and ptable.
 *
 * @param  b The board with both hash tables.
 */
void destroyTranspositionTables(Board* b);


void printTTStatus(Board* b);

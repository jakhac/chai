#pragma once

#include <math.h> // pow

#include "moveGenerator.h"

#ifdef __GNUC__
#define prefetch(x) __builtin_prefetch(x);
#else
#define prefetch(x) _m_prefetch(x);
#endif

const size_t DEFAULT_TT_SIZE = 256;
const size_t DEFAULT_PT_SIZE = 8;

const size_t MIN_TT_SIZE = 2;
const size_t MAX_TT_SIZE = 8192;
const size_t MAX_PT_SIZE = 16;

const int QS_DEPTH = -1;
const int QS_DEPTH_CHECK = 0;


/**
 * Initialize both TT and PT with default MB size.
 */
void initHashTables(board_t* b);

/**
 * Free memory allocated for ttable and ptable.
 *
 * @param  b The board with both hash tables.
 */
void freeHashTables(ttable_t* tt, pawntable_t* pt);

/**
 * Initialize transposition table: Clear used or allocated memory and re-allocate.
 *
 * @param  pvTable_s Transposition table.
 */
bool resizeHashTables(ttable_t* tt, pawntable_t* pt, size_t newMbSize);

/**
 * Reset all variables in transposition table. Only used in initialization or new game.
 *
 * @param  pvTable_s Transposition table.
 */
void clearTT(ttable_t* pvTable_s);

/**
 * Reset all variables used in pawn table. Only used before new game.
 *
 * @param  pawnTable Pawn table to clear.
 */
void clearPT(pawntable_t* pawnTable);

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
void storeTT(board_t* b, move_t move, value_t value, value_t staticEval, int flag, int depth);

/**
 * Store a pawn table entry with pawn evaluation score.
 *
 * @param  b    Current board.
 * @param  eval Score for pawn structure.
 */
void storePT(board_t* b, const value_t eval);

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
bool probeTT(board_t* b, move_t* move, value_t* hashValue, value_t* hashEval, uint8_t* hashFlag, int8_t* hashDepth);

/**
 * Probe pawn table and return score if found.
 *
 * @param  b		 Current board.
 * @param  hashScore The pointer to hash score.
 *
 * @returns True if hash was found and score is assigned.
 */
bool probePT(board_t* b, value_t* hashScore);

/**
 * Prefetch pawn entry in cache line using assembly instruction.
 *
 * @param  b The current board.
 */
void prefetchPT(board_t* b);

/**
 * Prefetch first tt entry of bucket into cache using assembly instructions.
 *
 * @param  key The zobrist hash of current board.
 */
void prefetchTT(board_t* b);


/**
 * Probe PV move from ttable.
 *
 * @param  b A board_t to process.
 *
 * @returns The hash move if found, else NO_MOVE.
 */
move_t probePV(board_t* b);

/**
 * Checks if score from ttable is mate score. Update score according with ply to
 * make engine visible to checkmate paths.
 *
 * @param  b	 The current board.
 * @param  score The score to check.
 */
int hashToSearch(board_t* b, value_t score);

/**
 * Checks if score from search is mate score. Update score according with ply to
 * make engine visible to checkmate paths.
 *
 * @param  b	 The current board.
 * @param  score The score to check.
 */
int searchToHash(board_t* b, value_t score);


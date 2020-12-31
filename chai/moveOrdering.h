#pragma once

#include "moveGenerator.h"

/*
* Move Ordering:
* 1. Hash Move from PV / HashTable
*	 CAPTURES, PROMS
* 2. Good captures (caps with SEE > 0)
* 4. Promoting pawn with capture
* 5. Promoting pawn
* 6. Equal captures (caps with SEE = 0)
*	 QUIET
* 7. Both standard killer moves (quiet)
* 8. Rest of quiet moves ordered by history heuristic
*	 LOSING CAPS
* 9. Losing captures
*/

// pv move
const int HASH_MOVE = 20000;

// caps, proms and ep
const int GOOD_CAPTURE = 15000;
const int PROMOTING_CAPTURE = 13000;
const int PROMOTION = 12000;
const int EQUAL_CAPTURE = 10000;

// quiet moves
const int MATE_KILLER = 9000;
const int KILLER_SCORE_1 = 8000;
const int KILLER_SCORE_2 = 7000;
const int COUNTER_SCORE = 6000;
const int QUIET_SCORE = 5000;

const int CASTLE_SCORE = 500;

static int MVV_LVA[13][13];


/**
 * Initialize mvv-lva array. Only used once in start.
 */
void initMVV_LVA();

/**
 * Gets least valuable piece
 *
 * @param  b	    Board to find least valuable piece on.
 * @param  atkSet   Bitboard with all attackers and defenders.
 * @param  side	    The attacking side.
 * @param  atkPiece The initial attacking piece.
 *
 * @returns The bitboard with least valuable piece.
 */
bitboard_t getLeastValuablePiece(Board* b, bitboard_t atkSet, int side, int atkPiece);

/**
 * Calculates static exchange evaluation starting with given move.
 *
 * @param  b    Current board.
 * @param  move Capturing move to start exchange.
 *
 * @returns Final score in centipawns after all exchanges have been made.
 */
int see(Board* b, const int move);

/**
 * Score moves according to moveOrdering.h rules. After scoring every move, best move is swapped
 * to last position (moves[cnt] = bestMove).
 *
 * @param  b	    Reference to board.
 * @param  moveList Reference to moveList to score moves in.
 * @param  hashMove Hash move found in pvLine or ttable.
 */
void scoreMoves(Board* b, moveList_t* moveList, move_t hashMove);

/**
 * Update the index of the move with highest score yet. Used to swap best move to the end after
 * all moves are scored.
 *
 * @param  scores  Reference to scores in moveList_t.
 * @param  bestIdx Reference to bestIdx of move with highest score yet.
 * @param  curIdx  Current index of loop.
 */
static void updateBestMove(int* scores, int* bestIdx, int curIdx);


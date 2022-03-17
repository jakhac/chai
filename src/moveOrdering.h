#pragma once

#include "moveGenerator.h"
#include "thread.h"

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

namespace MoveOrder {

// PV move
const int HASH_MOVE         = 20000;

// Captures, promotions and enpas
const int PROMOTING_CAPTURE = 16000;
const int GOOD_CAPTURE      = 15000;
const int PROMOTION         = 12000;
const int BAD_CAPTURE       = 0;

// Quiet moves
const int MATE_KILLER       = 9000;
const int KILLER_SCORE_1    = 8000;
const int KILLER_SCORE_2    = 7000;
const int COUNTER_SCORE     = 6000;
const int QUIET_SCORE       = 5000;
const int CASTLE_SCORE      = QUIET_SCORE + 500;


// No MVV-LVA score is greater than this upper bound.
const int MVV_LVA_UBOUND = 1000;

extern int MVV_LVA[13][13];

// victim scores used to calculate mvv lva score
const int victimScore[13] = { 
    0, 100, 200, 300, 400, 500, 600, 100, 200, 300, 400, 500, 600 
};

const int SEEPieceValues[13] = {
    0, 100, 325, 325, 550, 1100, 0, 100, 325, 325, 550, 1100, 0
};


/**
 * Initialize mvv-lva array. Only used once in start.
 */
void init();

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
Bitboard getLeastValuablePiece(Board* b, Bitboard atkSet, int side);

/**
 * Calculates static exchange evaluation starting with given move. Fully calculates all
 * till there are no captures to be made. Currently used to order alphaBeta movegen.
 *
 * @param  b    Current board.
 * @param  move Capturing move to start exchange.
 *
 * @returns Final score in centipawns after all exchanges have been made.
 */
int see(Board* b, const int move);

/**
 * Calculate the see score and compare against a given treshold.
 *
 * @param b
 * @param move
 * @param threshHold
 * @return True if see score is greater than the threshold, else false.
 */
bool see_ge(Board* b, Move move, int threshHold);

/**
 * Score moves according to moveOrdering.h rules. After scoring every move, best move is swapped
 * to first position. (not yet)
 *
 * @param  Thread   Reference to the thread.
 * @param  moveList Reference to moveList to score moves in.
 * @param  hashMove Hash move found in pvLine or ttable.
 */
void scoreMoves(Thread thread, MoveList* moveList, Move hashMove);

} // namespace MoveOrder

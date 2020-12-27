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

/// <summary>
/// Score moves according to moveOrdering.h rules. After scoring every move,
/// best move is swapped to last position (moves[cnt] = bestMove).
/// </summary>
/// <param name="b">Reference to board</param>
/// <param name="moveList">Reference to moveList to score moves in</param>
/// <param name="hashMove">Rerefence to hash move fonud in pvLine or ttable</param>
/// <returns>Amount of major pieces</returns>
void scoreMoves(Board* b, moveList_t* moveList, move_t* hashMove);

/// <summary>
/// Update the index of the move with highest score yet. Used to swap best move to the
/// end after all moves are scored.
/// </summary>
/// <param name="scores">Reference to scores in moveList_t</param>
/// <param name="bestIdx">Reference to bestIdx of move with highest score yet</param>
/// <param name="curIdx">Current index of loop</param>
static void updateBestMove(int* scores, int* bestIdx, int curIdx);



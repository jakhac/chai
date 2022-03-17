#pragma once

#include "board.h"


const Value PAWN_LACK_PENALTY        = -16;
const Value BACKWARD_PAWN_PENALTY    = -3;
const Value DOUBLED_PAWN_PENALTY     = -4;
const Value PAWN_ISLAND_PENALTY      = -2;
const Value PAWN_ISOLATION_PENALTY   = -10;
const Value HIDDE_PASSED_PAWN_REWARD = 3;
const Value PAWN_CHAIN_REWARD        = 1;
const Value PAWN_SHIELD_REWARD       = 5;

// const Value PASSED_PAWN_REWARD = 8;
const Value PASSED_PAWN_REWARD[2][8] = {
    { 200, 110, 75, 35, 20, 10, 5, 0 },
    { 0, 5, 10, 20, 35, 75, 110, 200 }
};


/**
 * @brief Evaluate pawn complete pawn structure. The computed value is not
 * interpolated between opening and endgame influence yet.
 */
Value evaluatePawns(Board* b);

/**
 * @brief Evaluate pawn structure with respect to king-safety.
 */
template<Color color>
Value kingPawnSafety(Board* b);

/**
 * @brief Return a bitboard of all white backward pawns
 * Backwards Pawn:
 * 		1) 2-rank || 7-rank
 * 		2) On semi-open file (no more pawns on this file)
 * 		3) Stop-square controlled by opponent pawn
 * 		4) Has no pawn-neighbor on same rank
 */
Bitboard wBackwardPawns(Board* b);

/**
 * @brief Return a bitboard of all black backward pawns
 */
Bitboard bBackwardPawns(Board* b);

/**
 * @brief Return number of doubled pawns for given color.
 */
template<Color color>
int doubledPawns(Board* b);

/**
 * @brief Return number of pawn islands for given color.
 */
template<Color color>
int pawnIslands(Board* b);

/**
 * @brief Return number of isolated pawns for given color.
 */
template<Color color>
int isolatedPawns(Board* b);

/**
 * @brief Return passed-pawns-score for given color based on distance to promotion.
 */
template<Color color>
Value passedPawns(Board* b);

/**
 * @brief Return the number of hidden assed pawns for given color.
 */
template<Color color>
int hiddenPassedPawns(Board* b);

/**
 * @brief Count pawn chains for given color.
 */
template<Color color>
Value pawnChain(Board* b);

/**
 * @brief Return the number of { Knight, Bishop, Rook, Queen } pieces
 * for given side.
 */
int nonPawnPieces(Board* b, Color color);

/**
 * @brief Check if there are non-pawn (and king) pieces left on the board.
 */
bool nonPawnPieces(Board* b);


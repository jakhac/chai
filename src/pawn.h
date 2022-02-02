#pragma once

#include "board.h"


const value_t PAWN_LACK_PENALTY        = -16;
const value_t BACKWARD_PAWN_PENALTY    = -3;
const value_t DOUBLED_PAWN_PENALTY     = -4;
const value_t PAWN_ISLAND_PENALTY      = -2;
const value_t PAWN_ISOLATION_PENALTY   = -10;
const value_t HIDDE_PASSED_PAWN_REWARD = 3;
const value_t PAWN_CHAIN_REWARD        = 1;
const value_t PAWN_SHIELD_REWARD       = 5;

// const value_t PASSED_PAWN_REWARD = 8;
const value_t PASSED_PAWN_REWARD[2][8] = {
	{ 200, 110, 75, 35, 20, 10, 5, 0 },
	{ 0, 5, 10, 20, 35, 75, 110, 200 }
};


/**
 * @brief Evaluate pawn complete pawn structure. The computed value is not
 * interpolated between opening and endgame influence yet.
 */
value_t evaluatePawns(board_t* b);

/**
 * @brief Evaluate pawn structure with respect to king-safety.
 */
template<color_t color>
value_t kingPawnSafety(board_t* b);

/**
 * @brief Return a bitboard of all white backward pawns
 * Backwards Pawn:
 * 		1) 2-rank || 7-rank
 * 		2) On semi-open file (no more pawns on this file)
 * 		3) Stop-square controlled by opponent pawn
 * 		4) Has no pawn-neighbor on same rank
 */
bitboard_t wBackwardPawns(board_t* b);

/**
 * @brief Return a bitboard of all black backward pawns
 */
bitboard_t bBackwardPawns(board_t* b);

/**
 * @brief Return number of doubled pawns for given color.
 */
template<color_t color>
int doubledPawns(board_t* b);

/**
 * @brief Return number of pawn islands for given color.
 */
template<color_t color>
int pawnIslands(board_t* b);

/**
 * @brief Return number of isolated pawns for given color.
 */
template<color_t color>
int isolatedPawns(board_t* b);

/**
 * @brief Return passed-pawns-score for given color based on distance to promotion.
 */
template<color_t color>
value_t passedPawns(board_t* b);

/**
 * @brief Return the number of hidden assed pawns for given color.
 */
template<color_t color>
int hiddenPassedPawns(board_t* b);

/**
 * @brief Count pawn chains for given color.
 */
template<color_t color>
value_t pawnChain(board_t* b);

/**
 * @brief Return the number of { Knight, Bishop, Rook, Queen } pieces
 * for given side.
 */
int nonPawnPieces(board_t* b, color_t color);

/**
 * @brief Check if there are non-pawn (and king) pieces left on the board.
 */
bool nonPawnPieces(board_t* b);


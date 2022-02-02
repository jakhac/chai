#pragma once

#include "endgame.h"
#include "pawn.h"
#include "psqt.h"
#include "timeMan.h"
#include "./nnue/nnue.h"
#include "attacks.h"

#include "bitboard.h"


const value_t VALUE_INFTY      = 32000;
const value_t VALUE_MATE       = 31000;
const value_t VALUE_IS_MATE_IN = VALUE_MATE - (MAX_DEPTH * 2);
const value_t VALUE_NONE       = VALUE_INFTY + 1;
const value_t VALUE_WIN        = 10000;
const value_t VALUE_LOSS       = -VALUE_WIN;
const value_t VALUE_TB_WIN     = VALUE_MATE - 1000;


inline tuple_t t(value_t v1, value_t v2) {
	return (v2 << 16) + (v1);
}

inline value_t t1(tuple_t tuple) {
	return (int16_t)((uint16_t)((unsigned)(tuple)));
}

inline value_t t2(tuple_t tuple) {
	return (int16_t)((uint16_t)((unsigned)(tuple + 0x8000) >> 16));
}

// namespace chai {

// namespace Eval {


const value_t pieceValues[13] = {
	0, 100, 325, 325, 550, 1000,
	0, 100, 325, 325, 550, 1000, 
	0
};


const tuple_t pieceTupleValues[13] = {
	0, t(100, 160), t(325, 310), t(325, 340), t(550, 640), t(1000, 1150),
	0, t(100, 160), t(325, 310), t(325, 340), t(550, 640), t(1000, 1150),
	0
};


// Maximum material in pawns excluding kings
const int maximumMaterial = 78;



/**
 * @brief Returns the number of squares where pType can check uncontestedly.
 */
template<pType_t pType>
int undefendedChecksFrom(board_t* b, int kSq, color_t col);

/**
 * @brief Return true if rookSq1 and rookSq2 are aligned and connected.
 */
bool rooksConnected(board_t* b, int rookSq1, int rookSq2);

/**
 * Main evaluation function. Returns static board evaulation.
 * @returns Score in centipawns.
 */
value_t evaluation(board_t* b);

/**
 * @brief Return the material score as value_t.
 */
value_t materialScore(board_t* b);

/**
 * @copydoc materialScore() for tuple_t.
 */
tuple_t materialTupleScore(board_t* b);

/**
 * @brief Return true if a rook is above/below a passed pawn
 * @param above True if rook should control passer from above
 */
bool rookControlsPasser(bitboard_t rooks, bitboard_t passer, bool above);

/**
 * @brief Calculate complete PSQT balance for given psqtTable, but only used for assertions!
 */
value_t calcPSQT(board_t* b, const value_t* psqtTable[64]);

/**
 * @brief Returns true if material on board is insufficient for a decisive result.
 */
bool insufficientMaterial(board_t* b);



// } // namespace Eval

// } // namespace chai

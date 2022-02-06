#pragma once

#include "endgame.h"
#include "pawn.h"
#include "psqt.h"
#include "timeMan.h"
#include "./nnue/nnue.h"
#include "attacks.h"

#include "bitboard.h"


const Value VALUE_INFTY      = 32000;
const Value VALUE_MATE       = 31000;
const Value VALUE_IS_MATE_IN = VALUE_MATE - (MAX_DEPTH * 2);
const Value VALUE_NONE       = VALUE_INFTY + 1;
const Value VALUE_WIN        = 10000;
const Value VALUE_LOSS       = -VALUE_WIN;
const Value VALUE_TB_WIN     = VALUE_MATE - 1000;


inline Tuple t(Value v1, Value v2) {
    return (v2 << 16) + (v1);
}

inline Value t1(Tuple tuple) {
    return (int16_t)((uint16_t)((unsigned)(tuple)));
}

inline Value t2(Tuple tuple) {
    return (int16_t)((uint16_t)((unsigned)(tuple + 0x8000) >> 16));
}

// namespace chai {

// namespace Eval {


const Value pieceValues[13] = {
    0, 100, 325, 325, 550, 1000,
    0, 100, 325, 325, 550, 1000, 
    0
};


const Tuple pieceTupleValues[13] = {
    0, t(100, 160), t(325, 310), t(325, 340), t(550, 640), t(1000, 1150),
    0, t(100, 160), t(325, 310), t(325, 340), t(550, 640), t(1000, 1150),
    0
};


// Maximum material in pawns excluding kings
const int maximumMaterial = 78;



/**
 * @brief Returns the number of squares where pType can check uncontestedly.
 */
template<PieceType pType>
int undefendedChecksFrom(Board* b, int kSq, Color col);

/**
 * @brief Return true if rookSq1 and rookSq2 are aligned and connected.
 */
bool rooksConnected(Board* b, int rookSq1, int rookSq2);

/**
 * Main evaluation function. Returns static board evaulation.
 * @returns Score in centipawns.
 */
Value evaluation(Board* b);

/**
 * @brief Return the material score as Value.
 */
Value materialScore(Board* b);

/**
 * @copydoc materialScore() for Tuple.
 */
Tuple materialTupleScore(Board* b);

/**
 * @brief Return true if a rook is above/below a passed pawn
 * @param above True if rook should control passer from above
 */
bool rookControlsPasser(Bitboard rooks, Bitboard passer, bool above);

/**
 * @brief Calculate complete PSQT balance for given psqtTable, but only used for assertions!
 */
Value calcPSQT(Board* b, const Value* psqtTable[64]);

/**
 * @brief Returns true if material on board is insufficient for a decisive result.
 */
bool insufficientMaterial(Board* b);



// } // namespace Eval

// } // namespace chai

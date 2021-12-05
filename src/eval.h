#pragma once

#include "pawn.h"
#include "psqt.h"
#include "timeMan.h"

const value_t VALUE_INFTY = 32000;
const value_t VALUE_MATE = 31000;
const value_t VALUE_IS_MATE_IN = VALUE_MATE - (MAX_DEPTH * 2);
const value_t VALUE_NONE = VALUE_INFTY + 1;
const value_t VALUE_WIN = 10000;
const value_t VALUE_TB_WIN = VALUE_MATE - 1000;


const value_t pieceValues[13] = {
	0, 100, 325, 325, 550, 1000,
	0, 100, 325, 325, 550, 1000, 0
};

const value_t kingZoneTropism[9] = { 0, 40, 60, 77, 87, 92, 95, 97, 100 };

const value_t openFileBonusR = 10;
const value_t openFileBonusQ = 5;

// Maximum material in pawns excluding kings
const int maximumMaterial = 78;


/**
 * Main evaluation function. Returns static board evaulation.
 * @returns Score in centipawns.
 */
value_t evaluation(board_t* b);

/**
 * Lazy evaluation function calculates static board evaulation. Considers piece values and
 * PST sum.
 * @returns Score in centipawns.
 */
value_t lazyEvaluation(board_t* b);


// 
value_t materialScore(board_t* b);
value_t openFilesRQ(board_t* b, color_t color);
value_t bishopPair(board_t* b, color_t color);
value_t kingSafety(board_t* b, color_t color, float* t);
value_t mobility(board_t* b, color_t color, float* t);
float interpolate(int a, int b, float t);
int scale(int scaler, int pressure);





/**
 * Return superiority/inferiority based on material on board.
 *
 * @param  b board_t.
 *
 * @returns Superiority/inferiority score viewed as player to move (signed)
 */
value_t contemptFactor(board_t* b);

/**
 * @brief Calculate complete PSQT balance for given psqtTable
 * 
 * @note Not used during main search but for assertions only.
 *  
 * @param b 
 * @param psqtTable 
 * @return value_t 
 */
value_t calcPSQT(board_t* b, const value_t* psqtTable[64]);

bool insufficientMaterial(board_t* b);

extern const int* maps[7];
extern bitboard_t pawnIsolatedMask[64];
extern bitboard_t pawnPassedMask[2][64];
extern bitboard_t upperMask[64];
extern bitboard_t lowerMask[64];
extern bitboard_t pawnShield[2][64];
extern bitboard_t xMask[64];

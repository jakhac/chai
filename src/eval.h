#pragma once

#include "pawn.h"
#include "psqt.h"
#include "timeMan.h"
#include "./nnue/transformer.h"


const int pPhase   = 0;
const int nPhase   = 1;
const int bPhase   = 1;
const int rPhase   = 2;
const int qPhase   = 4;
const int maxPhase = pPhase*16 + nPhase*4 + bPhase*4 + rPhase*4 + qPhase*2;

const value_t VALUE_INFTY      = 32000;
const value_t VALUE_MATE       = 31000;
const value_t VALUE_IS_MATE_IN = VALUE_MATE - (MAX_DEPTH * 2);
const value_t VALUE_NONE       = VALUE_INFTY + 1;
const value_t VALUE_WIN        = 10000;
const value_t VALUE_TB_WIN     = VALUE_MATE - 1000;

// Maximum material in pawns excluding kings
const int maximumMaterial = 78;
// position startpos moves e2e4 e7e5 g1f3 b8c6 b1c3 g8f6 f1b5 f8b4 e1g1 e8g8 d2d3 d7d6 c3e2 c8g4 c2c3 b4c5 d3d4 e5d4 c3d4 c5b6 b5c6 b7c6 e2g3 d6d5 e4e5 f6d7 c1e3 c6c5 d4c5 d7c5 h2h3 g4c8 e3c5 b6c5 a1c1 c5b6 d1d3 c7c5 g3e2 c5c4 d3d2 c8b7 e2d4 d8d7 d2g5 g8h8 a2a3 h7h6 g5f5 b7c8 f5f4 b6c5 f4e3 a8b8 e3c3 c5e7 c3c2 d7b7 c1b1 b7c7 b1c1 c8d7 c2d2 c7b6 c1c2 d7a4 c2c1 e7c5 c1b1 f8e8 d2f4 b6g6 f3d2 g6d3 d2f3 b8b7 f4e3 d3e3 f2e3 e8b8 f1f2 a7a5 f3e1 h8g8 e1f3 a4e8 e5e6 f7e6 g2g4 c5a3 d4e6 c4c3 f3h4 b7b2 f2b2 b8b2

// const tuple_t pieceValues[13] = { 
// 	t(0, 0), t(100, 150), t(325, 300), t(325, 350), t(550, 575), t(1000, 1000),
// 	t(0, 0), t(100, 150), t(325, 300), t(325, 350), t(550, 575), t(1000, 1000)
// };

const value_t pieceValues[13] = {
	0, 100, 325, 325, 550, 1000,
	0, 100, 325, 325, 550, 1000, 
	0
};

const tuple_t pieceTupleValues[13] = {
	0, t(100, 150), t(325, 305), t(325, 325), t(550, 550), t(1000, 1000),
	0, t(100, 150), t(325, 305), t(325, 325), t(550, 550), t(1000, 1000),
	0
};

const value_t CENTER_OCCUPIED = 2;
const value_t CENTER_ATTACKED = 2;

const tuple_t BISHOP_MOBILITY = t(2, 2);
const tuple_t KNIGHT_MOBILITY = t(2, 1);
const tuple_t ROOK_MOBILITY   = t(1, 2);

const tuple_t BISHOP_PAIR              = t(24, 58);
const tuple_t BISHOP_COLOR_BLOCKED     = t(-2, -10);
const tuple_t BISHOP_FIANCHETTO        = t(17, 0);
const tuple_t BISHOP_STRONG_COLOR_CPLX = t(10, 28);
const tuple_t BISHOP_OUTPOST           = t(15, -10);
const tuple_t BISHOP_OUTPOST_DEFENDED  = t(35, 0);

const tuple_t KNIGHT_OUTPOST          = t(18, -4);
const tuple_t KNIGHT_OUTPOST_DEFENDED = t(30, 16);
const tuple_t KNIGHT_CENTER_ATTACKS   = t(3, 1);
const tuple_t KNIGHT_BORDER_SQUARE    = t(-7, -14);
const tuple_t KNIGHT_SHIELDED_BY_PAWN = t(18, 4);

const tuple_t ROOK_CONTROLS_PASSER   = t(5, 15);
const tuple_t ROOK_SAME_FILE_AS_KING = t(15, 0);
const tuple_t ROOK_OPEN_FILE         = t(40, 8);
const tuple_t ROOK_SEMI_OPEN_FILE    = t(10, 8);
const tuple_t ROOK_ON_SEVENTH        = t(-1, 33);
const tuple_t ROOK_CONNECTED         = t(10, 15);

const tuple_t QUEEN_EARLY_DEVELOPMENT = t(-5, 0);
const tuple_t QUEEN_DANGEROUS_SQUARE  = t(-7, -7);
// const tuple_t QUEEN_CENTER_SIGHT      = t(2, 3);

const tuple_t KING_DANGEROUS_SQUARE   = t(-15, -25);
const tuple_t KING_CHECK_UNDEF_KNIGHT = t(-3, -3);
const tuple_t KING_CHECK_UNDEF_BISHOP = t(-4, -4);
const tuple_t KING_CHECK_UNDEF_ROOK   = t(-7, -7);
const tuple_t KING_CHECK_UNDEF_QUEEN  = t(-6, -6);
const tuple_t KING_ACTIVITY			  = t(0,  15);

const tuple_t T_KING_OPEN_NEIHGBOR_FILE  = t(-15, 0);
const tuple_t T_KING_OPEN_FILE           = t(-23, 0);

const int KNIGHT_UNIT = 3;
const int BISHOP_UNIT = 3;
const int ROOK_UNIT   = 4;
const int QUEEN_UNIT  = 6;
const value_t safetyTable[100] = {
	  0,   0,   1,   2,   3,   5,   7,   9,  12,  15,
	 18,  22,  26,  30,  35,  39,  44,  50,  56,  62,
	 68,  75,  82,  85,  89,  97, 105, 113, 122, 131,
	140, 150, 169, 180, 191, 202, 213, 225, 237, 248,
	260, 272, 283, 295, 307, 319, 330, 342, 354, 366,
	377, 389, 401, 412, 424, 436, 448, 459, 471, 483,
	494, 500, 500, 500, 500, 500, 500, 500, 500, 500,
	500, 500, 500, 500, 500, 500, 500, 500, 500, 500,
	500, 500, 500, 500, 500, 500, 500, 500, 500, 500,
	500, 500, 500, 500, 500, 500, 500, 500, 500, 500
};
const value_t kingZoneTropism[9] = { 0, 40, 60, 77, 87, 92, 95, 97, 100 };


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

tuple_t materialTupleScore(board_t* b); // TODO
value_t materialScore(board_t* b);
tuple_t squareControl(board_t* b, color_t color);
tuple_t mobility(board_t* b, color_t color);

tuple_t evaluateBishops(board_t* b, color_t color);
tuple_t evaluateKnights(board_t* b, color_t color);
tuple_t evaluateRooks(board_t* b, color_t color);
tuple_t evaluateQueens(board_t* b, color_t color);
tuple_t evaluateKing(board_t* b, color_t color);


/**
 * @brief Returns true if both squares are connected either horizontal or vertical.
 */
bool rooksConnected(board_t* b, int sq1, int sq2);

/**
 * @brief Returns true if at least one rook sits behind one of the passers.
 */
bool rooksBelowPasser(bitboard_t rooks, bitboard_t passer);

/**
 * @brief Returns true if at least one rook sits in front of one of the passers.
 */
bool rooksAbovePasser(bitboard_t rooks, bitboard_t passer);

/**
 * @brief Return number of undefended square where knights can deliver check.
 * 
 * @param b 
 * @param kSq Square of king that receives check
 * @param color Color of piece that delivers check
 */
int undefendedKnightChecks(board_t* b, int kSq, color_t color);

/**
 * @brief Return number of undefended square where bishops that can deliver check.
 * 
 * @param b 
 * @param kSq Square of king that receives check
 * @param color Color of piece that delivers check
 */
int undefendedBishopChecks(board_t* b, int kSq, color_t color);

/**
 * @brief Return number of undefended square where rooks that can deliver check.
 * 
 * @param b 
 * @param kSq Square of king that receives check
 * @param color Color of piece that delivers check
 */
int undefendedRookChecks(board_t* b, int kSq, color_t color);

/**
 * @brief Return number of undefended square wherequeens that can deliver check.
 * 
 * @param b 
 * @param kSq Square of king that receives check
 * @param color Color of piece that delivers check
 */
int undefendedQueenChecks(board_t* b, int kSq, color_t color);

/**
 * @brief Return superiority/inferiority based on material 
 * on board viewed as player to move (signed).
 */
value_t contemptFactor(board_t* b);

/**
 * @brief Calculate complete PSQT balance for given psqtTable.
 */
value_t calcPSQT(board_t* b, const value_t* psqtTable[64]);

/**
 * @brief Returns true if material on board is insufficient for a decisive result.
 */
bool insufficientMaterial(board_t* b);


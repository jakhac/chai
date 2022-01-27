#pragma once

#include "endgame.h"
#include "pawn.h"
#include "psqt.h"
#include "timeMan.h"


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


const tuple_t KNIGHT_MOBILITY[9] = {
	t(-41, -51), t(-34, -37), t(-15, -28), 
	t( -4, -15), t(  8,  12), t( 14,  16), 
	t( 20,  24), t( 28,  24), t( 37,  20)
};

const tuple_t BISHOP_MOBILITY[14] = {
	t(-57, -89), t(-24, -67), t(  8,  -8),
	t( 17,   8), t( 31,  13), t( 39,  24),
	t( 41,  28), t( 42,  34), t( 43,  39),
	t( 45,  47), t( 55,  58), t( 68,  70),
	t( 89,  88), t( 94,  90)
};

const tuple_t ROOK_MOBILITY[15] = {
	t(-67, -82), t(-34, -82), t(-14, -23),
	t(-10,   3), t( -8,  20), t( -6,  27),
	t( 10,  39), t( 11,  42), t( 13,  52),
	t( 22,  60), t( 36,  75), t( 45,  90),
	t( 55, 100), t( 58, 105), t( 59, 110)
};


const tuple_t BISHOP_PAIR              = t(30, 69);
const tuple_t BISHOP_OUTPOST_DEFENDED  = t(17,  10);
const tuple_t BISHOP_LONG_DIAGONAL     = t(29,  1);

const tuple_t KNIGHT_OUTPOST          = t(12, 12);
const tuple_t KNIGHT_OUTPOST_DEFENDED = t(31, 16);
const tuple_t KNIGHT_BORDER_SQUARE    = t(-7, -14);
const tuple_t KNIGHT_BLOCKS_PAWN      = t(6, 16);

const tuple_t ROOK_CONTROLS_PASSER   = t(0, 12);
const tuple_t ROOK_OPEN_FILE         = t(40, 8);
const tuple_t ROOK_SEMI_OPEN_FILE    = t(10, 8);
const tuple_t ROOK_ON_SEVENTH        = t(-1, 33);
const tuple_t ROOK_CONNECTED         = t(7, 13);

const tuple_t QUEEN_DANGEROUS_SQUARE  = t(-7, -7);

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


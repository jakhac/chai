#pragma once

#include "board.h"
#include "tt.h"

/**
 * Main evaluation function calculates static board evaulation.
 *
 * @param  b Current board to evaluate.
 *
 * @returns Score in centipawns.
 */
int eval(Board* b);

/**
 * Lazy evaluation function calculates static board evaulation. Considers piece values and
 * PST sum.
 *
 * @param  b Current board to evaluate.
 *
 * @returns Score in centipawns.
 */
int lazyEvalulation(Board* b);

bool insufficientMaterial(Board* b);

/**
 * Evaulate all pieces of given side regarding piece square tables. Opening and endgame tables
 * are interpolated based on halfMoves and material.
 *
 * @param  b    Board reference.
 * @param  side Current side to score.
 * @param  t    Interpolation factor.
 *
 * @returns An int.
 */
int evalPST(Board* b, int side, float* t);

int materialScore(Board* b, int side);

int isolatedPawns(Board* b, int side);

int passedPawns(Board* b, int side);

int stackedPawn(Board* b, int side);

int evaluatePawns(Board* b, float* t);

int openFilesRQ(Board* b, int side);

int bishopPair(Board* b, int side);

int kingSafety(Board* b, int side, float* t);

int mobility(Board* b, int side, float* t);

float interpolate(int a, int b, float t);

int scale(int scaler, int pressure);

/**
 * Return superiority/inferiority based on material on board.
 *
 * @param  b Board.
 *
 * @returns Superiority/inferiority score viewed as player to move (signed)
 */
int contemptFactor(Board* b);

const int PAWN_OPEN[64] = {
	0,  0,  0,  0,  0,  0,  0,  0,
	5, 10, 10,-20,-20, 10, 10,  5,
	5, -5,-10,  0,  0,-10, -5,  5,
	0,  0,  0, 20, 20,  0,  0,  0,
	5,  5, 10, 25, 25, 10,  5,  5,
	10, 10, 20, 30, 30, 20, 10, 10,
	50, 50, 50, 50, 50, 50, 50, 50,
	0,  0,  0,  0,  0,  0,  0,  0,
};

const int PAWN_ENDGAME[64] = {
	  0,  0,  0,  0,  0,  0,  0,  0
	- 20,-20,-20,-20,-20,-20,-20,-20,
	  0,  0,  0,  0,  0,  0,  0,  0,
	 10, 10, 10, 10, 10, 10, 10, 10,
	 15, 15, 15, 15, 15, 15, 15, 15,
	 30, 30, 30, 30, 30, 30, 30, 30,
	 80, 80, 80, 80, 80, 80, 80, 80,
	  0,  0,  0,  0,  0,  0,  0,  0,
};

const int KNIGHT_OPEN[64] = {
	-50,-40,-30,-30,-30,-30,-40,-50,
	-40,-20,  0,  5,  5,  0,-20,-40,
	-30,  5, 10, 15, 15, 10,  5,-30,
	-30,  0, 15, 20, 20, 15,  0,-30,
	-30,  5, 15, 20, 20, 15,  5,-30,
	-30,  0, 10, 15, 15, 10,  0,-30,
	-40,-20,  0,  0,  0,  0,-20,-40,
	-50,-40,-30,-30,-30,-30,-40,-50
};

const int KNIGHT_ENDGAME[64] = {
	-50,-40,-30,-30,-30,-30,-40,-50
	- 40,-20,  0,  5,  5,  0,-20,-40,
	-30,  5, 10, 15, 15, 10,  5,-30,
	-30,  0, 15, 20, 20, 15,  0,-30,
	-30,  5, 15, 20, 20, 15,  5,-30,
	-30,  0, 10, 15, 15, 10,  0,-30,
	-40,-20,  0,  0,  0,  0,-20,-40,
	-50,-40,-30,-30,-30,-30,-40,-50,
};

const int BISHOP_OPEN[64] = {
	-20,-10,-10,-10,-10,-10,-10,-20,
	-10,  5,  0,  0,  0,  0,  5,-10,
	-10, 10, 10, 10, 10, 10, 10,-10,
	-10,  0, 10, 10, 10, 10,  0,-10,
	-10,  5,  5, 10, 10,  5,  5,-10,
	-10,  0,  5, 10, 10,  5,  0,-10,
	-10,  0,  0,  0,  0,  0,  0,-10,
	-20,-10,-10,-10,-10,-10,-10,-20
};

const int BISHOP_ENDGAME[64] = {
	-20,-10,-10,-10,-10,-10,-10,-20
	- 10,  5,  0,  0,  0,  0,  5,-10,
	-10, 10, 10, 10, 10, 10, 10,-10,
	-10,  0, 10, 10, 10, 10,  0,-10,
	-10,  5,  5, 10, 10,  5,  5,-10,
	-10,  0,  5, 10, 10,  5,  0,-10,
	-10,  0,  0,  0,  0,  0,  0,-10,
	-20,-10,-10,-10,-10,-10,-10,-20,
};

const int ROOK_OPEN[64] = {
	0,  0, 20,  5,  5, 20,  0,  0,
	5,  0,  0,  0,  0,  0,  0,  5,
   -5,  0,  0,  0,  0,  0,  0, -5,
   -5,  0,  0,  0,  0,  0,  0, -5,
   -5,  0,  0,  0,  0,  0,  0, -5,
   -5,  0,  0,  0,  0,  0,  0, -5,
   -5,  0,  0,  0,  0,  0,  0, -5,
	0,  0,  0,  0,  0,  0,  0,  0
};

const int ROOK_ENDGAME[64] = {
	0,  0,  0,  0,  0,  0,  0,  0,
   -5,  0,  0,  0,  0,  0,  0, -5,
   -5,  0,  0,  0,  0,  0,  0, -5,
   -5,  0,  0,  0,  0,  0,  0, -5,
   -5,  0,  0,  0,  0,  0,  0, -5,
   -5,  0,  0,  0,  0,  0,  0, -5,
   -5,  0,  0,  0,  0,  0,  0, -5,
	0,  0,  0,  0,  0,  0,  0,  0
};

const int QUEEN_OPEN[64] = {
	-20,-10,-10, -5, -5,-10,-10,-20
	- 10,  0,  5,  0,  0,  0,  0,-10,
	-10,  5,  5,  5,  5,  5,  0,-10,
	  0,  0,  5,  5,  5,  5,  0, -5,
	 -5,  0,  5,  5,  5,  5,  0, -5,
	-10,  0,  5,  5,  5,  5,  0,-10,
	-10,  0,  0,  0,  0,  0,  0,-10,
	-20,-10,-10, -5, -5,-10,-10,-20,
};

const int QUEEN_ENDGAME[64] = {
	-20,-10,-10, -5, -5,-10,-10,-20
	- 10,  0,  5,  0,  0,  0,  0,-10,
	-10,  5,  5,  5,  5,  5,  0,-10,
	  0,  0,  5,  5,  5,  5,  0, -5,
	 -5,  0,  5,  5,  5,  5,  0, -5,
	-10,  0,  5,  5,  5,  5,  0,-10,
	-10,  0,  0,  0,  0,  0,  0,-10,
	-20,-10,-10, -5, -5,-10,-10,-20,
};

const int KING_OPENING[64] = {
	 30, 30, 40, 20, 20, 30, 40, 30,
	 20, 20,  0,  0,  0,  0, 20, 20,
	-10,-20,-20,-20,-20,-20,-20,-10,
	-20,-30,-30,-40,-40,-30,-30,-20,
	-30,-40,-40,-50,-50,-40,-40,-30,
	-30,-40,-40,-50,-50,-40,-40,-30,
	-30,-40,-40,-50,-50,-40,-40,-30,
	-30,-40,-40,-50,-50,-40,-40,-30
};

const int KING_ENDGAME[64] = {
	-50,-30,-30,-30,-30,-30,-30,-50
	- 30,-30,  0,  0,  0,  0,-30,-30,
	-30,-10, 20, 30, 30, 20,-10,-30,
	-30,-10, 30, 40, 40, 30,-10,-30,
	-30,-10, 30, 40, 40, 30,-10,-30,
	-30,-10, 20, 30, 30, 20,-10,-30,
	-30,-20,-10,  0,  0,-10,-20,-30,
	-50,-40,-30,-20,-20,-30,-40,-50,
};

const int mirror64[64] = {
	56	,	57	,	58	,	59	,	60	,	61	,	62	,	63	,
	48	,	49	,	50	,	51	,	52	,	53	,	54	,	55	,
	40	,	41	,	42	,	43	,	44	,	45	,	46	,	47	,
	32	,	33	,	34	,	35	,	36	,	37	,	38	,	39	,
	24	,	25	,	26	,	27	,	28	,	29	,	30	,	31	,
	16	,	17	,	18	,	19	,	20	,	21	,	22	,	23	,
	8	,	9	,	10	,	11	,	12	,	13	,	14	,	15	,
	0	,	1	,	2	,	3	,	4	,	5	,	6	,	7
};

extern const int* maps[7];
extern bitboard_t pawnIsolatedMask[64];
extern bitboard_t pawnPassedMask[2][64];
extern bitboard_t upperMask[64];
extern bitboard_t lowerMask[64];
extern bitboard_t pawnShield[2][64];
extern bitboard_t xMask[64];

const int passedBonus[2][8] = {
	{ 200, 100, 60, 35, 20, 10, 5, 0 },
	{ 0, 5, 10, 20, 35, 60, 100, 200 }
};

const int kingZoneTropism[9] = { 0, 40, 60, 77, 87, 92, 95, 97, 100 };

const int openFileBonusR = 10;
const int openFileBonusQ = 5;

#define MIRROR64(sq) (mirror64[(sq)])

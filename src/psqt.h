#pragma once

#include "types.h"

#define MIRROR64(sq) (mirror64[(sq)])

extern const value_t* PSQT_OPENING[7];
extern const value_t* PSQT_ENDGAME[7];

const value_t PAWN_OPENING[64] = {
	0,  0,  0,  0,  0,  0,  0,  0,
	5, 10, 10,-20,-20, 10, 10,  5,
	5, -5,-10,  0,  0,-10, -5,  5,
	0,  0,  0, 20, 20,  0,  0,  0,
	5,  5, 10, 25, 25, 10,  5,  5,
	10, 10, 20, 30, 30, 20, 10, 10,
	50, 50, 50, 50, 50, 50, 50, 50,
	0,  0,  0,  0,  0,  0,  0,  0,
};

const value_t PAWN_ENDGAME[64] = {
	  0,  0,  0,  0,  0,  0,  0,  0
	-20,-20,-20,-20,-20,-20,-20,-20,
	  0,  0,  0,  0,  0,  0,  0,  0,
	 10, 10, 10, 10, 10, 10, 10, 10,
	 15, 15, 15, 15, 15, 15, 15, 15,
	 30, 30, 30, 30, 30, 30, 30, 30,
	 80, 80, 80, 80, 80, 80, 80, 80,
	  0,  0,  0,  0,  0,  0,  0,  0,
};

const value_t KNIGHT_OPENING[64] = {
	-50,-40,-30,-30,-30,-30,-40,-50,
	-40,-20,  0,  5,  5,  0,-20,-40,
	-30,  5, 10, 15, 15, 10,  5,-30,
	-30,  0, 15, 20, 20, 15,  0,-30,
	-30,  5, 15, 20, 20, 15,  5,-30,
	-30,  0, 10, 15, 15, 10,  0,-30,
	-40,-20,  0,  0,  0,  0,-20,-40,
	-50,-40,-30,-30,-30,-30,-40,-50
};

const value_t KNIGHT_ENDGAME[64] = {
	-50,-40,-30,-30,-30,-30,-40,-50
	- 40,-20,  0,  5,  5,  0,-20,-40,
	-30,  5, 10, 15, 15, 10,  5,-30,
	-30,  0, 15, 20, 20, 15,  0,-30,
	-30,  5, 15, 20, 20, 15,  5,-30,
	-30,  0, 10, 15, 15, 10,  0,-30,
	-40,-20,  0,  0,  0,  0,-20,-40,
	-50,-40,-30,-30,-30,-30,-40,-50,
};

const value_t BISHOP_OPENING[64] = {
	-20,-10,-10,-10,-10,-10,-10,-20,
	-10,  5,  0,  0,  0,  0,  5,-10,
	-10, 10, 10, 10, 10, 10, 10,-10,
	-10,  0, 10, 10, 10, 10,  0,-10,
	-10,  5,  5, 10, 10,  5,  5,-10,
	-10,  0,  5, 10, 10,  5,  0,-10,
	-10,  0,  0,  0,  0,  0,  0,-10,
	-20,-10,-10,-10,-10,-10,-10,-20
};

const value_t BISHOP_ENDGAME[64] = {
	-20,-10,-10,-10,-10,-10,-10,-20
	- 10,  5,  0,  0,  0,  0,  5,-10,
	-10, 10, 10, 10, 10, 10, 10,-10,
	-10,  0, 10, 10, 10, 10,  0,-10,
	-10,  5,  5, 10, 10,  5,  5,-10,
	-10,  0,  5, 10, 10,  5,  0,-10,
	-10,  0,  0,  0,  0,  0,  0,-10,
	-20,-10,-10,-10,-10,-10,-10,-20,
};

const value_t ROOK_OPENING[64] = {
	0,  0, 20,  5,  5, 20,  0,  0,
	5,  0,  0,  0,  0,  0,  0,  5,
   -5,  0,  0,  0,  0,  0,  0, -5,
   -5,  0,  0,  0,  0,  0,  0, -5,
   -5,  0,  0,  0,  0,  0,  0, -5,
   -5,  0,  0,  0,  0,  0,  0, -5,
   -5,  0,  0,  0,  0,  0,  0, -5,
	0,  0,  0,  0,  0,  0,  0,  0
};

const value_t ROOK_ENDGAME[64] = {
	0,  0,  0,  0,  0,  0,  0,  0,
   -5,  0,  0,  0,  0,  0,  0, -5,
   -5,  0,  0,  0,  0,  0,  0, -5,
   -5,  0,  0,  0,  0,  0,  0, -5,
   -5,  0,  0,  0,  0,  0,  0, -5,
   -5,  0,  0,  0,  0,  0,  0, -5,
   -5,  0,  0,  0,  0,  0,  0, -5,
	0,  0,  0,  0,  0,  0,  0,  0
};

const value_t QUEEN_OPENING[64] = {
	-20,-10,-10, -5, -5,-10,-10,-20
	- 10,  0,  5,  0,  0,  0,  0,-10,
	-10,  5,  5,  5,  5,  5,  0,-10,
	  0,  0,  5,  5,  5,  5,  0, -5,
	 -5,  0,  5,  5,  5,  5,  0, -5,
	-10,  0,  5,  5,  5,  5,  0,-10,
	-10,  0,  0,  0,  0,  0,  0,-10,
	-20,-10,-10, -5, -5,-10,-10,-20,
};

const value_t QUEEN_ENDGAME[64] = {
	-20,-10,-10, -5, -5,-10,-10,-20
	- 10,  0,  5,  0,  0,  0,  0,-10,
	-10,  5,  5,  5,  5,  5,  0,-10,
	  0,  0,  5,  5,  5,  5,  0, -5,
	 -5,  0,  5,  5,  5,  5,  0, -5,
	-10,  0,  5,  5,  5,  5,  0,-10,
	-10,  0,  0,  0,  0,  0,  0,-10,
	-20,-10,-10, -5, -5,-10,-10,-20,
};

const value_t KING_OPENING[64] = {
	 30, 30, 40, 20, 20, 30, 40, 30,
	 20, 20,  0,  0,  0,  0, 20, 20,
	-10,-20,-20,-20,-20,-20,-20,-10,
	-20,-30,-30,-40,-40,-30,-30,-20,
	-30,-40,-40,-50,-50,-40,-40,-30,
	-30,-40,-40,-50,-50,-40,-40,-30,
	-30,-40,-40,-50,-50,-40,-40,-30,
	-30,-40,-40,-50,-50,-40,-40,-30
};

const value_t KING_ENDGAME[64] = {
	-50,-30,-30,-30,-30,-30,-30,-50
	-30,-30,  0,  0,  0,  0,-30,-30,
	-30,-10, 20, 30, 30, 20,-10,-30,
	-30,-10, 30, 40, 40, 30,-10,-30,
	-30,-10, 30, 40, 40, 30,-10,-30,
	-30,-10, 20, 30, 30, 20,-10,-30,
	-30,-20,-10,  0,  0,-10,-20,-30,
	-50,-40,-30,-20,-20,-30,-40,-50,
};

const unsigned int mirror64[64] = {
	56,	57,	58,	59,	60,	61, 62, 63,
	48,	49,	50,	51,	52,	53, 54, 55,
	40,	41,	42,	43,	44,	45, 46, 47,
	32,	33,	34,	35,	36,	37, 38, 39,
	24,	25,	26,	27,	28,	29, 30, 31,
	16,	17,	18,	19,	20,	21, 22, 23,
	8,	9 ,	10,	11,	12,	13, 14, 15,
	0,	1 ,	2 ,	3 ,	4 ,	5 , 6 , 7
};
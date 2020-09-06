#pragma once

#include "defs.h"

class BoardData
{
public:

	// index is true if the piece is knight / king / bishop / rook
	int pieceKnight[13] = { 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 };
	int pieceKing[13] = { 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1 };
	int pieceRookQueen[13] = { 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0 };
	int pieceBishopQueen[13] = { 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0 };

	// returns true if piece is BQ / RQ / N / K
	int isBQ(int p);
	int isRQ(int p);
	int isN(int p);
	int isK(int p);

	/* index true if piece is big / maj / min /wb and value */
	int pieceBig[13] = { 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1 };
	int pieceMaj[13] = { 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1 };
	int pieceMin[13] = { 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0 };
	int pieceVal[13] = { 0, 100, 325, 325, 550, 1000, 50000, 100, 325, 325, 550, 1000, 50000 };

	// contains directions for pieces for move generation and attacks
	int nDir[8] = { -8, -19, -21, -12, 8, 19, 21, 12 };
	int rDir[4] = { -1, -10, 1, 10 };
	int bDir[4] = { -9, -11, 11, 9 };
	int kDir[8] = { -1, -10, 1, 10, -9, -11, 11, 9 };

	/* variables to parse fen into board variables */
	string pieceChar = ".PNBRQKpnbrqk";
	string sideChar = "wb-";
	string rankChar = "12345678";
	string fileChar = "abcdefgh";

	U64 rand64();

	/* Address small and big board squares with each board type square as index */
	void init120To64();
	int sq120ToSq64[NUM_SQUARES];
	int sq64ToSq120[64];

	/* Address file and rank with given square as index */
	void initRankFileArrays();
	int fileBoard[NUM_SQUARES];
	int rankBoard[NUM_SQUARES];

	int file_rank_2_sq(int f, int r);

	int SQ_64(int sq120);
	int SQ_120(int sq64);

};


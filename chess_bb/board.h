#pragma once

#include "defs.h"

/*
- Board representation with 8 bitboards for each piece and each color
- Parse fen in boards
- Clear / Reset
- Check board function

- Castling rights
- En Passant squares
- Generate zobrist key

- Parse Algebraic notation
- Push Pop moves, move stack



*/

class Board
{
public:

	int side;
	int enPas = -1;
	int ply;
	U64 zobristKey;

	U64 color[2] = { 0ULL, 0ULL }; // { WHITE, BLACK }
	U64 pieces[7] = { EMPTY, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL };
	U64 occupied = 0ULL;


	const int bitTable[64] = {
	63, 30, 3, 32, 25, 41, 22, 33, 15, 50, 42, 13, 11, 53, 19, 34, 61, 29, 2,
	51, 21, 43, 45, 10, 18, 47, 1, 54, 9, 57, 0, 35, 62, 31, 40, 4, 49, 5, 52,
	26, 60, 6, 23, 44, 46, 27, 56, 16, 7, 39, 48, 24, 59, 14, 12, 55, 38, 28,
	58, 20, 37, 17, 36, 8
	};

	int countBits(U64* bb);
	int popBit(U64* bb);

	void clearBit(U64* bb, int square);
	void setBit(U64* bb, int square);

	void clearPiece(int piece, int square, int side);
	void setPiece(int piece, int square, int side);

	void init();
	void reset();

	void initClearSetMask();
	U64 setMask[64];
	U64 clearMask[64];

	U64 pieceKeys[13][64];
	U64 sideKey;
	U64 castleKeys[16];
	U64 generateZobristHash(Board* b);
	void initHashKeys();

	U64 getPiecesByColor(int piece, int side);
	int pieceAt(int square);

	void printAttackers(U64* bb);
	void printBitBoard(U64* bb);
	void printBoard();

	void parseFen(string fen);


};


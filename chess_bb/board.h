#pragma once

#include "defs.h"
#include "validate.h"
#include "undo.h"
#include <stack>
#include <bitset>


/*
- Move stack
- Pop move: undo class / struct, update zobrist keys (?)
- Check valid zobrist key
*/


class Board
{
public:

	int side;
	int enPas = -1;
	int ply = 0;
	int fiftyMove = 0;
	int castlePermission = 0;
	U64 zobristKey = 0x0;

	U64 color[2] = { 0ULL, 0ULL };
	U64 pieces[7] = { EMPTY, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL };
	U64 occupied = 0ULL;

	stack <Undo> undoStack;

	int squareToRank[64];
	int squareToFile[64];
	void initSquareToRankFile();

	int countBits(U64 bb);
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
	U64 generateZobristKey();
	void initHashKeys();

	U64 getPiecesByColor(int piece, int side);
	int pieceAt(int square);
	void clearCastlePermission(int side);

	void printBitBoard(U64* bb);
	void printBoard();
	void printMove(const int move);
	void printMoveStatus(int move);

	void parseFen(string fen);
	int parseMove(string move);

	int checkBoard();

	void push(int move);
	void pushCastle(int clearRookSq, int setRookSq, int side);

};


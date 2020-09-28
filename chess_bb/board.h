#pragma once

#include "defs.h"
#include "validate.h"
#include "undo.h"
#include "util.h"
#include "rays.h"
#include "mask.h"
#include "info.h"

#include <stack>
#include <bitset>
#include <iostream>

/*
* - Pawn Shield Pattern for king safety
* - Distance to king for safety
*/

class Board
{
public:

	int side;
	int enPas = 0;
	int ply = 0;
	int fiftyMove = 0;
	int castlePermission = 0;
	U64 zobristKey = 0x0;

	U64 color[2] = { 0ULL, 0ULL };
	U64 pieces[7] = { EMPTY, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL };
	U64 occupied = 0ULL;

	stack <Undo> undoStack;

	void clearPiece(int piece, int square, int side);
	void setPiece(int piece, int square, int side);

	void reset();

	void initHashKeys();
	U64 pieceKeys[13][64];
	U64 castleKeys[16];
	U64 sideKey;
	U64 generateZobristKey();

	U64 getPiecesByColor(int piece, int side);
	int pieceAt(int square);
	void clearCastlePermission(int side);

	void parseFen(string fen);
	int parseMove(string move);
	void printBoard();

	int checkBoard();

	// generate attacker mask for given side
	U64 attackerSet(int side);
	// check if attackerSet & kingSquare is true
	int isCheck();

	void push(int move);
	void pushCastle(int clearRookSq, int setRookSq, int side);
	Undo pop();
	void popCastle(int clearRookSq, int setRookSq, int side);

};


#pragma once

#include "stdlib.h"
#include "defs.h"
#include "UndoMove.h"
#include "BoardData.h"
#include "BitBoard.h"
#include "Validator.h"
#include <iostream>

class Board
{
private:
public:

	// store all 120 squares from big board
	int pieces[NUM_SQUARES];

	// current king squares for black and white
	int KingSquares[2];

	// pawn bitboards for black white both
	U64 pawns[3] = {0ULL, 0ULL, 0ULL};

	int side;
	int enPas;
	int fiftyMove;
	int castlePermission;

	int ply;
	int hisPly;

	// for each piece store current square (max 10 rooks possible)
	int pieceList[13][10]; 

	/* piece and material counters */
	int pieceNumber[13]; // store the amount of pieces on board (pieceNumber[whitepawn] = 8)
	int bigPieces[2]; // number of pieces excuding pawns
	int majPieces[2]; // number of rooks and queens
	int minPieces[2]; // number of knights and bishops
	int material[2] = { 0, 0 }; // material score for black and white

	UndoMove history[MAX_GAME_MOVES];

	U64 posKey;


	int pieceCol[13] = { BOTH, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK };

	void initHashKeys();
	U64 pieceKeys[13][120];
	U64 sideKey;
	U64 castleKeys[16];
	U64 generateZobristHash(Board* b);

	BoardData bd = BoardData();
	BitBoard bb = BitBoard();

	int* fileBoard = bd.fileBoard;
	int* rankBoard = bd.rankBoard;

	char* printMove(const int move);
	int sqOnBoard(int sq);
	
	void init();
	
	void resetBoard(Board* bb);
	void printBoard(const Board *board);
	int parseFen(char* fen, Board* board);

	int checkBoard(Board* board);
	int squareAttacked(const int sq, const int side, Board* board);
	void printAttackers(const int side, Board* b);
	void updateListsMaterial(Board* b);
};

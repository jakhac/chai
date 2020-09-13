#pragma once

#include "stdlib.h"
#include "Definition.h"
#include "UndoMove.h"
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

	U64 pieceKeys[13][120];
	U64 sideKey;
	U64 castleKeys[16];
	U64 generateZobristHash(Board* b);

	BitBoard bb = BitBoard();

	char* printMove(const int move);
	char* getSquareStr(const int sq);

	int sqOnBoard(int sq);

	/* Address small and big board squares with each board type square as index */
	int sq120ToSq64[NUM_SQUARES];
	int sq64ToSq120[64];

	/* Address file and rank with given square as index */
	int fileBoard[NUM_SQUARES];
	int rankBoard[NUM_SQUARES];
	
	void init();
	void initHashKeys();
	void initRankFileArrays();
	void init120To64();

	int SQ_120(int sq64);
	int SQ_64(int sq120);
	
	void resetBoard(Board* bb);
	void printBoard(const Board *board);
	void printBitBoard(U64 bb);
	int parseFen(char* fen, Board* board);

	int checkBoard(Board* board);
	int squareAttacked(const int sq, const int side, Board* board);
	void printAttackers(const int side, Board* b);
	void updateListsMaterial(Board* b);

	void clearPiece(Board* b, int sq);
	void addPiece(Board* b, const int sq, const int piece);
	void movePiece(Board* b, const int from, const int to);

	int push(Board* b, int move);
	void pop(Board* b);


};

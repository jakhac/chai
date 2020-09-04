#include "stdlib.h"
#include <bitset>
#include "defs.h"
#include "init_data.h"
#include <iostream>

class Board
{
public:

	int pieces[NUM_SQUARES];
	int KingSquares[2];
	U64 pawns[3];

	int side;
	int enPas;
	int fiftyMove;

	int ply;
	int hisPly;

	int castlePermission;

	U64 posKey;

	const int bitTable[64] = {
	  63, 30, 3, 32, 25, 41, 22, 33, 15, 50, 42, 13, 11, 53, 19, 34, 61, 29, 2,
	  51, 21, 43, 45, 10, 18, 47, 1, 54, 9, 57, 0, 35, 62, 31, 40, 4, 49, 5, 52,
	  26, 60, 6, 23, 44, 46, 27, 56, 16, 7, 39, 48, 24, 59, 14, 12, 55, 38, 28,
	  58, 20, 37, 17, 36, 8
	};

	U64 setMask[64];
	U64 clearMask[64];

	int pieceNumber[13];
	int bigPieces[3]; // pieces excuding pawns
	int majorPieces[3]; // rook and queen
	int minorPieces[3]; // knight and bishop

	UNDO_MOVE history[MAX_GAME_MOVES];

	int pieceList[13][10]; // keep track of possible moves for pieces

	int sq120ToSq64[NUM_SQUARES];
	int sq64ToSq120[64];

	// zobrist key
	U64 pieceKeys[13][120];
	U64 sideKey;
	U64 castleKeys[16];

	U64 rand64();
	void initHashKeys();
	U64 generateZobristHash(Board *b);

	void init();
	void init120To64();
	void printBitBoard(U64 bb);

	// Reset all variables of referenced board
	void resetBoard(Board *bb);

	// Init starting fen position on referenced board
	int parseFen(char *fen, Board *board);

	void printBoard(const Board *board);

	// Find 120-based index for given file and rank
	int file_rank_2_sq(int f, int r);

	// method to address 64 indexed array with A1 ... H8 constants
	int squareConstantsTo64(int square);

	// count bits on bitboard (use intern popcount?)
	int countBits(U64 bb);

	// pop least significant bit and return index
	int popBit(U64 *bb);

	// init set and clear bitmask for pawns
	void initClearSetMask();

	// clear bit from (pawn) bitboard
	void clearBit(U64 bb, int square);

	// set bit from (pawn) bitboard
	void setBit(U64 bb, int square);

};


#define SQ64(sq120) (sq120ToSq64[(sq120)])
#define SQ120(sq64) (sq64ToSq120[(sq64)])
#pragma once
#include "Definition.h"

#ifndef U64
typedef unsigned long long U64;
#endif // !U64


class BitBoard
{
private:
	
	// 6 boards: pieces, 2 boards: colors
	U64 allPiecesBB[8];

public:

	// Empty bitboard.
	const U64 EMPTY_BB[64] = { 0 };
	

	// Default white positions for game start. 
	const U64 DEFAULT_WHITE[64] = { 1, 1, 1, 1, 1, 1, 1, 1,
									1, 1, 1, 1, 1, 1, 1, 1,
									0, 0, 0, 0, 0, 0, 0, 0,
									0, 0, 0, 0, 0, 0, 0, 0,
									0, 0, 0, 0, 0, 0, 0, 0,
									0, 0, 0, 0, 0, 0, 0, 0,
									0, 0, 0, 0, 0, 0, 0, 0,
									0, 0, 0, 0, 0, 0, 0, 0 };

	// Default black positions for game start. 
	const U64 DEFAULT_BLACK[64] = { 0, 0, 0, 0, 0, 0, 0, 0,
									0, 0, 0, 0, 0, 0, 0, 0,
									0, 0, 0, 0, 0, 0, 0, 0,
									0, 0, 0, 0, 0, 0, 0, 0,
									0, 0, 0, 0, 0, 0, 0, 0,
									0, 0, 0, 0, 0, 0, 0, 0,
									1, 1, 1, 1, 1, 1, 1, 1,
									1, 1, 1, 1, 1, 1, 1, 1 };

	// Default pawn positions for game start. 
	const U64 DEFAULT_PAWNS[64] = { 0, 0, 0, 0, 0, 0, 0, 0,
									1, 1, 1, 1, 1, 1, 1, 1,
									0, 0, 0, 0, 0, 0, 0, 0, 
									0, 0, 0, 0, 0, 0, 0, 0, 
									0, 0, 0, 0, 0, 0, 0, 0, 
									0, 0, 0, 0, 0, 0, 0, 0, 
									1, 1, 1, 1, 1, 1, 1, 1,
									0, 0, 0, 0, 0, 0, 0, 0 };

	// Default knight positions for game start. 
	const U64 DEFAULT_KNIGHT[64] = { 0, 1, 0, 0, 0, 0, 1, 0,
									 0, 0, 0, 0, 0, 0, 0, 0,
									 0, 0, 0, 0, 0, 0, 0, 0,
									 0, 0, 0, 0, 0, 0, 0, 0,
									 0, 0, 0, 0, 0, 0, 0, 0,
								 	 0, 0, 0, 0, 0, 0, 0, 0,
									 0, 0, 0, 0, 0, 0, 0, 0,
									 0, 1, 0, 0, 0, 0, 1, 0 };

	// Default bishop positions for game start. 
	const U64 DEFAULT_BISHOP[64] = { 0, 0, 1, 0, 0, 1, 0, 0,
									 0, 0, 0, 0, 0, 0, 0, 0,
									 0, 0, 0, 0, 0, 0, 0, 0,
									 0, 0, 0, 0, 0, 0, 0, 0,
									 0, 0, 0, 0, 0, 0, 0, 0,
									 0, 0, 0, 0, 0, 0, 0, 0,
									 0, 0, 0, 0, 0, 0, 0, 0,
									 0, 0, 1, 0, 0, 1, 0, 0 };

	// Default rook positions for game start. 
	const U64 DEFAULT_ROOK[64] = { 1, 0, 0, 0, 0, 0, 0, 1,
								   0, 0, 0, 0, 0, 0, 0, 0,
								   0, 0, 0, 0, 0, 0, 0, 0,
								   0, 0, 0, 0, 0, 0, 0, 0,
								   0, 0, 0, 0, 0, 0, 0, 0,
								   0, 0, 0, 0, 0, 0, 0, 0,
								   0, 0, 0, 0, 0, 0, 0, 0,
								   1, 0, 0, 0, 0, 0, 0, 1 };

	// Default queen positions for game start. 
	const U64 DEFAULT_QUEEN[64] = { 0, 0, 0, 0, 1, 0, 0, 0,
									0, 0, 0, 0, 0, 0, 0, 0,
									0, 0, 0, 0, 0, 0, 0, 0,
									0, 0, 0, 0, 0, 0, 0, 0,
									0, 0, 0, 0, 0, 0, 0, 0,
									0, 0, 0, 0, 0, 0, 0, 0,
									0, 0, 0, 0, 0, 0, 0, 0,
									0, 0, 0, 0, 1, 0, 0, 0 };

	// Default king positions for game start. 
	const U64 DEFAULT_KING[64] =  { 0, 0, 0, 1, 0, 0, 0, 0,
									0, 0, 0, 0, 0, 0, 0, 0,
									0, 0, 0, 0, 0, 0, 0, 0,
									0, 0, 0, 0, 0, 0, 0, 0,
									0, 0, 0, 0, 0, 0, 0, 0,
									0, 0, 0, 0, 0, 0, 0, 0,
									0, 0, 0, 0, 0, 0, 0, 0,
									0, 0, 0, 1, 0, 0, 0, 0 };


	enum N_PIECES
	{
		N_WHITE, //any white piece
		N_BLACK, 
		N_PAWN, // any pawn
		N_KNIGHT,
		N_BISHOP,
		N_ROOK,
		N_QUEEN,
		N_KING
	};

	/// <summary>
	/// Generates a bitboard for a specific piece type, e.g. "black pawns".
	/// </summary>
	/// <param name="p">Piece type according to PIECE_VALUES enum.</param>
	/// <returns>U64 Bitboard of specified piece type.</returns>
	U64 getPieceSet(PIECE_VALUES p);


	int countBits(U64* bb);
	int popBit(U64* bb);

	void clearBit(U64* bb, int square);
	void setBit(U64* bb, int square);

	void initClearSetMask();
	U64 setMask[64];
	U64 clearMask[64];

	const int bitTable[64] = {
		63, 30, 3, 32, 25, 41, 22, 33, 15, 50, 42, 13, 11, 53, 19, 34, 61, 29, 2,
		51, 21, 43, 45, 10, 18, 47, 1, 54, 9, 57, 0, 35, 62, 31, 40, 4, 49, 5, 52,
		26, 60, 6, 23, 44, 46, 27, 56, 16, 7, 39, 48, 24, 59, 14, 12, 55, 38, 28,
		58, 20, 37, 17, 36, 8
	};

};


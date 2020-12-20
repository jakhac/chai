#include "defs.h"

const int isBQ(int p) {
	return pieceBishopQueen[p];
}

const int isRQ(int p) {
	return pieceRookQueen[p];
}

const int isN(int p) {
	return pieceKnight[p];
}

const int isK(int p) {
	return pieceKing[p];
}

/*
Converts a file and a rank into the corresponding square.
*/
const int file_rank_2_sq(int f, int r) {
	return 8 * r + f;
}

/*
Generate a random 64bit integer for zobrist hashing
*/
const bitboard_t rand64() {
	return (bitboard_t)rand() |
		((bitboard_t)rand() << 15) |
		((bitboard_t)rand() << 30) |
		((bitboard_t)rand() << 45) |
		(((bitboard_t)rand() & 0xf) << 60);
}

const bitboard_t randomFewBits() {
	return rand64() & rand64() & rand64();
}


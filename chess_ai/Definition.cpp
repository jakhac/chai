#include "Definition.h"

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
	return (21 + f) + (r * 10);
}

/*
Generate a random 64bit integer for zobrist hashing
*/
const U64 rand64() {
	return (U64)rand() |
		((U64)rand() << 15) |
		((U64)rand() << 30) |
		((U64)rand() << 45) |
		(((U64)rand() & 0xf) << 60);
}


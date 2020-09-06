#include "BoardData.h"


int BoardData::isBQ(int p) {
	return pieceBishopQueen[p];
}

int BoardData::isRQ(int p) {
	return pieceRookQueen[p];
}

int BoardData::isN(int p) {
	return pieceKnight[p];
}

int BoardData::isK(int p) {
	return pieceKing[p];
}

/*
Converts a file and a rank into the corresponding square.
*/
int BoardData::file_rank_2_sq(int f, int r) {
	return (21 + f) + (r * 10);
}

/*
Generate a random 64bit integer for zobrist hashing
*/
U64 BoardData::rand64() {
	return (U64)rand() |
		((U64)rand() << 15) |
		((U64)rand() << 30) |
		((U64)rand() << 45) |
		(((U64)rand() & 0xf) << 60);
}

/*
Init 120 to 63 indexed arrays
*/
void BoardData::init120To64() {
	int index = 0;
	int file = FILE_A;
	int rank = RANK_1;
	int sq = A1;
	int squareCounter = 0;

	// Set both arrays non-legal values to prevent errors
	for (index = 0; index < NUM_SQUARES; index++) {
		sq120ToSq64[index] = 65;
	}

	for (rank = RANK_1; rank <= RANK_8; rank++) {
		sq64ToSq120[rank] = 120;
	}

	// Fill arrays with indices
	for (rank = RANK_1; rank <= RANK_8; ++rank) {
		for (file = FILE_A; file <= FILE_H; ++file) {
			sq = file_rank_2_sq(file, rank);

			// TODO use methods
			sq120ToSq64[sq] = squareCounter;
			sq64ToSq120[squareCounter] = sq;

			squareCounter++;
		}
	}
}

/*
Initialize rank and file arrays.
*/
void BoardData::initRankFileArrays() {
	int square = A1;

	for (int i = 0; i < NUM_SQUARES; i++) {
		fileBoard[i] = OFFBOARD;
		rankBoard[i] = OFFBOARD;
	}

	for (int rank = RANK_1; rank <= RANK_8; rank++) {
		for (int file = FILE_A; file <= FILE_H; file++) {
			square = file_rank_2_sq(file, rank);
			fileBoard[square] = file;
			rankBoard[square] = rank;
		}
	}
}

/*
Conert 120 based into 64 based index.
*/
int BoardData::SQ_64(int sq120) {
	return sq120ToSq64[sq120];
}

/*
Conert 64 based into 120 based index.
*/
int BoardData::SQ_120(int sq64) {
	return sq64ToSq120[sq64];
}

#include "mask.h"

// clear set bit masks
U64 setMask[64];
U64 clearMask[64];

// convert sq to rank or file
int squareToRank[64];
int squareToFile[64];

// attacker mask for pieces
U64 pawnAtkMask[2][64];
U64 knightAtkMask[64];
U64 kingAtkMask[64];

// rook masks
U64 rays[8][64];
U64 rookMasks[64];

/// <summary>
/// Generate rays for rays array for each direction on each square.
/// </summary>
void initRays() {
	for (int sq = 0; sq < 64; sq++) {
		rays[NORTH][sq] = 0x0101010101010100ULL << sq;
		rays[SOUTH][sq] = 0x0080808080808080ULL >> (63 - sq);
		rays[EAST][sq] = 2 * ((1ULL << (sq | 7)) - (1ULL << sq));
		rays[WEST][sq] = (1ULL << sq) - (1ULL << (sq & 56));
		rays[NORTH_WEST][sq] = _westN(0x102040810204000ULL, 7 - _col(sq)) << (_row(sq) * 8);
		rays[NORTH_EAST][sq] = _eastN(0x8040201008040200ULL, _col(sq)) << (_row(sq) * 8);
		rays[SOUTH_WEST][sq] = _westN(0x40201008040201ULL, 7 - _col(sq)) >> ((7 - _row(sq)) * 8);
		rays[SOUTH_EAST][sq] = _eastN(0x2040810204080ULL, _col(sq)) >> ((7 - _row(sq)) * 8);
	}
}

/// <summary>
/// Get rays in given direction of given square.
/// </summary>
/// <param name="dir">Direction (use enum Dir)</param>
/// <param name="sq">Starting square, is not included  in return</param>
/// <returns>Bitboard with set bits in given direction</returns>
U64 getRay(int dir, int sq) {
	return rays[dir][sq];
}

void initRookMasks() {
	for (int i = 0; i < 64; i++) {
		rookMasks[i] = (getRay(NORTH, i) & ~RANK_8_HEX) |
			(getRay(SOUTH, i) & ~RANK_1_HEX) |
			(getRay(EAST, i) & ~FILE_H_HEX) |
			(getRay(WEST, i) & ~FILE_A_HEX);
	}
}

/// <summary>
/// Initialize clear and set mask arrays for usage.
/// </summary>
void initClearSetMask() {
	int index = 0;

	// Zero both arrays
	for (index = 0; index < 64; index++) {
		setMask[index] = 0ULL << index;
		clearMask[index] = 0ULL << index;
	}

	for (index = 0; index < 64; index++) {
		setMask[index] |= (1ULL << index);
		clearMask[index] = ~setMask[index];
	}
}

/// <summary>
/// Initialize arrays to index square to files and ranks.
/// </summary>
void initSquareToRankFile() {
	int sq;
	for (int rank = RANK_8; rank >= RANK_1; rank--) {
		for (int file = FILE_A; file <= FILE_H; file++) {
			sq = file_rank_2_sq(file, rank);
			squareToFile[sq] = file;
			squareToRank[sq] = rank;
		}
	}
}

/// <summary>
/// Initialize attacker mask for all pieces.
/// </summary>
void initAttackerMasks() {
	int offSet = 0;

	// pawns
	for (int i = 8; i < 56; i++) {
		pawnAtkMask[WHITE][i] = (setMask[i] << 7 & ~FILE_H_HEX) | (setMask[i] << 9 & ~FILE_A_HEX);
	}
	for (int i = 55; i > 7; i--) {
		pawnAtkMask[BLACK][i] = (setMask[i] >> 7 & ~FILE_A_HEX) | (setMask[i] >> 9 & ~FILE_H_HEX);
	}

	// knights
	for (int i = 0; i < 64; i++) {
		knightAtkMask[i] |= (setMask[i] << 17) & ~FILE_A_HEX;
		knightAtkMask[i] |= (setMask[i] << 10) & ~(FILE_A_HEX | FILE_B_HEX);
		knightAtkMask[i] |= (setMask[i] >> 6) & ~(FILE_A_HEX | FILE_B_HEX);
		knightAtkMask[i] |= (setMask[i] >> 15) & ~FILE_A_HEX;
		knightAtkMask[i] |= (setMask[i] << 15) & ~FILE_H_HEX;
		knightAtkMask[i] |= (setMask[i] << 6) & ~(FILE_G_HEX | FILE_H_HEX);
		knightAtkMask[i] |= (setMask[i] >> 10) & ~(FILE_G_HEX | FILE_H_HEX);
		knightAtkMask[i] |= (setMask[i] >> 17) & ~FILE_H_HEX;
	}

	// kings
	for (int i = 0; i < 64; i++) {
		U64 kingSet = setMask[i];
		U64 attacks = (kingSet << 1 & ~FILE_A_HEX) | (kingSet >> 1 & ~FILE_H_HEX);
		kingSet |= attacks;
		attacks |= kingSet << 8 | kingSet >> 8;
		kingAtkMask[i] = attacks;
	}
}
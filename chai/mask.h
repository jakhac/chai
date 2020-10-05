#pragma once

#include "defs.h"

#ifndef MASK_H
#define MASK_H

// clear set bit masks
extern U64 setMask[64];
extern U64 clearMask[64];

// convert sq to rank or file
extern int squareToRank[64];
extern int squareToFile[64];

// attacker mask for pieces
extern U64 pawnAtkMask[2][64];
extern U64 knightAtkMask[64];
extern U64 kingAtkMask[64];

/// <summary>
/// Initialize clear and set mask arrays for usage.
/// </summary>
inline void initClearSetMask() {
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
inline void initSquareToRankFile() {
	int sq;
	for (int rank = RANK_8; rank >= RANK_1; rank--) {
		for (int file = FILE_A; file <= FILE_H; file++) {
			//sq = file_rank_2_sq(file, rank);
			sq = 8 * rank + file;
			squareToFile[sq] = file;
			squareToRank[sq] = rank;
		}
	}
}

/// <summary>
/// Initialize attacker mask for all pieces.
/// </summary>
inline void initAttackerMasks() {
	int offSet = 0;

	// pawns
	for (int i = 0; i < 64; i++) {
		pawnAtkMask[WHITE][i] = (setMask[i] << 7 & ~FILE_H_HEX) | (setMask[i] << 9 & ~FILE_A_HEX);
	}
	for (int i = 64; i > 0; i--) {
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

#endif // !MASK_H
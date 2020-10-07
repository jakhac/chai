#pragma once

#include <vector>

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

// helper arrays to generate obstructed method
extern U64 dirBitmap[64][8];
extern U64 inBetween[64][64];
extern int dirFromTo[64][64];
extern U64 lineBB[64][64];

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

/// <summary>
/// Initialize obstructed array. All bits set between start and end square.
/// </summary>
inline void initObstructed() {
	const int kingd[8] = { -9, -1, 7, 8, 9, 1, -7, -8 };

	for (int sq = 0; sq < 64; ++sq) {
		for (int sq2 = 0; sq2 < 64; ++sq2) dirFromTo[sq][sq2] = 8;
		for (int k = 0; k < 8; ++k) {
			dirBitmap[sq][k] = 0;
			for (int p = sq, n = sq + kingd[k]; n >= 0 && n < 64 && abs((n & 7) - (p & 7)) < 2; p = n, n += kingd[k]) {
				dirFromTo[sq][n] = k;
				dirBitmap[sq][k] |= setMask[n];
			}
		}
	}
	for (int sq = 0; sq < 64; ++sq) {
		for (int sq2 = 0; sq2 < 64; ++sq2) {
			inBetween[sq][sq2] = U64(0);
			int k = dirFromTo[sq][sq2];
			if (k != 8) {
				int k2 = dirFromTo[sq2][sq];
				inBetween[sq][sq2] = dirBitmap[sq][k] & dirBitmap[sq2][k2];
			}
		}
	}
}

inline vector<U64> initLine() {

	U64 midDiagonalUp = 0x8040201008040201;
	U64 midDiagonalDown = 0x0102040810204080;
	U64 vertical = FILE_A_HEX;
	U64 horizontal = RANK_1_HEX;
	vector<U64> axis = { midDiagonalDown, midDiagonalUp, vertical, horizontal, 
		FILE_B_HEX, FILE_C_HEX, FILE_D_HEX, FILE_E_HEX, 
		FILE_F_HEX, FILE_G_HEX, FILE_H_HEX };

	for (int i = 1; i < 8; i++) {
		axis.push_back(midDiagonalDown << (i * 8));
		axis.push_back(midDiagonalDown >> (i * 8));

		axis.push_back(midDiagonalUp << (i * 8));
		axis.push_back(midDiagonalUp >> (i * 8));

		axis.push_back(horizontal << (i * 8));
	}

	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 64; j++) {
			lineBB[i][j] = 0;
		}
	}

	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 64; j++) {
			if ((i == j) || lineBB[i][j] != 0) continue;
			for (auto d : axis) {
				if ((setMask[i] & d) && (setMask[j] & d)) {
					lineBB[i][j] = d;
					lineBB[j][i] = d;
				}
			}
		}
	}

	return axis;
}

inline U64 line_bb(int s1, int s2) {
	return U64();
	//return LineBB[s1][s2];
}

inline bool aligned(int s1, int s2, int s3) {
	return line_bb(s1, s2) & s3;
}



/// <summary>
/// Returns bitboard with all bits sets between sq1 and sq2
/// </summary>
/// <param name="sq1">Square 1</param>
/// <param name="sq2">Square 2</param>
/// <returns></returns>
inline U64 obstructed(int sq1, int sq2) {
	return inBetween[sq1][sq2];
}

#endif // !MASK_H
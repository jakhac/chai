#pragma once

#include "defs.h"
#include "mask.h"

#ifndef UTIL_H

#define UTIL_H

/**
 * @brief Moves all set bits in the given bitboard n squares west and returns the new
 * bitboard, discarding those that fall off the edge.
 *
 * @param board Board to move bits west on
 * @param n Number of squares to move west
 * @return A bitboard with all set bits moved one square west, bits falling off the edge discarded
 */
inline U64 _westN(U64 board, int n) {
	U64 newBoard = board;
	for (int i = 0; i < n; i++) {
		newBoard = ((newBoard >> 1) & (~FILE_H));
	}

	return newBoard;
}

/**
* @brief Moves all set bits in the given bitboard n squares east and returns
* the new bitboard, discarding those that fall off the edge.
*
* @param board Board to move bits east on
* @param n Number of squares to move east
* @return A bitboard with all set bits moved one square east, bits falling off the edge discarded
*/
inline U64 _eastN(U64 board, int n) {
	U64 newBoard = board;
	for (int i = 0; i < n; i++) {
		newBoard = ((newBoard << 1) & (~FILE_A));
	}

	return newBoard;
}

/**
 * @brief Returns the zero indexed row of the given square
 *
 * @param square A square in little endian rank file mapping form
 * @return The zero indexed row of the square
 */
inline int _row(int square) {
	return square / 8;
}

/**
 * @brief Returns the zero indexed column of the given square.
 *
 * @param square A square in little endian rank file mapping form
 * @return The zero indexed row of the squares
 */
inline int _col(int square) {
	return square % 8;
}

/// <summary>
/// Counts bits of given ULL integer.
/// </summary>
inline int countBits(U64 bb) {
	int cnt = 0;
	while (bb) {
		cnt += bb & 1;
		bb >>= 1;
	}
	return cnt;
}

/// <summary>
/// Pops least significant bit and returns index.
/// </summary>
inline int popBit(U64* bb) {
	U64 b = *bb ^ (*bb - 1);
	unsigned int fold = (unsigned)((b & 0xffffffff) ^ (b >> 32));
	*bb &= (*bb - 1);
	return bitTable[(fold * 0x783a9b23) >> 26];
}

/// <summary>
/// Set bit at given bitboard.
/// </summary>
inline void setBit(U64* bb, int i) {
	*bb |= setMask[i];
}

/// <summary>
/// Clear bit at given bitboard.
/// </summary>
inline void clearBit(U64* bb, int i) {
	*bb &= clearMask[i];
}

#endif // !UTIL_H

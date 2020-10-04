#pragma once

#include "defs.h"
#include "mask.h"
#include <iostream>

#ifndef UTIL_H
#define UTIL_H

/// <summary>
/// Moves all set bits in the given bitboard n squares west and returns the new
/// bitboard, discarding those that fall off the edge.
/// </summary>
/// <param name="board">Board to move bits west on</param>
/// <param name="n">Number of squares to move west</param>
/// <returns>New bitboard with shifted bits to west</returns>
inline U64 westN(U64 board, int n) {
	U64 newBoard = board;
	for (int i = 0; i < n; i++) {
		newBoard = ((newBoard >> 1) & (~FILE_H));
	}
	return newBoard;
}

/// <summary>
/// Moves all set bits in the given bitboard n squares east and returns the new
/// bitboard, discarding those that fall off the edge.
/// </summary>
/// <param name="board">Board to move bits east on</param>
/// <param name="n">Number of squares to move east</param>
/// <returns>New bitboard with shifted bits to east</returns>
inline U64 eastN(U64 board, int n) {
	U64 newBoard = board;
	for (int i = 0; i < n; i++) {
		newBoard = ((newBoard << 1) & (~FILE_A));
	}
	return newBoard;
}

/// <summary>
/// Return zero indexed row of given square.
/// </summary>
/// <param name="square">Square to check row on</param>
/// <returns>Zero indexed row</returns>
inline int row(int square) {
	return square / 8;
}

/// <summary>
/// Return zero indexed column of given square.
/// </summary>
/// <param name="square">Square to check column on</param>
/// <returns>Zero indexed column</returns>
inline int col(int square) {
	return square % 8;
}

/// <summary>
/// Bit scan forward and return index. If Board is 0 return -1. Uses compiler bitscan.
/// </summary>
/// <param name="board">Board</param>
/// <returns>INdex of first found bit</returns>
inline int bitscanForward(U64 board) {
	if (board == 0ULL) return -1;

	unsigned long ret;
	_BitScanForward64(&ret, board);
	return (int)ret;
}

/// <summary>
/// Reversed bit scan forward amd return index. If Board is 0 return -1.
/// </summary>
/// <param name="board">Board</param>
/// <returns>Index of first found bit</returns>
inline int bitscanReverse(U64 board) {
	if (board == 0ULL) return -1;

	unsigned long ret;
	_BitScanReverse64(&ret, board);
	return (int)ret;
}

/// <summary>
/// Counts bits of given ULL integer.
/// </summary>
/// <param name="bb">Bitboard to count bits on</param>
/// <returns>Amount of bits set to 1 in bb</returns>
inline int countBits(U64 bb) {
	int cnt = 0;
	while (bb) {
		cnt += bb & 1;
		bb >>= 1;
	}
	return cnt;
}

/// <summary>
/// Pops least significant bit from bitboard and returns index.
/// </summary>
/// <param name="bb">Bitboard to pop lsb on</param>
/// <returns>Index of popped bit</returns>
inline int popBit(U64* bb) {
	U64 b = *bb ^ (*bb - 1);
	unsigned int fold = (unsigned)((b & 0xffffffff) ^ (b >> 32));
	*bb &= (*bb - 1);
	return bitTable[(fold * 0x783a9b23) >> 26];
}

/// <summary>
/// Set bit at given bitboard.
/// </summary>
/// <param name="bb">Bitboard to set bit on</param>
/// <param name="i">Index of bit that is set to 1</param>
inline void setBit(U64* bb, int i) {
	*bb |= setMask[i];
}

/// <summary>
/// Clear bit at given bitboard.
/// </summary>
/// <param name="bb">Bitboard to clear bit on</param>
/// <param name="i">Index of bit that is set to 0</param>
inline void clearBit(U64* bb, int i) {
	*bb &= clearMask[i];
}

#endif // !UTIL_H

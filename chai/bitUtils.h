#pragma once

#include <intrin.h>
#include <iostream>
#include "windows.h"

#include "defs.h"
#include "mask.h"

/**
 * Bit scan forward and return index. If board_t is 0 return -1. Uses compiler bitscan.
 *
 * @param  board board_t.
 *
 * @returns Index of first found bit.
 */
inline int bitscanForward(bitboard_t board) {
	if (board == 0ULL) return -1;

	unsigned long ret;
	_BitScanForward64(&ret, board);
	return (int)ret;
}

/**
 * Reversed bit scan forward and return index. If board_t is 0 return -1.
 *
 * @param  board board_t.
 *
 * @returns Index of first found bit.
 */
inline int bitscanReverse(bitboard_t board) {
	if (board == 0ULL) return -1;

	unsigned long ret;
	_BitScanReverse64(&ret, board);
	return (int)ret;
}

/**
 * Counts bits of given ULL integer.
 *
 * @param  bb Bitboard to count bits on.
 *
 * @returns Amount of bits set to 1 in bb.
 */
inline int countBits(bitboard_t bb) {
	return (int)__popcnt64(bb);
}

/**
 * Pops least significant bit from bitboard and returns index.
 *
 * @param  bb Bitboard to pop lsb on.
 *
 * @returns Index of popped bit.
 */
inline int popBit(bitboard_t* bb) {
	unsigned long ret;
	_BitScanForward64(&ret, *bb);
	*bb &= (*bb - 1);
	return (int)ret;
}

/**
 * Pops least significant bit from bitboard and returns index.
 *
 * @param  bb Bitboard to pop lsb on.
 *
 * @returns Index of popped bit.
 */
inline int popBit(bitboard_t bb) {
	unsigned long ret;
	_BitScanForward64(&ret, bb);
	bb &= (bb - 1);
	return (int)ret;
}

/**
 * Set bit at given bitboard.
 *
 * @param  bb Bitboard to set bit on.
 * @param  i  Index of bit that is set to 1.
 */
inline void setBit(bitboard_t* bb, int i) {
	*bb |= setMask[i];
}

/**
 * Clear bit at given bitboard.
 *
 * @param  bb Bitboard to clear bit on.
 * @param  i  Index of bit that is set to 0.
 */
inline void clearBit(bitboard_t* bb, int i) {
	*bb &= clearMask[i];
}

/**
 * Gets current time milliseconds.
 *
 * @returns The time in milliseconds.
 */
inline int getTimeMs() {
	return (int)GetTickCount64();
}

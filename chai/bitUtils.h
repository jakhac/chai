#pragma once

#if defined(_MSC_VER)
/* Microsoft C/C++-compatible compiler */
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
/* GCC-compatible compiler, targeting x86/x86-64 */
#include <x86intrin.h>
#endif

#include "windows.h" // used for getTimeMs()

#include "types.h"

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
#if defined(_MSC_VER)
	return (int)__popcnt64(bb);
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
	return (int)_popcnt64(bb);
#endif
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
	*bb |= (1ULL << i);
}

/**
 * Clear bit at given bitboard.
 *
 * @param  bb Bitboard to clear bit on.
 * @param  i  Index of bit that is set to 0.
 */
inline void clearBit(bitboard_t* bb, int i) {
	*bb &= ~(1ULL << i);
}

/**
 * Gets current time milliseconds.
 *
 * @returns The time in milliseconds.
 */
inline int getTimeMs() {
#ifdef __GNUG__
	return (int)GetTickCount();
#else
	return (int)GetTickCount64();
#endif
}

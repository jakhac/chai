#pragma once

#include "windows.h" // used for getTimeMs()

#include "types.h"

#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <x86intrin.h>
#endif

/**
 * Bit scan forward and return index. If board_t is 0 return -1. Uses compiler bitscan.
 *
 * @param  board board_t.
 *
 * @returns Index of first found bit.
 */
inline int getLSB(bitboard_t board) {
	if (board == 0ULL) return -1;

	unsigned long ret;
	_BitScanForward64(&ret, board);
	return (int)ret;
}

/**
 * Reversed bit scan reverse and return index. If board_t is 0 return -1.
 *
 * @param  board board_t.
 *
 * @returns Index of first found bit.
 */
inline int getMSB(bitboard_t board) {
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
inline int popCount(bitboard_t bb) {
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
inline int getPopLSB(bitboard_t* bb) {
	unsigned long ret;
	_BitScanForward64(&ret, *bb);
	*bb &= (*bb - 1);
	return (int)ret;
}

// Pop lsb and return modified board
inline bitboard_t popLSB(bitboard_t bb) {
	if (!bb) return 0;

	return (bb & (bb - 1));
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

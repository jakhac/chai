#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif // !NOMINMAX
#include "windows.h" // used for getTimeMs()

#include "defs.h"

#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <x86intrin.h>
#endif

/**
 * Bit scan forward and return index. If Board is 0 return 0. Uses compiler bitscan.
 *
 * @param  board Board.
 *
 * @returns Index of first found bit.
 */
inline int getLSB(Bitboard bb) {
    Assert(bb);

#if defined(_MSC_VER)
    unsigned long ret;
    _BitScanForward64(&ret, bb);
    return unsigned(ret);
#else
    return __builtin_ctzll(bb);
#endif
}

inline int getLSB_tb(Bitboard bb) {
    Assert(bb);

#if defined(_MSC_VER)
    unsigned long ret;
    _BitScanForward64(&ret, bb);
    return unsigned(ret);
#else
    return __builtin_ctzll(bb);
#endif
}

/**
 * Reversed bit scan reverse and return index. If Board is 0 return 0.
 *
 * @param  board Board.
 *
 * @returns Index of first found bit.
 */
inline int getMSB(Bitboard bb) {
    Assert(bb);

#if defined(_MSC_VER)
    unsigned long ret;
    _BitScanReverse64(&ret, bb);
    return (unsigned)ret;
#else
    return __builtin_clzll(bb) ^ 63;
#endif
}

/**
 * Counts bits of given ULL integer.
 *
 * @param  bb Bitboard to count bits on.
 *
 * @returns Amount of bits set to 1 in bb.
 */
inline int popCount(Bitboard bb) {
#if defined(_MSC_VER)
    return (int)__popcnt64(bb);
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
    return (int)_popcnt64(bb);
#endif
}

/**
 * Pops least significant bit from bitboard and returns index.
 *
 * @param bb Bitboard to pop lsb on.
 *
 * @returns Index of popped bit.
 */
inline int popLSB(Bitboard* bb) {
    Assert(*bb);
    int lsb = getLSB(*bb);
    *bb &= (*bb - 1);
    return lsb;
}

inline int popLSB_tb(uint64_t* bb) {
    Assert(*bb);
    int lsb = getLSB_tb(*bb);
    *bb &= (*bb - 1);
    return lsb;
}

inline int popMSB(uint64_t* bb) {
    int msb = getMSB(*bb);
    *bb ^= 1ull << msb;
    return msb;
}

inline bool bitIsSet(uint64_t bb, int i) {
    return bb & (1ull << i);
}

/**
 * Set bit at given bitboard.
 *
 * @param  bb Bitboard to set bit on.
 * @param  i  Index of bit that is set to 1.
 */
inline void setBit(Bitboard* bb, int i) {
    Assert(bitIsSet(*bb, i));
    *bb ^= (1ULL << i);
}

/**
 * Clear bit at given bitboard.
 *
 * @param  bb Bitboard to clear bit on.
 * @param  i  Index of bit that is set to 0.
 */
inline void clearBit(Bitboard* bb, int i) {
    Assert(bitIsSet(*bb, i));
    *bb ^= 1ULL << i;
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

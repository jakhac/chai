#pragma once

#include "defs.h"
#include "mask.h"
#include <intrin.h>
#include <iostream>
#include "io.h"

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

#include "windows.h"
inline int getTimeMs() {
	return (int) GetTickCount64();
}

inline bool inputWaiting() {
	static int init = 0, pipe;
	static HANDLE inh;
	DWORD dw;

	if (!init) {
		init = 1;
		inh = GetStdHandle(STD_INPUT_HANDLE);
		pipe = !GetConsoleMode(inh, &dw);
		if (!pipe) {
			SetConsoleMode(inh, dw & ~(ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT));
			FlushConsoleInputBuffer(inh);
		}
	}
	if (pipe) {
		if (!PeekNamedPipe(inh, NULL, 0, NULL, &dw, NULL))
			return 1;
		return dw;
	} else {
		GetNumberOfConsoleInputEvents(inh, &dw);
		return dw <= 1 ? 0 : dw;
	}
}

inline void readInput(SEARCH_INFO_S* s) {
	int bytes;
	char input[256] = "", * endc;

	if (inputWaiting()) {
		s->stopped = TRUE;
		do {
			bytes = _read(_fileno(stdin), input, 256);
		} while (bytes < 0);
		endc = strchr(input, '\n');
		if (endc)
			*endc = 0;

		if (strlen(input) > 0) {
			if (!strncmp(input, "quit", 4)) {
				s->quit = TRUE;
			}
		}
		return;
	}
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
	return (int)__popcnt64(bb);
}


/// <summary>
/// Pops least significant bit from bitboard and returns index.
/// </summary>
/// <param name="bb">Bitboard to pop lsb on</param>
/// <returns>Index of popped bit</returns>
inline int popBit(U64* bb) {
	unsigned long ret;
	_BitScanForward64(&ret, *bb);
	*bb &= (*bb - 1);
	return (int)ret;
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
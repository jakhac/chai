#pragma once

#include <intrin.h>
#include "io.h"
#include <iostream>
#include <fstream>
#include <ctime>
#include <chrono>  // chrono::system_clock
#include <ctime>   // localtime
#include <iomanip> // put_time

#include "defs.h"
#include "mask.h"
#include "windows.h"

/**
 * Moves all set bits in the given bitboard n squares west and returns the new bitboard,
 * discarding those that fall off the edge.
 *
 * @param  board Board to move bits west on.
 * @param  n	 Number of squares to move west.
 *
 * @returns New bitboard with shifted bits to west.
 */
inline bitboard_t westN(bitboard_t board, int n) {
	bitboard_t newBoard = board;
	for (int i = 0; i < n; i++) {
		newBoard = ((newBoard >> 1) & (~FILE_H));
	}
	return newBoard;
}

/**
 * Gets current time milliseconds.
 *
 * @returns The time in milliseconds.
 */
inline int getTimeMs() {
	return (int)GetTickCount64();
}

/**
 * Determines if we can input waiting
 *
 * @returns True if input is waiting, else false.
 */
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

/**
 * Read input from command line during search. Sets stop to true,
 * if search has to be aborted.
 *
 * @param  s The search info.
 */
inline void readInput(search_t* s) {
	int bytes;
	char input[256] = "", * endc;

	if (inputWaiting()) {
		s->stopped = true;
		do {
			bytes = _read(_fileno(stdin), input, 256);
		} while (bytes < 0);
		endc = strchr(input, '\n');
		if (endc)
			*endc = 0;

		if (strlen(input) > 0) {
			if (!strncmp(input, "quit", 4)) {
				cout << "READ INPUT: quit" << endl;
				s->quit = true;
			}
		}
		return;
	}
}

/**
 * Gets the time
 *
 * @returns The time.
 */
inline string getTime() {
	char str[32]{};
	time_t a = time(nullptr);
	struct tm time_info;

	if (localtime_s(&time_info, &a) == 0) strftime(str, sizeof(str), "%H:%M:%S", &time_info);
	return str;
}

/**
 * Write logging information into log.txt file.
 *
 * @param  logMsg Message to write into log file.
 */
inline void log(string logMsg) {
	ofstream ofs("log.txt", std::ios_base::out | std::ios_base::app);
	ofs << getTime() << "\t" << logMsg << '\n';
	ofs.close();
}

/**
 * Moves all set bits in the given bitboard n squares east and returns the new bitboard,
 * discarding those that fall off the edge.
 *
 * @param  board Board to move bits east on.
 * @param  n	 Number of squares to move east.
 *
 * @returns New bitboard with shifted bits to east.
 */
inline bitboard_t eastN(bitboard_t board, int n) {
	bitboard_t newBoard = board;
	for (int i = 0; i < n; i++) {
		newBoard = ((newBoard << 1) & (~FILE_A));
	}
	return newBoard;
}

/**
 * Return zero indexed row of given square.
 *
 * @param  square Square to check row on.
 *
 * @returns Zero indexed row.
 */
inline int row(int square) {
	return square / 8;
}

/**
 * Return zero indexed column of given square.
 *
 * @param  square Square to check column on.
 *
 * @returns Zero indexed column.
 */
inline int col(int square) {
	return square % 8;
}

/**
 * Bit scan forward and return index. If Board is 0 return -1. Uses compiler bitscan.
 *
 * @param  board Board.
 *
 * @returns INdex of first found bit.
 */
inline int bitscanForward(bitboard_t board) {
	if (board == 0ULL) return -1;

	unsigned long ret;
	_BitScanForward64(&ret, board);
	return (int)ret;
}

/**
 * Reversed bit scan forward amd return index. If Board is 0 return -1.
 *
 * @param  board Board.
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


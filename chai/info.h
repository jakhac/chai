#pragma once

#include "defs.h"
#include "mask.h"
#include <iostream>
#include <bitset>

/**
 * Print referenced bitboard.
 *
 * @param  bb Bitboard.
 */
inline void printBitBoard(bitboard_t* bb) {
	bitboard_t shiftBit = 1ULL;
	int sq;

	std::cout << std::endl;
	for (int rank = RANK_8; rank >= RANK_1; rank--) {
		for (int file = FILE_A; file <= FILE_H; file++) {
			sq = 8 * rank + file;

			if ((shiftBit << sq) & *bb) {
				std::cout << "1 ";
			} else {
				std::cout << ". ";
			}
		}
		std::cout << std::endl;
	}
}

/**
 * Print move in algebraic notation and promotions if possible
 *
 * @param  move Move.
 */
inline void printMove(const int move) {

	if (move == -1) {
		cout << "0000" << endl;
		return;
	}

	int promoted = promPiece(move);
	char promChar = ' ';

	if (promoted) {
		promChar = 'q';
		if (promoted == n || promoted == N) {
			promChar = 'n';
		} else if (promoted == r || promoted == R) {
			promChar = 'r';
		} else if (promoted == b || promoted == B) {
			promChar = 'b';
		}
	}

	string ret = "";
	ret += ('a' + squareToFile[fromSq(move)]);
	ret += ('1' + squareToRank[fromSq(move)]);
	ret += ('a' + squareToFile[toSq(move)]);
	ret += ('1' + squareToRank[toSq(move)]);

	cout << ret << promChar << endl;
}

/**
 * Returns the algebraic notation of given move.
 *
 * @param  move The move.
 *
 * @returns The move in algebraic notation.
 */
inline string getStringMove(const int move) {
	if (move == -1) {
		return "0000";
	}


	int promoted = promPiece(move);
	string promChar = " ";

	if (promoted) {
		promChar = "q ";
		if (promoted == n || promoted == N) {
			promChar = "n ";
		} else if (promoted == r || promoted == R) {
			promChar = "r ";
		} else if (promoted == b || promoted == B) {
			promChar = "b ";
		}
	}

	string ret = "";
	ret += ('a' + squareToFile[fromSq(move)]);
	ret += ('1' + squareToRank[fromSq(move)]);
	ret += ('a' + squareToFile[toSq(move)]);
	ret += ('1' + squareToRank[toSq(move)]);

	ret += promChar;
	return ret;
}

/**
 * Print all flags and attributes of given move.
 *
 * @param  move Move.
 */
inline void printMoveStatus(int move) {
	cout << "\n#### - Move Status: " << getStringMove(move) << endl;
	cout << "From " << fromSq(move) << " to " << toSq(move) << endl;
	cout << "Pawn start " << (move & MFLAG_PS) << endl;
	cout << "EP capture " << (move & MFLAG_EP) << endl;
	cout << "Castle move " << (move & MFLAG_CAS) << endl;
	cout << "Promoted " << (move & MCHECK_PROM) << endl;
	cout << "Promoted piece " << (promPiece(move)) << endl;
	cout << "Capture " << (move & MCHECK_CAP) << " with captured piece " << capPiece(move) << endl;
	cout << "####\n" << endl;
}

/**
 * Print binary format of given integer.
 *
 * @param  x A bitboard_t to process.
 */
inline void printBinary(bitboard_t x) {
	std::bitset<64> b(x);
	cout << b << endl;
}
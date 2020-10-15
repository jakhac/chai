#pragma once

#include "defs.h"
#include "mask.h"
#include <iostream>
#include <bitset>

/// <summary>
/// Print referenced bitboard.
/// </summary>
/// <param name="bb">Bitboard</param>
inline void printBitBoard(U64* bb) {
	U64 shiftBit = 1ULL;
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

/// <summary>
/// Print move in algebraic notation and promotions if possible
/// </summary>
/// <param name="move">Move</param>
inline void printMove(const int move) {

	if (move == -1) {
		cout << "0000" << endl;
		return;
	}

	int promoted = PROMOTED(move);
	char promChar = ' ';

	if (promoted) {
		promChar = 'q';
		if (isN(promoted)) {
			promChar = 'n';
		} else if (isRQ(promoted) && !isBQ(promoted)) {
			promChar = 'r';
		} else if (!isRQ(promoted) && isBQ(promoted)) {
			promChar = 'b';
		}
	}

	string ret = "";
	ret += ('a' + squareToFile[FROMSQ(move)]);
	ret += ('1' + squareToRank[FROMSQ(move)]);
	ret += ('a' + squareToFile[TOSQ(move)]);
	ret += ('1' + squareToRank[TOSQ(move)]);

	cout << ret << promChar << endl;
}

inline string getStringMove(const int move) {
	if (move == -1) {
		return "0000";
	}


	int promoted = PROMOTED(move);
	char promChar = ' ';

	if (promoted) {
		promChar = 'q';
		if (isN(promoted)) {
			promChar = 'n';
		} else if (isRQ(promoted) && !isBQ(promoted)) {
			promChar = 'r';
		} else if (!isRQ(promoted) && isBQ(promoted)) {
			promChar = 'b';
		}
	}

	string ret = "";
	ret += ('a' + squareToFile[FROMSQ(move)]);
	ret += ('1' + squareToRank[FROMSQ(move)]);
	ret += ('a' + squareToFile[TOSQ(move)]);
	ret += ('1' + squareToRank[TOSQ(move)]);

	ret += promChar;
	return ret;
}

/// <summary>
/// Print all flags and attributes of given move.
/// </summary>
/// <param name="move">Move</param>
inline void printMoveStatus(int move) {
	cout << "\n#### - Move Status" << endl;
	cout << "From " << FROMSQ(move) << " to " << TOSQ(move) << endl;
	cout << "Pawn start " << (move & MFLAGPS) << endl;
	cout << "EP capture " << (move & MFLAGEP) << endl;
	cout << "Castle move " << (move & MFLAGCA) << endl;
	cout << "Promoted " << (move & MCHECKPROM) << endl;
	cout << "Promoted piece " << (PROMOTED(move)) << endl;
	cout << "Capture " << (move & MCHECKCAP) << " with captured piece " << CAPTURED(move) << endl;
	cout << "####\n" << endl;
}

/// <summary>
/// Print binary format of given integer.
/// </summary>
/// /// <param name="x">unsigned long long number</param>
inline void printBinary(U64 x) {
	std::bitset<64> b(x);
	cout << b << endl;
}
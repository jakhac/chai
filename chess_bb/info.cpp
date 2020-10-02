#include "info.h"

// declare extern vars
int squareToRank[64];
int squareToFile[64];

void printBitBoard(U64* bb) {
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
void printMove(const int move) {

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

/// <summary>
/// Print all flags and attributes of given move.
/// </summary>
void printMoveStatus(int move) {
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
void printBinary(int x) {
	std::bitset<64> b(x);
	cout << b << endl;
}
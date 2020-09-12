#include "BitBoard.h"

int BitBoard::countBits(U64* bb) {
	int cnt = 0;
	while (*bb) {
		cnt += *bb & 1;
		*bb >>= 1;
	}
	return cnt;
}

int BitBoard::popBit(U64* bb) {
	U64 b = *bb ^ (*bb - 1);
	unsigned int fold = (unsigned)((b & 0xffffffff) ^ (b >> 32));
	*bb &= (*bb - 1);
	return bitTable[(fold * 0x783a9b23) >> 26];
}

///*
//Print a bitboard.
//*/
//void BitBoard::printBitBoard(U64 bb) {
//
//	U64 shiftBit = 1ULL;
//
//	int rank = 0;
//	int file = 0;
//	int sq = 0;
//	int sq64 = 0;
//
//	std::cout << std::endl;
//	for (rank = RANK_8; rank >= RANK_1; rank--) {
//		for (file = FILE_A; file <= FILE_H; file++) {
//			sq = bd.file_rank_2_sq(file, rank); // 120 based index
//			sq64 = sq120ToSq64[sq]; // 63 based index
//
//			if (shiftBit << sq64 & bb) {
//				std::cout << "1 ";
//			}
//			else {
//				std::cout << ". ";
//			}
//		}
//		std::cout << std::endl;
//	}
//}

void BitBoard::clearBit(U64* bb, int square) {
	*bb &= clearMask[square];
}

void BitBoard::setBit(U64* bb, int square) {
	*bb |= setMask[square];
}

void BitBoard::initClearSetMask() {
	int index = 0;

	// Zero both arrays
	for (index = 0; index < 64; index++) {
		setMask[index] = 0ULL << index;
		clearMask[index] = 0ULL << index;
	}

	for (index = 0; index < 64; index++) {
		setMask[index] |= (1ULL << index);
		clearMask[index] = ~setMask[index];
	}
}

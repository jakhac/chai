#include "BitBoard.h"
#include <stdexcept>


U64 BitBoard::getPieceSet(PIECE_VALUES p)
{
	// C26812: Enum class instead of enum

	int color = p / 7;
	int piece = p % 7 + 2;
	if (piece < 8) 
	{
		return allPiecesBB[color] & allPiecesBB[piece];
	}
	else 
	{
		throw std::invalid_argument("Piece type does not exist");
	}
}

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

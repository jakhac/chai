#include "attacks.h"

/// <summary>
/// (Extern) Store rook masks for all squares of board.
/// </summary>
bitboard_t rookMasks[64];

/// <summary>
/// (Extern) Store rook attacks for all possible blockers for each square.
/// </summary>
bitboard_t rookTable[64][4096];

/// <summary>
/// (Extern) Store bishop masks for all squares of board.
/// </summary>
bitboard_t bishopMasks[64];

/// <summary>
/// (Extern) Store bishop attacks for all possible blockers for each square.
/// </summary>
bitboard_t bishopTable[64][1024];

/// <summary>
/// (Extern) Store ray masks for all directions on each square.
/// </summary>
bitboard_t rays[8][64];

bitboard_t getBlockers(bitboard_t mask, int idx) {
	bitboard_t blockers = 0ULL;
	bitboard_t m = mask;
	int bits = popCount(m);
	for (int i = 0; i < bits; i++) {
		int bitPos = getPopLSB(&m);
		if (idx & (1 << i)) {
			blockers |= (1ULL << bitPos);
		}
	}
	return blockers;
}

void initRookMasks() {
	bitboard_t result;
	for (int sq = 0; sq < 64; sq++) {
		result = 0ULL;
		int rk = sq / 8, fl = sq % 8, r, f;
		for (r = rk + 1; r <= 6; r++) result |= (1ULL << (fl + r * 8));
		for (r = rk - 1; r >= 1; r--) result |= (1ULL << (fl + r * 8));
		for (f = fl + 1; f <= 6; f++) result |= (1ULL << (f + rk * 8));
		for (f = fl - 1; f >= 1; f--) result |= (1ULL << (f + rk * 8));
		rookMasks[sq] = result;
	}
}

void initBishopMasks() {
	for (int sq = 0; sq < 64; sq++) {
		bitboard_t result = 0ULL;
		int rk = sq / 8, fl = sq % 8, r, f;
		for (r = rk + 1, f = fl + 1; r <= 6 && f <= 6; r++, f++) result |= (1ULL << (f + r * 8));
		for (r = rk + 1, f = fl - 1; r <= 6 && f >= 1; r++, f--) result |= (1ULL << (f + r * 8));
		for (r = rk - 1, f = fl + 1; r >= 1 && f <= 6; r--, f++) result |= (1ULL << (f + r * 8));
		for (r = rk - 1, f = fl - 1; r >= 1 && f >= 1; r--, f--) result |= (1ULL << (f + r * 8));
		bishopMasks[sq] = result;
	}
}

void initRookMagicTable() {
	bitboard_t tableIndex;
	bitboard_t blockers, rookMoves;
	for (int sq = 0; sq < 64; sq++) {
		for (int block = 0; block < (1 << rookIndexBits[sq]); block++) {
			blockers = getBlockers(rookMasks[sq], block);

			tableIndex = (blockers * rookMagic[sq]) >> (64 - rookIndexBits[sq]);
			rookMoves = calculateRookMoves(sq, blockers);

			// check for desired hash collision
			if (rookTable[sq][tableIndex] != 0) {
				Assert(rookMoves == rookTable[sq][tableIndex]);
			}
			rookTable[sq][tableIndex] = rookMoves;
		}
	}
}

void initBishopMagicTable() {
	bitboard_t tableIndex, blockers, bishopMoves;
	for (int sq = 0; sq < 64; sq++) {
		for (int block = 0; block < (1 << bishopIndexBits[sq]); block++) {
			blockers = getBlockers(bishopMasks[sq], block);
			tableIndex = (blockers * bishopMagic[sq]) >> (64 - bishopIndexBits[sq]);
			bishopMoves = calculateBishopMoves(sq, blockers);

			if (bishopTable[sq][tableIndex] != 0) {
				Assert(bishopTable[sq][tableIndex] == bishopMoves);
			}

			bishopTable[sq][tableIndex] = bishopMoves;
		}
	}
}

bitboard_t calculateRookMoves(int square, bitboard_t blockers) {
	bitboard_t result = 0ULL;
	int rk = square / 8;
	int fl = square % 8, r, f;
	for (r = rk + 1; r <= 7; r++) {
		result |= (1ULL << (fl + r * 8));
		if (blockers & (1ULL << (fl + r * 8))) break;
	}
	for (r = rk - 1; r >= 0; r--) {
		result |= (1ULL << (fl + r * 8));
		if (blockers & (1ULL << (fl + r * 8))) break;
	}
	for (f = fl + 1; f <= 7; f++) {
		result |= (1ULL << (f + rk * 8));
		if (blockers & (1ULL << (f + rk * 8))) break;
	}
	for (f = fl - 1; f >= 0; f--) {
		result |= (1ULL << (f + rk * 8));
		if (blockers & (1ULL << (f + rk * 8))) break;
	}
	return result;
}

bitboard_t calculateBishopMoves(int sq, bitboard_t blockers) {
	bitboard_t result = 0ULL;
	int rk = sq / 8, fl = sq % 8, r, f;
	for (r = rk + 1, f = fl + 1; r <= 7 && f <= 7; r++, f++) {
		result |= (1ULL << (f + r * 8));
		if (blockers & (1ULL << (f + r * 8))) break;
	}
	for (r = rk + 1, f = fl - 1; r <= 7 && f >= 0; r++, f--) {
		result |= (1ULL << (f + r * 8));
		if (blockers & (1ULL << (f + r * 8))) break;
	}
	for (r = rk - 1, f = fl + 1; r >= 0 && f <= 7; r--, f++) {
		result |= (1ULL << (f + r * 8));
		if (blockers & (1ULL << (f + r * 8))) break;
	}
	for (r = rk - 1, f = fl - 1; r >= 0 && f >= 0; r--, f--) {
		result |= (1ULL << (f + r * 8));
		if (blockers & (1ULL << (f + r * 8))) break;
	}
	return result;
}

bitboard_t lookUpRookMoves(int sq, bitboard_t blockers) {
	blockers &= rookMasks[sq];
	bitboard_t key = (blockers * rookMagic[sq]) >> (64 - rookIndexBits[sq]);
	return rookTable[sq][key];
}

bitboard_t lookUpBishopMoves(int sq, bitboard_t blockers) {
	blockers &= bishopMasks[sq];
	bitboard_t key = (blockers * bishopMagic[sq]) >> (64 - bishopIndexBits[sq]);
	return bishopTable[sq][key];
}


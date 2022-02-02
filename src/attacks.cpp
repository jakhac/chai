#include "attacks.h"

// Bit set- and clear-masks
bitboard_t setMask[64];
bitboard_t clearMask[64];

bitboard_t rookTable[64][4096];
bitboard_t bishopTable[64][1024];

// Move masks for all pieces
bitboard_t pawnAtkMask[2][64];
bitboard_t knightAtkMask[64];
bitboard_t bishopMasks[64];
bitboard_t rookMasks[64];
bitboard_t kingAtkMask[64];

int squareToRank[64];
int squareToFile[64];

int dirFromTo[64][64];
int manhatten[64][64];

bitboard_t dirBitmap[64][8];
bitboard_t inBetween[64][64];
bitboard_t lineBB[64][64];

// Evaluation masks
bitboard_t pawnIsolatedMask[64];
bitboard_t pawnPassedMask[2][64];
bitboard_t upperMask[64];
bitboard_t lowerMask[64];
bitboard_t pawnShield[2][64];
bitboard_t xMask[64];
bitboard_t dangerZone[2][64];
bitboard_t outpostSquares[2];
bitboard_t horizontalNeighbors[64];


static bitboard_t getBlockers(bitboard_t mask, int idx) {

	bitboard_t blockers = 0ULL;
	bitboard_t m = mask;
	int bits = popCount(m);

	for (int i = 0; i < bits; i++) {
		Assert(m);
		int bitPos = popLSB(&m);
		if (idx & (1 << i)) {
			blockers |= (1ULL << bitPos);
		}
	}

	return blockers;
}

static void initRookMasks() {

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

static void initBishopMasks() {

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

static void initNonSliderAttacks() {

	// Pawns
	for (int i = 0; i < 64; i++) {
		pawnAtkMask[WHITE][i] = (setMask[i] << 7 & ~FILE_H_HEX) 
							   | (setMask[i] << 9 & ~FILE_A_HEX);
	}
	for (int i = 63; i >= 0; i--) {
		pawnAtkMask[BLACK][i] = (setMask[i] >> 7 & ~FILE_A_HEX) 
			 				   | (setMask[i] >> 9 & ~FILE_H_HEX);
	}

	// Knights
	for (int i = 0; i < 64; i++) {
		knightAtkMask[i] |= (setMask[i] << 17) & ~FILE_A_HEX;
		knightAtkMask[i] |= (setMask[i] >> 17) & ~FILE_H_HEX;
		knightAtkMask[i] |= (setMask[i] >> 15) & ~FILE_A_HEX;
		knightAtkMask[i] |= (setMask[i] << 15) & ~FILE_H_HEX;
		knightAtkMask[i] |= (setMask[i] >> 6 ) & ~(FILE_A_HEX | FILE_B_HEX);
		knightAtkMask[i] |= (setMask[i] << 6 ) & ~(FILE_G_HEX | FILE_H_HEX);
		knightAtkMask[i] |= (setMask[i] << 10) & ~(FILE_A_HEX | FILE_B_HEX);
		knightAtkMask[i] |= (setMask[i] >> 10) & ~(FILE_G_HEX | FILE_H_HEX);
	}

	// Kings
	for (int i = 0; i < 64; i++) {
		bitboard_t kingSet = setMask[i];
		bitboard_t attacks = (kingSet << 1 & ~FILE_A_HEX) 
						   | (kingSet >> 1 & ~FILE_H_HEX);

		kingSet |= attacks;
		attacks |= (kingSet << 8) | (kingSet >> 8);

		kingAtkMask[i] = attacks;
	}
}

static bitboard_t calculateRookMoves(int square, bitboard_t blockers) {

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

static bitboard_t calculateBishopMoves(int sq, bitboard_t blockers) {

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

static void initRookMagicTable() {

	bitboard_t tableIndex;
	bitboard_t blockers, rookMoves;

	for (int sq = 0; sq < 64; sq++) {
		for (int block = 0; block < (1 << rookIndexBits[sq]); block++) {
			blockers = getBlockers(rookMasks[sq], block);

			tableIndex = (blockers * rookMagic[sq]) >> (64 - rookIndexBits[sq]);
			rookMoves = calculateRookMoves(sq, blockers);

			if (rookTable[sq][tableIndex] != 0) {
				Assert(rookMoves == rookTable[sq][tableIndex]);
			}

			rookTable[sq][tableIndex] = rookMoves;
		}
	}
}

static void initBishopMagicTable() {

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

bitboard_t lookUpQueenMoves(int sq, bitboard_t blockers) {
	return lookUpRookMoves(sq, blockers) | lookUpBishopMoves(sq, blockers);
}

static void initHorizontalNeighbors() {

	for (int i = 0; i < 64; i++) {

		bitboard_t left  = (toFile(i) == FILE_A) ? 0ULL : setMask[i-1];
		bitboard_t right = (toFile(i) == FILE_H) ? 0ULL : setMask[i+1];

		horizontalNeighbors[i] = left | right;
	}
}

static void initClearSetMask() {

	int index = 0;

	// Zero both arrays
	for (index = 0; index < 64; index++) {
		setMask[index]   = 0ULL << index;
		clearMask[index] = 0ULL << index;
	}

	for (index = 0; index < 64; index++) {
		setMask[index]  |= (1ULL << index);
		clearMask[index] = ~setMask[index];
	}
}

static void initSquareToRankFile() {

	int sq;
	for (int rank = RANK_8; rank >= RANK_1; rank--) {
		for (int file = FILE_A; file <= FILE_H; file++) {
			//sq = file_rank_2_sq(file, rank);
			sq = 8 * rank + file;
			squareToFile[sq] = file;
			squareToRank[sq] = rank;
		}
	}
}

static void initEvalMasks() {

	int file;
	bitboard_t left, right;
	for (int i = 0; i < 64; i++) {
		file = squareToFile[i] - 1;
		left = (file >= 0) ? FILE_LIST[file] : 0ULL;

		file = squareToFile[i] + 1;
		right = (file <= 7) ? FILE_LIST[file] : 0ULL;

		pawnIsolatedMask[i] = left | right;
	}

	for (int i = 0; i < 64; i++) {
		int rank = squareToRank[i] + 1;
		while (rank <= 7) {
			upperMask[i] |= RANK_LIST[rank];
			rank++;
		}

		rank = squareToRank[i] - 1;
		while (rank >= 0) {
			lowerMask[i] |= RANK_LIST[rank];
			rank--;
		}
	}

	for (int i = 0; i < 64; i++) {
		pawnPassedMask[WHITE][i]  = upperMask[i] & (FILE_LIST[squareToFile[i]] 
												 | pawnIsolatedMask[i]);
		pawnPassedMask[BLACK][i] = lowerMask[i] & (FILE_LIST[squareToFile[i]] 
												 | pawnIsolatedMask[i]);
	}

	bitboard_t shield;
	for (int i = 0; i < 64; i++) {
		shield = 0ULL;
		shield =  setMask[i]
			   | (setMask[i] >> 1 & ~FILE_H_HEX) 
			   | (setMask[i] << 1 & ~FILE_A_HEX) ;

		pawnShield[WHITE][i] = (shield << 8) | (shield << 16);
		pawnShield[BLACK][i] = (shield >> 8) | (shield >> 16);
	}

	for (int i = 0; i < 64; i++) {
		xMask[i] = pawnAtkMask[WHITE][i] | pawnAtkMask[BLACK][i];
	}

	for (int i = 0; i < 64; i++) {
		dangerZone[WHITE][i] = kingAtkMask[i] | (1ULL << i) | (1ULL << (i + 16));
		dangerZone[BLACK][i] = kingAtkMask[i] | (1ULL << i) | (1ULL << (i - 16));
	}

	outpostSquares[WHITE]  = RANK_4_HEX | RANK_5_HEX | RANK_6_HEX;
	outpostSquares[BLACK] = RANK_5_HEX | RANK_4_HEX | RANK_3_HEX;

}

static void initManhattenMask() {

	int file1, file2, rank1, rank2;
	int rankDistance, fileDistance;

	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 64; j++) {
			file1 = i & 7;
			file2 = j & 7;
			rank1 = i >> 3;
			rank2 = j >> 3;
			rankDistance = abs(rank2 - rank1);
			fileDistance = abs(file2 - file1);

			manhatten[i][j] = rankDistance + fileDistance;
		}
	}
}

static void initObstructed() {

	const int kingd[8] = { -9, -1, 7, 8, 9, 1, -7, -8 };

	for (int sq = 0; sq < 64; ++sq) {
		for (int sq2 = 0; sq2 < 64; ++sq2) dirFromTo[sq][sq2] = 8;
		for (int k = 0; k < 8; ++k) {
			dirBitmap[sq][k] = 0;
			for (int p = sq, n = sq + kingd[k]; n >= 0 && n < 64 && abs((n & 7) - (p & 7)) < 2; p = n, n += kingd[k]) {
				dirFromTo[sq][n] = k;
				dirBitmap[sq][k] |= setMask[n];
			}
		}
	}

	for (int sq = 0; sq < 64; ++sq) {
		for (int sq2 = 0; sq2 < 64; ++sq2) {
			inBetween[sq][sq2] = bitboard_t(0);
			int k = dirFromTo[sq][sq2];
			if (k != 8) {
				int k2 = dirFromTo[sq2][sq];
				inBetween[sq][sq2] = dirBitmap[sq][k] & dirBitmap[sq2][k2];
			}
		}
	}
}

static void initLine() {

	bitboard_t midDiagonalUp = 0x8040201008040201;
	bitboard_t midDiagonalDown = 0x0102040810204080;
	bitboard_t vertical = FILE_A_HEX;
	bitboard_t horizontal = RANK_1_HEX;
	std::vector<bitboard_t> axis = { 
		midDiagonalDown, midDiagonalUp, vertical, horizontal,
		FILE_B_HEX, FILE_C_HEX, FILE_D_HEX, FILE_E_HEX,
		FILE_F_HEX, FILE_G_HEX, FILE_H_HEX };

	for (int i = 1; i < 8; i++) {
		axis.push_back(midDiagonalDown << (i * 8));
		axis.push_back(midDiagonalDown >> (i * 8));

		axis.push_back(midDiagonalUp << (i * 8));
		axis.push_back(midDiagonalUp >> (i * 8));

		axis.push_back(horizontal << (i * 8));
	}

	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 64; j++) {
			lineBB[i][j] = 0;
		}
	}

	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 64; j++) {
			if ((i == j) || lineBB[i][j] != 0) continue;
			for (auto d : axis) {
				if ((setMask[i] & d) && (setMask[j] & d)) {
					lineBB[i][j] = d;
					lineBB[j][i] = d;
				}
			}
		}
	}

}



namespace Mask {

void init() {

	initClearSetMask();
	initSquareToRankFile();
	initNonSliderAttacks();
	initEvalMasks();
	initManhattenMask();

	initHorizontalNeighbors();

	initObstructed();
	initLine();

	initRookMasks();
	initRookMagicTable();
	initBishopMasks();
	initBishopMagicTable();
}

} // namespace Mask

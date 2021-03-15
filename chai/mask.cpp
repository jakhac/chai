#include "mask.h"

// include extern vars
bitboard_t setMask[64];
bitboard_t clearMask[64];

int squareToRank[64];
int squareToFile[64];

bitboard_t pawnAtkMask[2][64];
bitboard_t knightAtkMask[64];
bitboard_t kingAtkMask[64];

bitboard_t dirBitmap[64][8];
bitboard_t inBetween[64][64];
int dirFromTo[64][64];
bitboard_t lineBB[64][64];


void initClearSetMask() {
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

void initSquareToRankFile() {
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

void initAttackerMasks() {
	int offSet = 0;

	// pawns
	for (int i = 0; i < 64; i++) {
		pawnAtkMask[WHITE][i] = (setMask[i] << 7 & ~FILE_H_HEX) | (setMask[i] << 9 & ~FILE_A_HEX);
	}
	for (int i = 63; i >= 0; i--) {
		pawnAtkMask[BLACK][i] = (setMask[i] >> 7 & ~FILE_A_HEX) | (setMask[i] >> 9 & ~FILE_H_HEX);
	}

	// knights
	for (int i = 0; i < 64; i++) {
		knightAtkMask[i] |= (setMask[i] << 17) & ~FILE_A_HEX;
		knightAtkMask[i] |= (setMask[i] << 10) & ~(FILE_A_HEX | FILE_B_HEX);
		knightAtkMask[i] |= (setMask[i] >> 6) & ~(FILE_A_HEX | FILE_B_HEX);
		knightAtkMask[i] |= (setMask[i] >> 15) & ~FILE_A_HEX;
		knightAtkMask[i] |= (setMask[i] << 15) & ~FILE_H_HEX;
		knightAtkMask[i] |= (setMask[i] << 6) & ~(FILE_G_HEX | FILE_H_HEX);
		knightAtkMask[i] |= (setMask[i] >> 10) & ~(FILE_G_HEX | FILE_H_HEX);
		knightAtkMask[i] |= (setMask[i] >> 17) & ~FILE_H_HEX;
	}

	// kings
	for (int i = 0; i < 64; i++) {
		bitboard_t kingSet = setMask[i];
		bitboard_t attacks = (kingSet << 1 & ~FILE_A_HEX) | (kingSet >> 1 & ~FILE_H_HEX);
		kingSet |= attacks;
		attacks |= kingSet << 8 | kingSet >> 8;
		kingAtkMask[i] = attacks;
	}
}


void initEvalMasks() {
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
		pawnPassedMask[WHITE][i] = upperMask[i] & (FILE_LIST[squareToFile[i]] | pawnIsolatedMask[i]);
		pawnPassedMask[BLACK][i] = lowerMask[i] & (FILE_LIST[squareToFile[i]] | pawnIsolatedMask[i]);
	}

	bitboard_t shield;
	for (int i = 0; i < 64; i++) {
		shield = 0ULL;
		shield = (setMask[i] >> 1 & ~FILE_H_HEX) | (setMask[i] << 1 & ~FILE_A_HEX) | setMask[i];
		pawnShield[WHITE][i] = (shield << 8) | (shield << 16);
		pawnShield[BLACK][i] = (shield >> 8) | (shield >> 16);
	}

	for (int i = 0; i < 64; i++) {
		xMask[i] = pawnAtkMask[WHITE][i] | pawnAtkMask[BLACK][i];
	}
}

void initManhattenMask() {
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

			manhattenDistance[i][j] = rankDistance + fileDistance;
		}
	}
}

void initObstructed() {
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

vector<bitboard_t> initLine() {

	bitboard_t midDiagonalUp = 0x8040201008040201;
	bitboard_t midDiagonalDown = 0x0102040810204080;
	bitboard_t vertical = FILE_A_HEX;
	bitboard_t horizontal = RANK_1_HEX;
	vector<bitboard_t> axis = { midDiagonalDown, midDiagonalUp, vertical, horizontal,
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

	return axis;
}

bitboard_t line_bb(int s1, int s2) {
	return lineBB[s1][s2];
}

bool aligned(int s1, int s2, int s3) {
	return line_bb(s1, s2) & s3;
}

bitboard_t obstructed(int sq1, int sq2) {
	return inBetween[sq1][sq2];
}

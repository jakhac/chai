#pragma once

#include "defs.h"
#include <vector>

// clear set bit masks
extern bitboard_t setMask[64];
extern bitboard_t clearMask[64];

// convert sq to rank or file
extern int squareToRank[64];
extern int squareToFile[64];

// attacker mask for pieces
extern bitboard_t pawnAtkMask[2][64];
extern bitboard_t knightAtkMask[64];
extern bitboard_t kingAtkMask[64];

// helper arrays to generate obstructed method
extern bitboard_t dirBitmap[64][8];
extern bitboard_t inBetween[64][64];
extern int dirFromTo[64][64];
extern bitboard_t lineBB[64][64];

// eval masks
extern bitboard_t pawnIsolatedMask[64];
extern bitboard_t pawnPassedMask[2][64];
extern bitboard_t upperMask[64];
extern bitboard_t lowerMask[64];
extern bitboard_t pawnShield[2][64];
extern bitboard_t xMask[64];
extern int manhattenDistance[64][64];

const bitboard_t RANK_1_HEX = 0x00000000000000FF;
const bitboard_t RANK_2_HEX = 0x000000000000FF00;
const bitboard_t RANK_3_HEX = 0x0000000000FF0000;
const bitboard_t RANK_4_HEX = 0x00000000FF000000;
const bitboard_t RANK_5_HEX = 0x000000FF00000000;
const bitboard_t RANK_6_HEX = 0x0000FF0000000000;
const bitboard_t RANK_7_HEX = 0x00FF000000000000;
const bitboard_t RANK_8_HEX = 0xFF00000000000000;

const bitboard_t FILE_A_HEX = 0x0101010101010101;
const bitboard_t FILE_B_HEX = 0x0202020202020202;
const bitboard_t FILE_C_HEX = 0x0404040404040404;
const bitboard_t FILE_D_HEX = 0x0808080808080808;
const bitboard_t FILE_E_HEX = 0x1010101010101010;
const bitboard_t FILE_F_HEX = 0x2020202020202020;
const bitboard_t FILE_G_HEX = 0x4040404040404040;
const bitboard_t FILE_H_HEX = 0x8080808080808080;

const bitboard_t CENTER_SQUARES = (1ULL << D4) | (1ULL << D5) | (1ULL << E4) | (1ULL << E5);

const bitboard_t FILE_LIST[8] = {
	FILE_A_HEX, FILE_B_HEX, FILE_C_HEX, FILE_D_HEX,
	FILE_E_HEX, FILE_F_HEX, FILE_G_HEX, FILE_H_HEX
};

const bitboard_t RANK_LIST[8] = {
	RANK_1_HEX, RANK_2_HEX, RANK_3_HEX, RANK_4_HEX,
	RANK_5_HEX, RANK_6_HEX, RANK_7_HEX, RANK_8_HEX
};

const bitboard_t BLACK_SQUARES = 0xAA55AA55AA55AA55;
const bitboard_t WHITE_SQUARES = 0x55AA55AA55AA55AA;

void initClearSetMask();

/**
 * Initialize arrays to index square to files and ranks.
 */
void initSquareToRankFile();

/**
 * Initialize attacker mask for all pieces.
 */
void initAttackerMasks();
/**
 * Initializes the eval masks.
 */
void initEvalMasks();

/**
 * Initializes the manhatten mask.
 */
void initManhattenMask();

/**
 * Initialize obstructed array. All bits set between start and end square.
 */
void initObstructed();

/**
 * Initializes the line.
 *
 * @returns Bitboard;
 */
std::vector<bitboard_t> initLine();

/**
 * Returns a bitboard with line set between s1 and s2.
 *
 * @param  s1 First point on the line.
 * @param  s2 Second point on the line.
 *
 * @returns A bitboard_t.
 */
bitboard_t line_bb(int s1, int s2);

/**
 * Checks if three square are aligned (horizontal, vertical, diagonal)
 *
 * @param  s1 The first square.
 * @param  s2 The second square.
 * @param  s3 The third square.
 *
 * @returns True if it succeeds, false if it fails.
 */
bool aligned(int s1, int s2, int s3);

/**
 * Returns bitboard with all bits sets between sq1 and sq2
 *
 * @param  sq1 Square 1.
 * @param  sq2 Square 2.
 *
 * @returns A bitboard_t.
 */
bitboard_t obstructed(int sq1, int sq2);

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
vector<bitboard_t> initLine();

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

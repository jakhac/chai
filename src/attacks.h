#pragma once

#include "types.h"
#include "bitUtils.h"

// Bit set- and clear-masks
extern bitboard_t setMask[64];
extern bitboard_t clearMask[64];

// Table stores attacker set for each square and each blocker set
extern bitboard_t rookTable[64][4096];
extern bitboard_t bishopTable[64][1024];

// Array contains all masks for rook moves indexed by square
extern bitboard_t pawnAtkMask[2][64];
extern bitboard_t knightAtkMask[64];
extern bitboard_t kingAtkMask[64];
extern bitboard_t rookMasks[64];
extern bitboard_t bishopMasks[64];

extern int squareToRank[64];
extern int squareToFile[64];

extern int dirFromTo[64][64];
extern int manhatten[64][64];

extern bitboard_t dirBitmap[64][8];
extern bitboard_t inBetween[64][64];
extern bitboard_t lineBB[64][64];

// Evaluation masks
extern bitboard_t pawnIsolatedMask[64];
extern bitboard_t pawnPassedMask[2][64];
extern bitboard_t upperMask[64];
extern bitboard_t lowerMask[64];
extern bitboard_t pawnShield[2][64];
extern bitboard_t xMask[64];
extern bitboard_t dangerZone[2][64];
extern bitboard_t outpostSquares[2];
extern bitboard_t horizontalNeighbors[64];


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

const bitboard_t SQUARES_BLACK  = 0xAA55AA55AA55AA55;
const bitboard_t SQUARES_WHITE  = 0x55AA55AA55AA55AA;
const bitboard_t CENTER_SQUARES = 0x0000001818000000;
const bitboard_t MIDDLE_SQUARES = 0x00003C3C3C3C0000;
const bitboard_t BORDER_SQUARES = FILE_A_HEX | FILE_H_HEX | RANK_1_HEX | RANK_8_HEX;

const bitboard_t FILE_LIST[8] = {
	FILE_A_HEX, FILE_B_HEX, FILE_C_HEX, FILE_D_HEX,
	FILE_E_HEX, FILE_F_HEX, FILE_G_HEX, FILE_H_HEX
};

const bitboard_t RANK_LIST[8] = {
	RANK_1_HEX, RANK_2_HEX, RANK_3_HEX, RANK_4_HEX,
	RANK_5_HEX, RANK_6_HEX, RANK_7_HEX, RANK_8_HEX
};

namespace Mask {

	void init();

}

/**
 * Fast lookup for possible rook moves.
 */
bitboard_t lookUpRookMoves(int sq, bitboard_t blockers);

/**
 * Fast lookup for possible bishop moves.
 */
bitboard_t lookUpBishopMoves(int sq, bitboard_t blockers);

/**
 * Fast lookup for possible queen moves.
 */
bitboard_t lookUpQueenMoves(int sq, bitboard_t blockers);

/**
 * Returns a bitboard with line set between s1 and s2.
 * @returns Line-bitboard or zero (!) if s1 and s2 are not aligned.
 */
inline bitboard_t lineBetween(int s1, int s2) {
	return lineBB[s1][s2];
}

/**
 * Checks if three square are aligned (horizontal, vertical, diagonal)
 * @returns True if all squares are aligned, else false.
 */
inline bool aligned(int s1, int s2, int s3) {
	return lineBetween(s1, s2) & setMask[s3];
}

/**
 * Returns bitboard with all bits sets between sq1 and sq2
 */
inline bitboard_t obstructed(int sq1, int sq2) {
	return inBetween[sq1][sq2];
}

inline int toRank(int sq) {
	return squareToRank[sq];
}

inline int toFile(int sq) {
	return squareToFile[sq];
}

inline bitboard_t toRankBB(int sq) {
	return RANK_LIST[toRank(sq)];
}

inline bitboard_t toFileBB(int sq) {
	return FILE_LIST[toFile(sq)];
}


/**
 * @brief Return bitboard with all possible moves for given piece.
 */
template<pType_t piece>
inline bitboard_t getMoveMask(int sq, bitboard_t occ, color_t color) {

	switch (piece) {
		case cPAWN:   return pawnAtkMask[color][sq];
		case cKNIGHT: return knightAtkMask[sq];
		case cBISHOP: return lookUpBishopMoves(sq, occ);
		case cROOK:   return lookUpRookMoves(sq, occ);
		case cQUEEN:  return lookUpQueenMoves(sq, occ);
		case cKING:   return kingAtkMask[sq];
		default: break;
	}
}

/**
 * @brief Return bitboard with all possible moves for given piece.
 */
template<pType_t piece>
inline bitboard_t getMoveMask(int sq, bitboard_t occ) {

	switch (piece) {
		case cPAWN:   Assert(false); exit(1);
		case cKNIGHT: return knightAtkMask[sq];
		case cBISHOP: return lookUpBishopMoves(sq, occ);
		case cROOK:   return lookUpRookMoves(sq, occ);
		case cQUEEN:  return lookUpQueenMoves(sq, occ);
		case cKING:   return kingAtkMask[sq];
		default: break;
	}
}


/** Store number of bits required for that square in rook magic table index */
const int rookIndexBits[64] = {
	12, 11, 11, 11, 11, 11, 11, 12,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	12, 11, 11, 11, 11, 11, 11, 12
};

/** Store number of bits required for that square in bishop magic table index */
const int bishopIndexBits[64] = {
	6, 5, 5, 5, 5, 5, 5, 6,
	5, 5, 5, 5, 5, 5, 5, 5,
	5, 5, 7, 7, 7, 7, 5, 5,
	5, 5, 7, 9, 9, 7, 5, 5,
	5, 5, 7, 9, 9, 7, 5, 5,
	5, 5, 7, 7, 7, 7, 5, 5,
	5, 5, 5, 5, 5, 5, 5, 5,
	6, 5, 5, 5, 5, 5, 5, 6
};

/** Magic numbers for bishop squares */
const bitboard_t bishopMagic[64] = {
	bitboard_t(0x0002020202020200), bitboard_t(0x0002020202020000), bitboard_t(0x0004010202000000), bitboard_t(0x0004040080000000),
	bitboard_t(0x0001104000000000), bitboard_t(0x0000821040000000), bitboard_t(0x0000410410400000), bitboard_t(0x0000104104104000),
	bitboard_t(0x0000040404040400), bitboard_t(0x0000020202020200), bitboard_t(0x0000040102020000), bitboard_t(0x0000040400800000),
	bitboard_t(0x0000011040000000), bitboard_t(0x0000008210400000), bitboard_t(0x0000004104104000), bitboard_t(0x0000002082082000),
	bitboard_t(0x0004000808080800), bitboard_t(0x0002000404040400), bitboard_t(0x0001000202020200), bitboard_t(0x0000800802004000),
	bitboard_t(0x0000800400A00000), bitboard_t(0x0000200100884000), bitboard_t(0x0000400082082000), bitboard_t(0x0000200041041000),
	bitboard_t(0x0002080010101000), bitboard_t(0x0001040008080800), bitboard_t(0x0000208004010400), bitboard_t(0x0000404004010200),
	bitboard_t(0x0000840000802000), bitboard_t(0x0000404002011000), bitboard_t(0x0000808001041000), bitboard_t(0x0000404000820800),
	bitboard_t(0x0001041000202000), bitboard_t(0x0000820800101000), bitboard_t(0x0000104400080800), bitboard_t(0x0000020080080080),
	bitboard_t(0x0000404040040100), bitboard_t(0x0000808100020100), bitboard_t(0x0001010100020800), bitboard_t(0x0000808080010400),
	bitboard_t(0x0000820820004000), bitboard_t(0x0000410410002000), bitboard_t(0x0000082088001000), bitboard_t(0x0000002011000800),
	bitboard_t(0x0000080100400400), bitboard_t(0x0001010101000200), bitboard_t(0x0002020202000400), bitboard_t(0x0001010101000200),
	bitboard_t(0x0000410410400000), bitboard_t(0x0000208208200000), bitboard_t(0x0000002084100000), bitboard_t(0x0000000020880000),
	bitboard_t(0x0000001002020000), bitboard_t(0x0000040408020000), bitboard_t(0x0004040404040000), bitboard_t(0x0002020202020000),
	bitboard_t(0x0000104104104000), bitboard_t(0x0000002082082000), bitboard_t(0x0000000020841000), bitboard_t(0x0000000000208800),
	bitboard_t(0x0000000010020200), bitboard_t(0x0000000404080200), bitboard_t(0x0000040404040400), bitboard_t(0x0002020202020200)
};

/** Magic numbers for rook squares */
const bitboard_t rookMagic[64] = {
	bitboard_t(0x0080001020400080), bitboard_t(0x0040001000200040), bitboard_t(0x0080081000200080), bitboard_t(0x0080040800100080),
	bitboard_t(0x0080020400080080), bitboard_t(0x0080010200040080), bitboard_t(0x0080008001000200), bitboard_t(0x0080002040800100),
	bitboard_t(0x0000800020400080), bitboard_t(0x0000400020005000), bitboard_t(0x0000801000200080), bitboard_t(0x0000800800100080),
	bitboard_t(0x0000800400080080), bitboard_t(0x0000800200040080), bitboard_t(0x0000800100020080), bitboard_t(0x0000800040800100),
	bitboard_t(0x0000208000400080), bitboard_t(0x0000404000201000), bitboard_t(0x0000808010002000), bitboard_t(0x0000808008001000),
	bitboard_t(0x0000808004000800), bitboard_t(0x0000808002000400), bitboard_t(0x0000010100020004), bitboard_t(0x0000020000408104),
	bitboard_t(0x0000208080004000), bitboard_t(0x0000200040005000), bitboard_t(0x0000100080200080), bitboard_t(0x0000080080100080),
	bitboard_t(0x0000040080080080), bitboard_t(0x0000020080040080), bitboard_t(0x0000010080800200), bitboard_t(0x0000800080004100),
	bitboard_t(0x0000204000800080), bitboard_t(0x0000200040401000), bitboard_t(0x0000100080802000), bitboard_t(0x0000080080801000),
	bitboard_t(0x0000040080800800), bitboard_t(0x0000020080800400), bitboard_t(0x0000020001010004), bitboard_t(0x0000800040800100),
	bitboard_t(0x0000204000808000), bitboard_t(0x0000200040008080), bitboard_t(0x0000100020008080), bitboard_t(0x0000080010008080),
	bitboard_t(0x0000040008008080), bitboard_t(0x0000020004008080), bitboard_t(0x0000010002008080), bitboard_t(0x0000004081020004),
	bitboard_t(0x0000204000800080), bitboard_t(0x0000200040008080), bitboard_t(0x0000100020008080), bitboard_t(0x0000080010008080),
	bitboard_t(0x0000040008008080), bitboard_t(0x0000020004008080), bitboard_t(0x0000800100020080), bitboard_t(0x0000800041000080),
	bitboard_t(0x00FFFCDDFCED714A), bitboard_t(0x007FFCDDFCED714A), bitboard_t(0x003FFFCDFFD88096), bitboard_t(0x0000040810002101),
	bitboard_t(0x0001000204080011), bitboard_t(0x0001000204000801), bitboard_t(0x0001000082000401), bitboard_t(0x0001FFFAABFAD1A2)
};
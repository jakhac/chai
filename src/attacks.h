#pragma once

#include "types.h"
#include "bitUtils.h"

// Bit set- and clear-masks
extern Bitboard setMask[64];
extern Bitboard clearMask[64];

// Table stores attacker set for each square and each blocker set
extern Bitboard rookTable[64][4096];
extern Bitboard bishopTable[64][1024];

// Array contains all masks for rook moves indexed by square
extern Bitboard pawnAtkMask[2][64];
extern Bitboard knightAtkMask[64];
extern Bitboard kingAtkMask[64];
extern Bitboard rookMasks[64];
extern Bitboard bishopMasks[64];

extern int squareToRank[64];
extern int squareToFile[64];

extern int dirFromTo[64][64];
extern int manhatten[64][64];

extern Bitboard dirBitmap[64][8];
extern Bitboard inBetween[64][64];
extern Bitboard lineBB[64][64];

// Evaluation masks
extern Bitboard pawnIsolatedMask[64];
extern Bitboard pawnPassedMask[2][64];
extern Bitboard upperMask[64];
extern Bitboard lowerMask[64];
extern Bitboard pawnShield[2][64];
extern Bitboard xMask[64];
extern Bitboard dangerZone[2][64];
extern Bitboard outpostSquares[2];
extern Bitboard horizontalNeighbors[64];


const Bitboard RANK_1_HEX = 0x00000000000000FF;
const Bitboard RANK_2_HEX = 0x000000000000FF00;
const Bitboard RANK_3_HEX = 0x0000000000FF0000;
const Bitboard RANK_4_HEX = 0x00000000FF000000;
const Bitboard RANK_5_HEX = 0x000000FF00000000;
const Bitboard RANK_6_HEX = 0x0000FF0000000000;
const Bitboard RANK_7_HEX = 0x00FF000000000000;
const Bitboard RANK_8_HEX = 0xFF00000000000000;

const Bitboard FILE_A_HEX = 0x0101010101010101;
const Bitboard FILE_B_HEX = 0x0202020202020202;
const Bitboard FILE_C_HEX = 0x0404040404040404;
const Bitboard FILE_D_HEX = 0x0808080808080808;
const Bitboard FILE_E_HEX = 0x1010101010101010;
const Bitboard FILE_F_HEX = 0x2020202020202020;
const Bitboard FILE_G_HEX = 0x4040404040404040;
const Bitboard FILE_H_HEX = 0x8080808080808080;

const Bitboard SQUARES_BLACK  = 0xAA55AA55AA55AA55;
const Bitboard SQUARES_WHITE  = 0x55AA55AA55AA55AA;
const Bitboard CENTER_SQUARES = 0x0000001818000000;
const Bitboard MIDDLE_SQUARES = 0x00003C3C3C3C0000;
const Bitboard BORDER_SQUARES = FILE_A_HEX | FILE_H_HEX | RANK_1_HEX | RANK_8_HEX;

const Bitboard FILE_LIST[8] = {
    FILE_A_HEX, FILE_B_HEX, FILE_C_HEX, FILE_D_HEX,
    FILE_E_HEX, FILE_F_HEX, FILE_G_HEX, FILE_H_HEX
};

const Bitboard RANK_LIST[8] = {
    RANK_1_HEX, RANK_2_HEX, RANK_3_HEX, RANK_4_HEX,
    RANK_5_HEX, RANK_6_HEX, RANK_7_HEX, RANK_8_HEX
};

namespace Mask {

    void init();

}

/**
 * Fast lookup for possible rook moves.
 */
Bitboard lookUpRookMoves(int sq, Bitboard blockers);

/**
 * Fast lookup for possible bishop moves.
 */
Bitboard lookUpBishopMoves(int sq, Bitboard blockers);

/**
 * Fast lookup for possible queen moves.
 */
Bitboard lookUpQueenMoves(int sq, Bitboard blockers);

/**
 * Returns a bitboard with line set between s1 and s2.
 * @returns Line-bitboard or zero (!) if s1 and s2 are not aligned.
 */
inline Bitboard lineBetween(int s1, int s2) {
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
inline Bitboard obstructed(int sq1, int sq2) {
    return inBetween[sq1][sq2];
}

inline int toRank(int sq) {
    return squareToRank[sq];
}

inline int toFile(int sq) {
    return squareToFile[sq];
}

inline Bitboard toRankBB(int sq) {
    return RANK_LIST[toRank(sq)];
}

inline Bitboard toFileBB(int sq) {
    return FILE_LIST[toFile(sq)];
}


/**
 * @brief Return bitboard with all possible moves for given piece.
 */
template<PieceType piece>
inline Bitboard getMoveMask(int sq, Bitboard occ, Color color) {

    switch (piece) {
        case PAWN:   return pawnAtkMask[color][sq];
        case KNIGHT: return knightAtkMask[sq];
        case BISHOP: return lookUpBishopMoves(sq, occ);
        case ROOK:   return lookUpRookMoves(sq, occ);
        case QUEEN:  return lookUpQueenMoves(sq, occ);
        case KING:   return kingAtkMask[sq];
        default: break;
    }
}

/**
 * @brief Return bitboard with all possible moves for given piece.
 */
template<PieceType piece>
inline Bitboard getMoveMask(int sq, Bitboard occ) {

    switch (piece) {
        case PAWN:   Assert(false); exit(1);
        case KNIGHT: return knightAtkMask[sq];
        case BISHOP: return lookUpBishopMoves(sq, occ);
        case ROOK:   return lookUpRookMoves(sq, occ);
        case QUEEN:  return lookUpQueenMoves(sq, occ);
        case KING:   return kingAtkMask[sq];
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
const Bitboard bishopMagic[64] = {
    Bitboard(0x0002020202020200), Bitboard(0x0002020202020000), Bitboard(0x0004010202000000), Bitboard(0x0004040080000000),
    Bitboard(0x0001104000000000), Bitboard(0x0000821040000000), Bitboard(0x0000410410400000), Bitboard(0x0000104104104000),
    Bitboard(0x0000040404040400), Bitboard(0x0000020202020200), Bitboard(0x0000040102020000), Bitboard(0x0000040400800000),
    Bitboard(0x0000011040000000), Bitboard(0x0000008210400000), Bitboard(0x0000004104104000), Bitboard(0x0000002082082000),
    Bitboard(0x0004000808080800), Bitboard(0x0002000404040400), Bitboard(0x0001000202020200), Bitboard(0x0000800802004000),
    Bitboard(0x0000800400A00000), Bitboard(0x0000200100884000), Bitboard(0x0000400082082000), Bitboard(0x0000200041041000),
    Bitboard(0x0002080010101000), Bitboard(0x0001040008080800), Bitboard(0x0000208004010400), Bitboard(0x0000404004010200),
    Bitboard(0x0000840000802000), Bitboard(0x0000404002011000), Bitboard(0x0000808001041000), Bitboard(0x0000404000820800),
    Bitboard(0x0001041000202000), Bitboard(0x0000820800101000), Bitboard(0x0000104400080800), Bitboard(0x0000020080080080),
    Bitboard(0x0000404040040100), Bitboard(0x0000808100020100), Bitboard(0x0001010100020800), Bitboard(0x0000808080010400),
    Bitboard(0x0000820820004000), Bitboard(0x0000410410002000), Bitboard(0x0000082088001000), Bitboard(0x0000002011000800),
    Bitboard(0x0000080100400400), Bitboard(0x0001010101000200), Bitboard(0x0002020202000400), Bitboard(0x0001010101000200),
    Bitboard(0x0000410410400000), Bitboard(0x0000208208200000), Bitboard(0x0000002084100000), Bitboard(0x0000000020880000),
    Bitboard(0x0000001002020000), Bitboard(0x0000040408020000), Bitboard(0x0004040404040000), Bitboard(0x0002020202020000),
    Bitboard(0x0000104104104000), Bitboard(0x0000002082082000), Bitboard(0x0000000020841000), Bitboard(0x0000000000208800),
    Bitboard(0x0000000010020200), Bitboard(0x0000000404080200), Bitboard(0x0000040404040400), Bitboard(0x0002020202020200)
};

/** Magic numbers for rook squares */
const Bitboard rookMagic[64] = {
    Bitboard(0x0080001020400080), Bitboard(0x0040001000200040), Bitboard(0x0080081000200080), Bitboard(0x0080040800100080),
    Bitboard(0x0080020400080080), Bitboard(0x0080010200040080), Bitboard(0x0080008001000200), Bitboard(0x0080002040800100),
    Bitboard(0x0000800020400080), Bitboard(0x0000400020005000), Bitboard(0x0000801000200080), Bitboard(0x0000800800100080),
    Bitboard(0x0000800400080080), Bitboard(0x0000800200040080), Bitboard(0x0000800100020080), Bitboard(0x0000800040800100),
    Bitboard(0x0000208000400080), Bitboard(0x0000404000201000), Bitboard(0x0000808010002000), Bitboard(0x0000808008001000),
    Bitboard(0x0000808004000800), Bitboard(0x0000808002000400), Bitboard(0x0000010100020004), Bitboard(0x0000020000408104),
    Bitboard(0x0000208080004000), Bitboard(0x0000200040005000), Bitboard(0x0000100080200080), Bitboard(0x0000080080100080),
    Bitboard(0x0000040080080080), Bitboard(0x0000020080040080), Bitboard(0x0000010080800200), Bitboard(0x0000800080004100),
    Bitboard(0x0000204000800080), Bitboard(0x0000200040401000), Bitboard(0x0000100080802000), Bitboard(0x0000080080801000),
    Bitboard(0x0000040080800800), Bitboard(0x0000020080800400), Bitboard(0x0000020001010004), Bitboard(0x0000800040800100),
    Bitboard(0x0000204000808000), Bitboard(0x0000200040008080), Bitboard(0x0000100020008080), Bitboard(0x0000080010008080),
    Bitboard(0x0000040008008080), Bitboard(0x0000020004008080), Bitboard(0x0000010002008080), Bitboard(0x0000004081020004),
    Bitboard(0x0000204000800080), Bitboard(0x0000200040008080), Bitboard(0x0000100020008080), Bitboard(0x0000080010008080),
    Bitboard(0x0000040008008080), Bitboard(0x0000020004008080), Bitboard(0x0000800100020080), Bitboard(0x0000800041000080),
    Bitboard(0x00FFFCDDFCED714A), Bitboard(0x007FFCDDFCED714A), Bitboard(0x003FFFCDFFD88096), Bitboard(0x0000040810002101),
    Bitboard(0x0001000204080011), Bitboard(0x0001000204000801), Bitboard(0x0001000082000401), Bitboard(0x0001FFFAABFAD1A2)
};
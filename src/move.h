#pragma once

#include "defs.h"

/*
    16 Bit encoding:
    0000 0000 0011 1111 from square
    0000 1111 1100 1111 to square

    --00 Normal
    --01 Castle
    --10 EnPas
    --11 Promotion
    00-- N
    01-- B
    10-- R
    11-- Q
*/


enum MOVE_FLAG {
    NORMAL_MOVE    = 0 << 12,
    CASTLE_MOVE    = 1 << 12,
    EP_MOVE        = 2 << 12,
    PROM_MOVE      = 3 << 12,

    PROM_TO_KNIGHT = 0 << 14,
    PROM_TO_BISHOP = 1 << 14,
    PROM_TO_ROOK   = 2 << 14,
    PROM_TO_QUEEN  = 3 << 14,
};


const Move MOVE_NONE = 0;
const Move MOVE_NULL = 4095; // C1-C1 used as nullmove (impossible move, never generated)

const int sqBitMask = 0x3F;

const int promIndex[]      = { KNIGHT, BISHOP, ROOK, QUEEN };
const int piecePromIndex[] = { 0, 0, PROM_TO_KNIGHT, PROM_TO_BISHOP, PROM_TO_ROOK, PROM_TO_QUEEN, 0 };

// new move, promPiece
inline Move serializeMove(int from, int to, int moveType, int promoteTo) {
    Assert(promoteTo == 0 || (PROM_TO_KNIGHT <= promoteTo && promoteTo <= PROM_TO_QUEEN));

    return (from)
         | (to << 6)
         | (moveType)
         | (promoteTo);
}

inline int fromSq(Move move) {
    return move & sqBitMask;
}

inline int toSq(Move move) {
    return (move >> 6) & sqBitMask;
}

inline bool isPromotion(Move move) {
    return (move & (3 << 12)) == PROM_MOVE;
}

inline int promPiece(Board* b, Move move) {
    return isPromotion(move) ? (move >> 14) + 2 + (b->stm == BLACK ? 6 : 0)
                             : NO_PTYPE;
}

inline int promPieceType(Move move) {
    return (move >> 14) + 2;
}

inline bool isPawnStart(Move move, int movingPiece) {
    return (movingPiece == Piece::P || movingPiece == Piece::p)
        && abs(fromSq(move) - toSq(move)) == 16;
}

inline bool isEnPassant(Move move) {
    return (move & (3 << 12)) == EP_MOVE;
}

inline bool isCastling(Move move) {
    return (move & (3 << 12)) == CASTLE_MOVE;
}
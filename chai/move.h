#pragma once

//#include "board.h"
#include "defs.h"

/*
	16 Bit encoding:
	0000 0000 0011 1111 from square
	0000 1111 1100 1111 to square
	0001 0000 0000 0000 n prom
	0010 0000 0000 0000 b prom
	0100 0000 0000 0000 r prom
	1000 0000 0000 0000 q prom

	--00 Normal
	--01 Castle
	--10 EnPas
	--11 Promotion
	00-- N
	01-- B
	10-- R
	11-- Q
*/

const int sqBitMask = 0x3F;

const int promPieceIndex[] = { Piece::KNIGHT, Piece::BISHOP, Piece::ROOK, Piece::QUEEN };
const int piecePromIndex[] = { 0, 0, PROM_TO_KNIGHT, PROM_TO_BISHOP, PROM_TO_ROOK, PROM_TO_QUEEN, 0 };

// new move, promPiece
inline move_t serializeMove(int from, int to, int flag, int promoteTo) {
	Assert(promoteTo == 0 || (PROM_TO_KNIGHT <= promoteTo && promoteTo <= PROM_TO_QUEEN));

	return (from)
		| (to << 6)
		| (flag)
		| (promoteTo);
}

/**
 * Get the fromSq of a serialized move.
 *
 * @param  move Serialized move.
 *
 * @returns From square.
 */
inline int fromSq(move_t move) {
	return move & sqBitMask;
}

/**
 * Get the toSq of a serialized move.
 *
 * @param  move Serialized move.
 *
 * @returns To square.
 */
inline int toSq(move_t move) {
	return (move >> 6) & sqBitMask;
}

inline bool isPromotion(move_t move) {
	return (move & (3 << 12)) == PROM_MOVE;
}

inline int promPiece(board_t* b, move_t move) {
	return isPromotion(move) ?
		(move >> 14) + 2 + (b->side == BLACK ? 6 : 0)
		: Piece::NO_PIECE;
}

inline bool isPawnStart(move_t move, int movingPiece) {
	return piecePawn[movingPiece] && abs(fromSq(move) - toSq(move)) == 16;
}

inline bool isEnPassant(move_t move) {
	return (move & (3 << 12)) == EP_MOVE;
}

inline bool isCastling(move_t move) {
	return (move & (3 << 12)) == CASTLE_MOVE;
}
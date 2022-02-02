#pragma once

#include "attacks.h"
#include "info.h"


/**
 * @brief Get pieces of given type and color.
 */
inline bitboard_t getPieces(board_t* b, int piece, color_t color) {
	return b->pieces[pieceType[piece]] & b->color[color];
}

/**
 * @brief Returns the color at sq: Either black or white since we assume non-empty square!
 */
inline color_t colorAt(board_t* b, int sq) {
	return (b->color[WHITE] & (1ULL << sq)) ? WHITE : BLACK;
}

/**
 * @brief Determines the piece index at given square or NO_PIECE if empty.
 */
inline int pieceAt(board_t* b, int sq) {

	for (int i = cPAWN; i <= cKING; i++) {
		if (b->pieces[i] & (1ULL << sq)) {
			int offset = (colorAt(b, sq) == BLACK) ? 6 : 0;
			return i + offset;
		}
	}

	return Piece::NO_PIECE;
}

/**
 * @brief Return the king square for color.
 */
inline getKingSquare(board_t* b, color_t color) {
	Assert(getPieces(b, cKING, color));
	return getLSB(getPieces(b, cKING, color));
}





/**
 * Calculates all pinners towards given king square.
 *
 * @param  b board_t to call function.
 * @param  kSq   Square (of attacked king).
 * @param  kSide Side of attacked king.
 *
 * @returns Bitboard revealing all pinners.
 */
bitboard_t getPinner(board_t* b, int kSq, color_t kSide);

/**
 * Generate bitboard with pinned pieces according to king square and king side. Includes rook
 * and bishop moves.
 *
 * @param  b board_t to call function.
 * @param  kSq  Square of king.
 * @param  side Side of pinned pieces (king side).
 *
 * @returns A bitboard_t.
 */
bitboard_t getPinned(board_t* b, int kSq, color_t side);

/**
 * @brief Return rooks/bishops that deliver an discovered attack towards given sq.
 * 
 * @param b 
 * @param sq Target square that is under discovered attacked.
 * @param color Color of piece that is potentially attacked.
 */
bitboard_t getDiscoveredAttacks(board_t* b, int sq, color_t color);


/**
 * Generate bitboard of attacked squares and pieces by given side.
 *
 * @param  b board_t to call function.
 * @param  color Attacker side.
 *
 * @returns Bitboard with attacked squares and pieces set.
 */
bitboard_t attackerSet(board_t* b, color_t color);

template<pType_t pType>
bitboard_t attackerSetFor(board_t* b, color_t color);


/**
 * Generate bitboard with containing all pieces that can block the given square. King moves are
 * not included. Bitboard shows origin of pieces that can block.
 *
 * @param  b board_t to call function.
 * @param  color    Side of blocking pieces.
 * @param  blockSq Square that has to be occupied.
 *
 * @returns Bitboard with blocker pieces.
 */
bitboard_t blockerSet(board_t* b, color_t color, int blockSq);

/**
 * Check if given side attacks given square.
 *
 * @param  b board_t to call function.
 * @param  square Square to check attacks on.
 * @param  color   Side that might attack the square.
 *
 * @returns A bitboard_t.
 */
bitboard_t squareAttackedBy(board_t* b, int square, color_t color);

/**
 * Get all pieces attacking the given square, independet of side.
 *
 * @param  b board_t to call function.
 * @param  square Square to check.
 *
 * @returns Bitboard with attackers / defenders.
 */
bitboard_t squareAtkDef(board_t* b, int square);

/**
 * Get all pieces attacking the given square, independent of side.
 * Extra parameter to pass the occupied squares needed or slider generation.
 *
 * @param  b board_t to call function.
 * @param  occupied Bitboard of currently occupied squares
 * @param  square Square to check.
 *
 * @returns Bitboard with attackers / defenders.
 */
bitboard_t squareAtkDefOcc(board_t* b, bitboard_t occupied, int square);

/**
 * @brief Return all pawns of given color that have an opposite colored pawn ahead.
 * 
 * @return bitboard_t 
 */
bitboard_t getBlockedPawns(board_t* b, color_t color);

/**
 * @brief Return all passed pawns (no def pawns ahead) for given color.
 */
bitboard_t getPassers(board_t* b, color_t color);

/**
 * @brief Print bitboard representation of board to console.
 */
void printBoard(board_t* board);

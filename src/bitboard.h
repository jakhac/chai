#pragma once

#include "attacks.h"
#include "info.h"


/**
 * @brief Get pieces of given type and color.
 */
inline Bitboard getPieces(Board* b, int piece, Color color) {
	return b->pieces[pieceType[piece]] & b->color[color];
}

/**
 * @brief Returns the color at sq: Either black or white since we assume non-empty square!
 */
inline Color colorAt(Board* b, int sq) {
	return (b->color[WHITE] & (1ULL << sq)) ? WHITE : BLACK;
}

/**
 * @brief Determines the piece index at given square or NO_PIECE if empty.
 */
inline int pieceAt(Board* b, int sq) {

	for (int i = PAWN; i <= KING; i++) {
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
inline getKingSquare(Board* b, Color color) {
	Assert(getPieces(b, KING, color));
	return getLSB(getPieces(b, KING, color));
}





/**
 * Calculates all pinners towards given king square.
 *
 * @param  b Board to call function.
 * @param  kSq   Square (of attacked king).
 * @param  kSide Side of attacked king.
 *
 * @returns Bitboard revealing all pinners.
 */
Bitboard getPinner(Board* b, int kSq, Color kSide);

/**
 * Generate bitboard with pinned pieces according to king square and king side. Includes rook
 * and bishop moves.
 *
 * @param  b Board to call function.
 * @param  kSq  Square of king.
 * @param  side Side of pinned pieces (king side).
 *
 * @returns A Bitboard.
 */
Bitboard getPinned(Board* b, int kSq, Color side);

/**
 * @brief Return rooks/bishops that deliver an discovered attack towards given sq.
 * 
 * @param b 
 * @param sq Target square that is under discovered attacked.
 * @param color Color of piece that is potentially attacked.
 */
Bitboard getDiscoveredAttacks(Board* b, int sq, Color color);


/**
 * Generate bitboard of attacked squares and pieces by given side.
 *
 * @param  b Board to call function.
 * @param  color Attacker side.
 *
 * @returns Bitboard with attacked squares and pieces set.
 */
Bitboard attackerSet(Board* b, Color color);

template<PieceType pType>
Bitboard attackerSetFor(Board* b, Color color);


/**
 * Generate bitboard with containing all pieces that can block the given square. King moves are
 * not included. Bitboard shows origin of pieces that can block.
 *
 * @param  b Board to call function.
 * @param  color    Side of blocking pieces.
 * @param  blockSq Square that has to be occupied.
 *
 * @returns Bitboard with blocker pieces.
 */
Bitboard blockerSet(Board* b, Color color, int blockSq);

/**
 * Check if given side attacks given square.
 *
 * @param  b Board to call function.
 * @param  square Square to check attacks on.
 * @param  color   Side that might attack the square.
 *
 * @returns A Bitboard.
 */
Bitboard squareAttackedBy(Board* b, int square, Color color);

/**
 * Get all pieces attacking the given square, independet of side.
 *
 * @param  b Board to call function.
 * @param  square Square to check.
 *
 * @returns Bitboard with attackers / defenders.
 */
Bitboard squareAtkDef(Board* b, int square);

/**
 * Get all pieces attacking the given square, independent of side.
 * Extra parameter to pass the occupied squares needed or slider generation.
 *
 * @param  b Board to call function.
 * @param  occupied Bitboard of currently occupied squares
 * @param  square Square to check.
 *
 * @returns Bitboard with attackers / defenders.
 */
Bitboard squareAtkDefOcc(Board* b, Bitboard occupied, int square);

/**
 * @brief Return all pawns of given color that have an opposite colored pawn ahead.
 * 
 * @return Bitboard 
 */
Bitboard getBlockedPawns(Board* b, Color color);

/**
 * @brief Return all passed pawns (no def pawns ahead) for given color.
 */
Bitboard getPassers(Board* b, Color color);

/**
 * @brief Print bitboard representation of board to console.
 */
void printBoard(Board* board);

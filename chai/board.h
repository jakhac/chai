#pragma once

#include <stack>
#include <bitset>
#include <iostream>

#include "defs.h"
#include "validate.h"
#include "info.h"
#include "attacks.h"
#include "move.h"
#include "pieceKeys.h"
#include "validate.h"


/**
* Stores up to 2 killer moves for each ply.
*/
extern move_t killer[2][MAX_GAME_MOVES];

/**
* Stores up mate killer moves for each ply.
*/
extern move_t mateKiller[MAX_GAME_MOVES];

/**
* Stores history heuristic for both sides with [PIECE][TO] indices.
*/
extern int histHeuristic[2][64][64];

/**
* Stores maximum history score.
*/
extern int histMax;

/**
* Store counter moves for FROM and TO square of the previous move.
*/
extern move_t counterHeuristic[64][64][2];

/**
 * Count major pieces (Piece::n, b, Piece::r, Piece::q, Piece::k) on current board
 *
 * @param  b board_t to call function.
 * @param  side Side of pieces.
 *
 * @returns Amount of major pieces.
 */
int countMajorPieces(board_t* b, int side);

/**
 * Push a null move the null.
 */
void pushNull(board_t* b);

/**
 * Set bit at given index to 0 in side, occupied and piece bitboard.
 *
 * @param  b board_t to call function.
 * @param  piece  Piece index.
 * @param  square Square to clear piece on.
 * @param  side   Color of cleared piece.
 */
void clearPiece(board_t* b, int piece, int square, int side);

/**
 * Set bit at given index to 1 in side, occupied and piece bitboard.
 *
 * @param  b board_t to call function.
 * @param  piece  Piece index.
 * @param  square Square to set piece on.
 * @param  side   Color of set piece.
 */
void setPiece(board_t* b, int piece, int square, int side);

/**
 * Reset board variables to default values.
 */
void reset(board_t* b);

/**
 * @deprecated pieceKeys.h already stores fixed values
 * Initialize hash keys for zobristkey generation.
 */
void initHashKeys(board_t* b); // TODO

/**
 * Generate a unique zobristKey for current board.
 *
 * @param  b board_t to call function.
 *
 * @returns Unique 64-bit number.
 */
key_t generateZobristKey(board_t* b);

/**
 * Generate a unique pawn key for current board.
 *
 * @returns Unique 64-bit number.
 */
key_t generatePawnHashKey(board_t* b);

/**
 * Get pieces of given index and color.
 *
 * @param  b board_t to call function.
 * @param  piece Piece index.
 * @param  side  Color of piece.
 *
 * @returns Bitboard of piece and color.
 */
bitboard_t getPieces(board_t* b, int piece, int side);

/**
 * Determines the piece index at given square.
 *
 * @param  b board_t to call function.
 * @param  square Square.
 *
 * @returns Piece index or zero if empty.
 */
int pieceAt(board_t* b, int square);

/**
 * Calculates all pinners towards given king square.
 *
 * @param  b board_t to call function.
 * @param  kSq   Square (of attacked king)
 * @param  kSide Side of attacked king.
 *
 * @returns Bitboard revealing all pinners.
 */
bitboard_t getPinner(board_t* b, int kSq, int kSide);

/**
 * Generate bitboard with pinned pieces according to king square and king side. Includes rook
 * and bishop moves.
 *
 * @param  b board_t to call function.
 * @param  kSq  Square of king.
 * @param  side Side of pinned pieces (king side)
 *
 * @returns A bitboard_t.
 */
bitboard_t getPinned(board_t* b, int kSq, int side);

/**
 * Get king of given side as square index.
 *
 * @param  b board_t to call function.
 * @param  side Side of king.
 *
 * @returns Index of king.
 */
int getKingSquare(board_t* b, int side);

/**
 * Removes both castle rights for given side.
 *
 * @param  b board_t to call function.
 * @param  side Color.
 */
void clearCastlePermission(board_t* b, int side);

/**
 * Parse a given fen into board variables and print board. Updates zobristKey of board to new
 * generated zobristKey.
 *
 * @param  b board_t to call function.
 * @param  fen FEN Notation string of board.
 * @returns bool true if error occured or FEN invalid, else false.
 */
bool parseFen(board_t* b, string fen);

string getFEN(board_t* b);

/**
 * Parse a move into a bit move. Sets flags and captures according to current board state.
 *
 * @param  b board_t to call function.
 * @param  move Move in algebraic notation.
 *
 * @returns int move.
 */
move_t parseMove(board_t* b, string move);

/**
 * Print board with piece chars, rank and file indices.
 */
void printBoard(board_t* b);

/**
 * Check board for valid bitboard entries and board variables.
 *
 * @param  b board_t to call function.
 *
 * @returns True if no issues have been found, else false.
 */
bool checkBoard(board_t* b);

/**
 * Push move onto board. Update castle rights, enPas square, zobristKey and promotions. Pushes
 * undoMove object on undoStack for future undos. Method assumes correct pseudo-move!
 * <para>If pushed move leaves the moving side in check, move is popped from stack.</para>
 *
 * @param  b board_t to call function.
 * @param  move int move.
 *
 * @returns Returns true if move was valid and does not leave king in check, else false.
 */
bool push(board_t* b, move_t move);

/**
 * Push rooks with castle move on board. Small checks for valid init positions of king and rook.
 *
 * @param  b board_t to call function.
 * @param  clearRookSq Clear rook on this square.
 * @param  setRookSq   Set rook on this square.
 * @param  side		   Color.
 */
void pushCastle(board_t* b, int clearRookSq, int setRookSq, int side);

/**
 * Pops move from move stack and restores enPas square, castlePermission, zobristKey, captures
 * and promotions. Assert for correct zobristKey.
 *
 * @param  b board_t to call function.
 *
 * @returns Undo struct.
 */
undo_t pop(board_t* b);

/**
 * Reverse pushed castle move from board. Resets rook on init square.
 *
 * @param  b board_t to call function.
 * @param  clearRookSq Clear rook on this square.
 * @param  setRookSq   Set rook on this square.
 * @param  side		   Color.
 */
void popCastle(board_t* b, int clearRookSq, int setRookSq, int side);

/**
 * Generate bitboard of attacked squares and pieces by given side.
 *
 * @param  b board_t to call function.
 * @param  side Attacker side.
 *
 * @returns Bitboard with attacked squares and pieces set.
 */
bitboard_t attackerSet(board_t* b, int side);

/**
 * Generate bitboard with containing all pieces that can block the given square. King moves are
 * not included. Bitboard shows origin of pieces that can block.
 *
 * @param  b board_t to call function.
 * @param  side    Side of blocking pieces.
 * @param  blockSq Square that has to be occupied.
 *
 * @returns Bitboard with blocker pieces.
 */
bitboard_t blockerSet(board_t* b, int side, int blockSq);

/**
 * Check if given side attacks given square.
 *
 * @param  b board_t to call function.
 * @param  square Square to check attacks on.
 * @param  side   Side that might attack the square.
 *
 * @returns A bitboard_t.
 */
bitboard_t squareAttackedBy(board_t* b, int square, int side);

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
 * Get all pieces attacking the given square, independet of side.
 * Extra parameter to pass the occupied squares needed or slider generation.
 *
 * @param  b board_t to call function.
 * @param occupied Bitboard of currently occupied squares
 * @param  square Square to check.
 *
 * @returns Bitboard with attackers / defenders.
 */
bitboard_t squareAtkDefOcc(board_t* b, bitboard_t occupied, int square);

/**
 * Check if given side is currently in check.
 *
 * @param  b board_t to call function.
 * @param  side The side thats possibly in check.
 *
 * @returns Returns the mask of pieces giving check.
 */
bool isCheck(board_t* b, int side);

/**
 * Check if castle move is valid: castle permission, current check, empty squares between rook
 * and king, attack squares between rook and king.
 *
 * @param  b board_t to call function.
 * @param  castle	   Castle bit from board variable.
 * @param  attackerSet Set the attacker belongs to.
 *
 * @returns True if castling move is valid, else false.
 */
bool castleValid(board_t* b, int castle, bitboard_t* attackerSet);

/**
 * Check if potBlockerSq is blocking an attack to kSq by the discoverSide.
 *
 * @param  b board_t to call function.
 * @param  kSq king getting checked by discovered attack
 * @param  discoverSide Side to move, moves own piece with pot discovered check
 * @param  potBlockerSq sq that potentially discovers a check when piece moves.
 *
 * @returns True if moving potBlockerSq results in a discovered check to KSq.
 */
bool sqIsBlockerForKing(board_t* b, int kSq, int discoverSide, int potBlockerSq);

/**
 * Check before making the move if move gives check.
 * @param  b board_t to call function.
 * @param  move
 */
bool checkingMove(board_t* b, move_t move);







///////////////////////////////////////////////////////////////////////

/*
Move format is bits in hex
0000 0000 0000 0000 0000 0000 0001 -> 0x1
0000 0000 0000 0000 0000 0000 1111 -> 0xF

0000 0000 0000 0000 0000 0111 1111 -> From square bits
0000 0000 0000 0011 1111 1000 0000 -> To square bits >> 7
0000 0000 0011 1100 0000 0000 0000 -> captured piece >> 14
0000 0000 0100 0000 0000 0000 0000 -> ep bool
0000 0000 1000 0000 0000 0000 0000 -> pawn start
0000 1111 0000 0000 0000 0000 0000 -> Promoted piece >> 20
0001 0000 0000 0000 0000 0000 0000 -> castling bool 0x1000000
0010 0000 0000 0000 0000 0000 0000 -> prom flag 0x2000000
*/

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

inline int capPiece(board_t* b, move_t move) {
	return pieceAt(b, toSq(move));
}

inline bool isCapture(board_t* b, move_t move) {
	return pieceAt(b, toSq(move));
}

inline bool isPromotion(move_t move) {
	return (move & (3 << 12)) == PROM_MOVE;
}

inline bool isCaptureOrPromotion(board_t* b, move_t move) {
	return isCapture(b, move) || isPromotion(move);
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

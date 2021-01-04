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

/**
 * Board uses 8 bitboards to store each piece and squares occupied by each color. A chess game
 * should only uses one board instance since various push/pop moves in recursive search trees do
 * not require new boards.
 */
class Board {
public:

	/**
	 * Current side, 0 for black and 1 for white. Use enums for debug purpose.
	 */
	int side;

	/**
	 * Current en passant square. 0, if not set.
	 */
	int enPas = 0;

	/**
	 * Ply Counter.
	 */
	int ply = 0;

	/**
	 * Ply Counter for undoHistory array.
	 */
	int undoPly = 0;

	/**
	 * Fifty-move rule counter. Resets after captures.
	 */
	int fiftyMove = 0;

	/**
	 * Count half moves. Increment when push or pushNull, decrement when pop.
	 */
	int halfMoves = 0;

	/**
	 * CastlePermission stored as number between 0 and 15 (4 bits for each side and color).
	 */
	int castlePermission = 0;

	/**
	 * Unique zobrist key.
	 */
	bitboard_t zobristKey = 0x0;

	/**
	 * Unique zobrist pawn key.
	 */
	bitboard_t zobristPawnKey = 0x0;

	/**
	 * Store pieces for given color.
	 */
	bitboard_t color[2] = { 0ULL, 0ULL };

	/**
	 * Store pieces for given type.
	 */
	bitboard_t pieces[7] = { EMPTY, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL };

	/**
	 * Store occupied squares.
	 */
	bitboard_t occupied = 0ULL;

	/**
	 * Stores the currently attacked squares by side.
	 */
	bitboard_t attackedSquares[2] = { 0ULL, 0ULL };

	/**
	 * Castle keys. // TODO use fixed keys
	 */
	bitboard_t castleKeys[16];

	/**
	 * Stack stores pushed moves as Undo objects.
	 */
	undo_t undoHistory[MAX_GAME_MOVES];

	/**
	 * Transposition table.
	 */
	ttable_t tt[1];

	/**
	* Pawn hash table.
	*/
	pawntable_t pawnTable[1];

	/**
	* Stores the pv line.
	*/
	move_t pvArray[MAX_DEPTH];

	/**
	* Stores up to 2 killer moves for each ply.
	*/
	move_t killer[2][MAX_GAME_MOVES];

	/**
	* Stores up mate killer moves for each ply.
	*/
	move_t mateKiller[MAX_GAME_MOVES];

	/**
	* Stores history heuristic for both sides with [PIECE][TO] indices.
	*/
	int histHeuristic[13][64];

	/**
	* Stores maximum history score.
	*/
	int histMax = 0;

	/**
	* Store counter moves for FROM and TO square of the previous move.
	*/
	move_t counterHeuristic[64][64][2];

	move_t principle_variation[MAX_DEPTH][MAX_DEPTH];
	int length_of_variation[MAX_DEPTH];
	int quiescence_depth_of_variation[MAX_DEPTH];
	bool hash_hit[MAX_DEPTH];

	void backup_principle_variation(Board* game, int depth, move_t move);

	/**
	 * Count major pieces (n, b, r, q, k) on current board
	 *
	 * @param  side Side of pieces.
	 *
	 * @returns Amount of major pieces.
	 */
	int countMajorPieces(int side);


	/**
	 * Push a null move the null.
	 */
	void pushNull();

	/**
	 * Set bit at given index to 0 in side, occupied and piece bitboard.
	 *
	 * @param  piece  Piece index.
	 * @param  square Square to clear piece on.
	 * @param  side   Color of cleared piece.
	 */
	void clearPiece(int piece, int square, int side);

	/**
	 * Set bit at given index to 1 in side, occupied and piece bitboard.
	 *
	 * @param  piece  Piece index.
	 * @param  square Square to set piece on.
	 * @param  side   Color of set piece.
	 */
	void setPiece(int piece, int square, int side);

	/**
	 * Reset board variables to default values.
	 */
	void reset();

	/**
	 * Initialize hash keys for zobristkey generation. // TODO depreicated?
	 */
	void initHashKeys();

	/**
	 * Generate a unique zobristKey for current board.
	 *
	 * @returns Unique 64-bit number.
	 */
	bitboard_t generateZobristKey();

	/**
	 * Generate a unique pawn key for current board.
	 *
	 * @returns Unique 64-bit number.
	 */
	bitboard_t generatePawnHashKey();

	/**
	 * Get pieces of given index and color.
	 *
	 * @param  piece Piece index.
	 * @param  side  Color of piece.
	 *
	 * @returns Bitboard of piece and color.
	 */
	bitboard_t getPieces(int piece, int side);

	/**
	 * Determines the piece index at given square.
	 *
	 * @param  square Square.
	 *
	 * @returns Piece index or zero if empty.
	 */
	int pieceAt(int square);

	/**
	 * Calculates all pinners towards given king square.
	 *
	 * @param  kSq   Square (of attacked king)
	 * @param  kSide Side of attacked king.
	 *
	 * @returns Bitboard revealing all pinners.
	 */
	bitboard_t pinner(int kSq, int kSide);

	/**
	 * Generate bitboard with pinned pieces according to king square and king side. Includes rook
	 * and bishop moves.
	 *
	 * @param  kSq  Square of king.
	 * @param  side Side of pinned pieces (king side)
	 *
	 * @returns A bitboard_t.
	 */
	bitboard_t pinned(int kSq, int side);

	/**
	 * Get king of given side as square index.
	 *
	 * @param  side Side of king.
	 *
	 * @returns Index of king.
	 */
	int getKingSquare(int side);

	/**
	 * Removes both castle rights for given side.
	 *
	 * @param  side Color.
	 */
	void clearCastlePermission(int side);

	/**
	 * Parse a given fen into board variables and print board. Updates zobristKey of board to new
	 * generated zobristKey.
	 *
	 * @param  fen FEN Notation string of board.
	 */
	void parseFen(string fen);

	/**
	 * Parse a move into a bit move. Sets flags and captures according to current board state.
	 *
	 * @param  move Move in algebraic notation.
	 *
	 * @returns int move.
	 */
	int parseMove(string move);

	/**
	 * Print board with piece chars, rank and file indices.
	 */
	void printBoard();

	/**
	 * Check board for valid bitboard entries and board variables.
	 *
	 * @returns True if no issues have been found, else false.
	 */
	bool checkBoard();

	/**
	 * Push move onto board. Update castle rights, enPas square, zobristKey and promotions. Pushes
	 * undoMove object on undoStack for future undos. Method assumes correct pseudo-move!
	 * <para>If pushed move leaves the moving side in check, move is popped from stack.</para>
	 *
	 * @param  move int move.
	 *
	 * @returns Returns true if move was valid and does not leave king in check, else false.
	 */
	bool push(int move);

	/**
	 * Push rooks with castle move on board. Small checks for valid init positions of king and rook.
	 *
	 * @param  clearRookSq Clear rook on this square.
	 * @param  setRookSq   Set rook on this square.
	 * @param  side		   Color.
	 */
	void pushCastle(int clearRookSq, int setRookSq, int side);

	/**
	 * Pops move from move stack and restores enPas square, castlePermission, zobristKey, captures
	 * and promotions. Assert for correct zobristKey.
	 *
	 * @returns Undo struct.
	 */
	undo_t pop();

	/**
	 * Reverse pushed castle move from board. Resets rook on init square.
	 *
	 * @param  clearRookSq Clear rook on this square.
	 * @param  setRookSq   Set rook on this square.
	 * @param  side		   Color.
	 */
	void popCastle(int clearRookSq, int setRookSq, int side);

	/**
	 * Generate bitboard of attacked squares and pieces by given side.
	 *
	 * @param  side Attacker side.
	 *
	 * @returns Bitboard with attacked squares and pieces set.
	 */
	bitboard_t attackerSet(int side);

	/**
	 * Generate bitboard with containing all pieces that can block the given square. King moves are
	 * not included. Bitboard shows origin of pieces that can block.
	 *
	 * @param  side    Side of blocking pieces.
	 * @param  blockSq Square that has to be occupied.
	 *
	 * @returns Bitboard with blocker pieces.
	 */
	bitboard_t blockerSet(int side, int blockSq);

	/**
	 * Check if given side attacks given square.
	 *
	 * @param  square Square to check attacks on.
	 * @param  side   Side that might attack the square.
	 *
	 * @returns A bitboard_t.
	 */
	bitboard_t squareAttackedBy(int square, int side);

	/**
	 * Get all pieces attacking the given square, independet of side.
	 *
	 * @param  square Square to check.
	 *
	 * @returns Bitboard with attackers / defenders.
	 */
	bitboard_t squareAtkDef(int square);

	/**
	 * Check if given side is currently in check.
	 *
	 * @param  side The side thats possibly in check.
	 *
	 * @returns Returns the mask of pieces giving check.
	 */
	bool isCheck(int side);

	/**
	 * Deprecated.
	 */
	bool leavesKingInCheck(Board* b, const move_t move, const bool inCheck);

	/**
	 * Check if castle move is valid: castle permission, current check, empty squares between rook
	 * and king, attack squares between rook and king.
	 *
	 * @param  castle	   Castle bit from board variable.
	 * @param  attackerSet Set the attacker belongs to.
	 *
	 * @returns True if castling move is valid, else false.
	 */
	bool castleValid(int castle, bitboard_t* attackerSet);

};
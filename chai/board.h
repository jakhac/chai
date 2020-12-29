#pragma once

#include <stack>
#include <bitset>
#include <iostream>

#include "defs.h"
#include "validate.h"
#include "undo.h"
#include "info.h"
#include "attacks.h"
#include "move.h"
#include "pieceKeys.h"

/// <summary>
/// Board uses 8 bitboards to store each piece and squares occupied by each color. A chess game should only uses 
/// one board instance since various push/pop moves in recursive search trees do not require new boards.
/// </summary>
class Board {
public:

	/// <summary>Current side, 0 for black and 1 for white. Use enums for debug purpose.</summary>
	int side;

	/// <summary>Current en passant square. 0, if not set.</summary>
	int enPas = 0;

	/// <summary>Ply Counter</summary>
	int ply = 0;

	/// <summary>Ply Counter for undoHistory array</summary>
	int undoPly = 0;

	/// <summary>Fifty-move rule counter. Resets after captures.</summary>
	int fiftyMove = 0;

	/// <summary>Count half moves. Increment when push or pushNull, decrement when pop.</summary>
	int halfMoves = 0;

	/// <summary>CastlePermission stored as number between 0 and 15 (4 bits for each side and color)</summary>
	int castlePermission = 0;

	/// <summary>Unique zobrist key</summary>
	bitboard_t zobristKey = 0x0;

	/// <summary>Unique zobrist pawn key</summary>
	bitboard_t zobristPawnKey = 0x0;

	/// <summary>Store pieces for given color</summary>
	bitboard_t color[2] = { 0ULL, 0ULL };

	/// <summary>Store pieces for given type</summary>
	bitboard_t pieces[7] = { EMPTY, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL };

	/// <summary>Store occupied squares</summary>
	bitboard_t occupied = 0ULL;

	/// <summary>Stores the currently attacked squares by side</summary>
	bitboard_t attackedSquares[2] = { 0ULL, 0ULL };

	/// <summary>Unique 64bit number for each piece on each square. En pas key is stored as piece 0.</summary>
	//bitboard_t pieceKeys[13][64];

	/// <summary>Castle keys</summary>
	bitboard_t castleKeys[16];

	/// <summary>Side key for color change. Hash into zobristKey if white.</summary>
	//bitboard_t sideKey;

	/// <summary>Stack stores pushed moves as Undo objects.</summary>
	undo_t undoHistory[MAX_GAME_MOVES];

	//<summary>Transposition table</summary>
	ttable_t tt[1];

	pawntable_t pawnTable[1];

	/// <summary>Stores the pv line</summary>
	move_t pvArray[MAX_DEPTH];

	/// <summary>Stores up to 2 killer moves for each ply</summary>
	move_t killer[2][MAX_GAME_MOVES];

	/// <summary>Stores up mate killer moves for each ply</summary> 
	move_t mateKiller[MAX_GAME_MOVES];

	/// <summary>Stores history heuristic for both sides with [PIECE][TO] indices </summary>
	int histHeuristic[13][64];

	/// <summary>Stores maximum history score</summary>
	int histMax = 0;

	/// <summary>Store counter moves for FROM and TO square of the previous move</summary>
	move_t counterHeuristic[64][64][2];

	/// <summary>
	/// Count major pieces (n, b, r, q, k) on current board
	/// </summary>
	/// <param name="side">Side of pieces</param>
	/// <returns>Amount of major pieces</returns>
	int countMajorPieces(int side);

	void pushNull();

	/// <summary>
	/// Set bit at given index to 0 in side, occupied and piece bitboard.
	/// </summary>
	/// <param name="piece">Piece index</param>
	/// <param name="square">Square to clear piece on</param>
	/// <param name="side">Color of cleared piece</param>
	void clearPiece(int piece, int square, int side);

	/// <summary>
	/// Set bit at given index to 1 in side, occupied and piece bitboard.
	/// </summary>
	/// <param name="piece">Piece index</param>
	/// <param name="square">Square to set piece on</param>
	/// <param name="side">Color of set piece</param>
	void setPiece(int piece, int square, int side);

	/// <summary>
	/// Reset board variables to default values.
	/// </summary>
	void reset();

	/// <summary>
	/// Initialize hash keys for zobristkey generation.
	/// </summary>
	void initHashKeys();

	/// <summary>
	/// Generate a unique zobristKey for current board.
	/// </summary>
	/// <returns>Unique 64-bit number</returns>
	bitboard_t generateZobristKey();

	/// <summary>
	/// Generate a unique pawn key for current board.
	/// </summary>
	/// <returns>Unique 64-bit number</returns>
	bitboard_t generatePawnHashKey();

	/// <summary>
	/// Get pieces of given index and color.
	/// </summary>
	/// <param name="piece">Piece index</param>
	/// <param name="side">Color of piece</param>
	/// <returns>Bitboard of piece and color</returns>
	bitboard_t getPieces(int piece, int side);

	/// <summary>
	/// Determines the piece index at given square.
	/// </summary>
	/// <param name="square">Square</param>
	/// <returns>Piece index or zero if empty</returns>
	int pieceAt(int square);

	/// <summary>
	/// Calculates all pinners towards given king square.
	/// </summary>
	/// <param name="kSq">Square (of attacked king)</param>
	/// <param name="kSide">Side of attacked king</param>
	/// <returns>Bitboard revealing all pinners</returns>
	bitboard_t pinner(int kSq, int kSide);

	/// <summary>
	/// Generate bitboard with pinned pieces according to king square and king side. Includes rook
	/// and bishop moves.
	/// </summary>
	/// <param name="kSq">Square of king</param>
	/// <param name="side">Side of pinned pieces (king side)</param>
	/// <returns></returns>
	bitboard_t pinned(int kSq, int side);

	/// <summary>
	/// Get king of given side as square index.
	/// </summary>
	/// <param name="side">Side of king</param>
	/// <returns>Index of king</returns>
	int getKingSquare(int side);

	/// <summary>
	/// Removes both castle rights for given side.
	/// </summary>
	/// <param name="side">Color</param>
	void clearCastlePermission(int side);

	/// <summary>
	/// Parse a given fen into board variables and print board. Updates zobristKey of board
	/// to new generated zobristKey.
	/// </summary>
	/// <param name="fen">FEN Notation string of board</param>
	void parseFen(string fen);

	/// <summary>
	/// Parse a move into a bit move. Sets flags and captures according to current board state.
	/// </summary>
	/// <param name="move">Move in algebraic notation</param>
	/// <returns>int move</returns>
	int parseMove(string move);

	/// <summary>
	/// Print board with piece chars, rank and file indices.
	/// </summary>
	void printBoard();

	/// <summary>
	/// Check board for valid bitboard entries and board variables.
	/// </summary>
	int checkBoard();

	/// <summary>
	/// Push move onto board. Update castle rights, enPas square, zobristKey and promotions.
	/// Pushes undoMove object on undoStack for future undos. Method assumes correct pseudo-move!
	/// <para>If pushed move leaves the moving side in check, move is popped from stack.</para>
	/// </summary>
	/// <param name="move">int move</param>
	/// <returns>Returns true if move was valid and does not leave king in check, else false</returns>
	bool push(int move);

	/// <summary>
	/// Push rooks with castle move on board. Small checks for valid init positions of king and rook.
	/// </summary>
	/// <param name="clearRookSq">Clear rook on this square</param>
	/// <param name="setRookSq">Set rook on this square</param>
	/// <param name="side">Color</param>
	/// <see cref="push()"/>
	void pushCastle(int clearRookSq, int setRookSq, int side);

	/// <summary>
	/// Pops move from move stack and restores enPas square, castlePermission, zobristKey, 
	/// captures and promotions. Assert for correct zobristKey.
	/// </summary>
	/// <returns>undo struct</returns>
	undo_t pop();

	/// <summary>
	/// Reverse pushed castle move from board. Resets rook on init square.
	/// </summary>
	/// <param name="clearRookSq">Clear rook on this square</param>
	/// <param name="setRookSq">Set rook on this square</param>
	/// <param name="side">Color</param>
	/// <see cref="pop()"/>
	void popCastle(int clearRookSq, int setRookSq, int side);

	/// <summary>
	/// Generate bitboard of attacked squares and pieces by given side.
	/// </summary>
	/// <param name="side">Attacker side</param>
	/// <returns>Bitboard with attacked squares and pieces set</returns>
	bitboard_t attackerSet(int side);

	/// <summary>
	/// Generate bitboard with containing all pieces that can block the given square.
	/// King moves are not included. Bitboard shows origin of pieces that can block.
	/// </summary>
	/// <param name="side">Side of blocking pieces</param>
	/// <param name="blockSq">Square that has to be occupied</param>
	/// <returns>Bitboard with blocker pieces</returns>
	bitboard_t blockerSet(int side, int blockSq);

	/// <summary>
	/// Check if given side attacks given square.
	/// </summary>
	/// <param name="square">Square to check attacks on</param>
	/// <param name="side">Side that might attack the square</param>
	/// <returns></returns>
	bitboard_t squareAttackedBy(int square, int side);

	/// <summary>
	/// Get all pieces attacking the given square, independet of side.
	/// </summary>
	/// <param name="square">Square to check</param>
	/// <returns>Bitboard with attackers / defenders</returns>
	bitboard_t squareAtkDef(int square);

	/// <summary>
	/// Check if given side is currently in check.
	/// </summary>
	/// <param name="side">Side to check state</param>
	/// <returns>Returns the mask of pieces giving check</returns>
	bitboard_t isCheck(int side);

	/// <summary>
	/// Check if castle move is valid: castle permission, current check, empty squares
	/// between rook and king, attack squares between rook and king.
	/// </summary>
	/// <param name="castle">Castle bit from board variable</param>
	/// <returns>True if castling move is valid, else false</returns>
	bool castleValid(int castle, bitboard_t* attackerSet);

};
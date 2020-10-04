#pragma once

#include <stack>
#include <bitset>
#include <iostream>

#include "defs.h"
#include "validate.h"
#include "undo.h"
#include "rays.h"
#include "info.h"
#include "attacks.h"


/*
* - Pawn Shield Pattern for king safety
* - Distance to king for safety
*/

/// <summary>
/// Board uses 8 bitboards to store each piece and squares occupied by each color. A chess game should only uses 
/// one board instance since push/pop moves in recursive ssearch trees do not require new boards.
/// </summary>
class Board {
public:

	/// <summary>Current side, 0 for black and 1 for white. Use enums for debug purpose.</summary>
	int side;

	/// <summary>Current en passant square. 0, if not set.</summary>
	int enPas = 0;

	/// <summary>Ply Counter</summary>
	int ply = 0;

	/// <summary>Fifty-move rule counter. Resets after captures.</summary>
	int fiftyMove = 0;

	/// <summary>CastlePermission stored as number between 0 and 15 (4 bits for each side and color)</summary>
	int castlePermission = 0;

	/// <summary>Unique zobrist key</summary>
	U64 zobristKey = 0x0;

	/// <summary>Store pieces for given color</summary>
	U64 color[2] = { 0ULL, 0ULL };

	/// <summary>Store pieces for given type</summary>
	U64 pieces[7] = { EMPTY, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL };

	/// <summary>Store occupied squares</summary>
	U64 occupied = 0ULL;

	/// <summary>Stores the currently attacked squares both side</summary>
	U64 attackedSquares[2] = { 0ULL, 0ULL };

	/// <summary>Unique 64bit number for each piece on each square. En pas key is stored as piece 0.</summary>
	U64 pieceKeys[13][64];

	/// <summary>Castle keys</summary>
	U64 castleKeys[16];

	/// <summary>Side key for color change. Hash into zobristKey if white.</summary>
	U64 sideKey;

	/// <summary>Stack stores pushed moves as Undo objects.</summary>
	stack <Undo> undoStack;

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
	/// Generate a unique zobristKey for this board.
	/// </summary>
	/// <returns>Unique 64-bit number</returns>
	U64 generateZobristKey();

	/// <summary>
	/// Get pieces of given index and color.
	/// </summary>
	/// <param name="piece">Piece index</param>
	/// <param name="side">Color of piece</param>
	/// <returns>Bitboard of piece and color</returns>
	U64 getPieces(int piece, int side);

	/// <summary>
	/// Determines the piece index at given square.
	/// </summary>
	/// <param name="square">Square</param>
	/// <returns>Piece index or zero if empty</returns>
	int pieceAt(int square);

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
	/// <returns>undo object popped from move stack</returns>
	Undo pop();

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
	U64 attackerSet(int side);

	/// <summary>
	/// Check if given side attacks given square.
	/// </summary>
	/// <param name="square">Square to check attacks on</param>
	/// <param name="side">Side that might attack the square</param>
	/// <returns></returns>
	bool squareAttacked(int square, int side);

	/// <summary>
	/// Check if given side is currently in check.
	/// </summary>
	/// <param name="side">Side to check state</param>
	/// <returns>Return true if side is in check, else false</returns>
	bool isCheck(int side);

	/// <summary>
	/// Check if castle move is valid: castle permission, current check, empty squares
	/// between rook and king, attack squares between rook and king.
	/// </summary>
	/// <param name="castle">Castle bit from board variable</param>
	/// <returns>True if castling move is valid, else false</returns>
	bool castleValid(int castle);

};


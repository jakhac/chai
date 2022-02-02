#pragma once

#include <bitset> // printBinary

#include "defs.h"
#include "info.h"
#include "attacks.h"
#include "move.h"
#include "pieceKeys.h"
#include "validate.h"
#include "psqt.h"
#include "bitboard.h"


enum CASTLING_RIGHTS {
	K_CASTLE = 1 << 0,
	Q_CASTLE = 1 << 1,
	k_CASTLE = 1 << 2,
	q_CASTLE = 1 << 3
};


const int piecePawn[13]        = { 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 };
const int pieceKnight[13]      = { 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 };
const int pieceRook[13]        = { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0 };
const int pieceRookQueen[13]   = { 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0 };
const int pieceBishopQueen[13] = { 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0 };
const int pieceKing[13]        = { 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1 };


const int stmPiece[7][2] = { 
	{Piece::NO_PIECE, Piece::NO_PIECE},
	{Piece::p, Piece::P},
	{Piece::n, Piece::N},
	{Piece::b, Piece::B},
	{Piece::r, Piece::R},
	{Piece::q, Piece::Q},
	{Piece::k, Piece::K},
};

const color_t pieceCol[13] = { BW,
	WHITE, WHITE, WHITE, WHITE, WHITE, WHITE,
	BLACK, BLACK, BLACK, BLACK, BLACK, BLACK
};

const std::string pieceChar = ".PNBRQKpnbrqk";
const std::string sideChar  = "wb-";
const std::string rankChar  = "12345678";
const std::string fileChar  = "abcdefgh";


inline int fileRankToSq(int f, int r) {
	return 8 * r + f;
}

inline bool hasBishopOrKnight(board_t* b, color_t color) {
	return (b->pieces[cBISHOP] & b->color[color]) 
		|| (b->pieces[cKNIGHT] & b->color[color]);
}

template<color_t color>
inline int relSeventh() {
	return (color == WHITE) ? RANK_7 : RANK_2;
}

inline int relSquare(int sq, color_t color) {
	return (color == WHITE) ? sq : mirror64[sq];
}

bitboard_t getDiagPieces(board_t* b, color_t color);

bitboard_t getVertPieces(board_t* b, color_t color);


/**
 * board instance used for complete program
 */
extern board_t* p_board;

// Maximum score before rescale in history heuristic. Max history score needs to be less than 10000
// because move ordering scores "QUIET_SCORES=5000 + hist/10" before COUNTER_SCORE=6000.
const int HISTORY_MAX = 1000 - 1;

/**
* Store counter moves for FROM and TO square of the previous move.
*/
extern move_t counterHeuristic[64][64][2];

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
void delPiece(board_t* b, int piece, int square, color_t side);

/**
 * Set bit at given index to 1 in side, occupied and piece bitboard.
 *
 * @param  b board_t to call function.
 * @param  piece  Piece index.
 * @param  square Square to set piece on.
 * @param  side   Color of set piece.
 */
void setPiece(board_t* b, int piece, int square, color_t side);



/**
 * Reset board variables to default values.
 */
void reset(board_t* b);

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


int capPiece(board_t* b, move_t move);

bool isCapture(board_t* b, move_t move);

bool isCaptureOrPromotion(board_t* b, move_t move);

/**
 * Get king of given side as square index.
 *
 * @param  b board_t to call function.
 * @param  side Side of king.
 *
 * @returns Index of king.
 */
int getKingSquare(board_t* b, color_t side);

/**
 * Removes both castle rights for given side.
 *
 * @param  b board_t to call function.
 * @param  side Color.
 */
void clearCastlePermission(board_t* b, color_t side);




/**
 * Check board for valid bitboard entries and board variables.
 * 
 * @returns True if no issues have been found, else false.
 */
bool checkBoard(board_t* board);

/**
 * Push move onto board. Update castle rights, enPas square, zobristKey and promotions. Pushes
 * undoMove object on undoStack for future undos. Method assumes correct pseudo-move!
 * <para>If pushed move leaves the moving side in check, move is popped from stack.</para>
 *
 * @param  b board_t to call function.
 * @param  move int move.
 * @param  bool evadingCheck Set to true if this is an evading move. This skips inCheck-
 *				check after move is done.
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
 * @param  color		   Color.
 */
void pushCastle(board_t* b, int clearRookSq, int setRookSq, color_t color);

/**
 * Pops move from move stack and restores enPas square, castlePermission, zobristKey, captures
 * and promotions. Assert for correct zobristKey.
 *
 * @param  b board_t to call function.
 *
 * @returns Undo struct.
 */
undo_t pop(board_t* b);

undo_t popNull(board_t* b);

/**
 * Reverse pushed castle move from board. Resets rook on init square.
 *
 * @param  b board_t to call function.
 * @param  clearRookSq Clear rook on this square.
 * @param  setRookSq   Set rook on this square.
 * @param  color		   Color.
 */
void popCastle(board_t* b, int clearRookSq, int setRookSq, color_t color);


/**
 * Check if given side is currently in check.
 *
 * @param  b board_t to call function.
 * @param  side The side thats possibly in check.
 *
 * @returns Returns the mask of pieces giving check.
 */
bool isCheck(board_t* b, color_t color);

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
 * Check before making the move if move gives check.
 * @param  b board_t to call function.
 * @param  move
 */
bool checkingMove(board_t* b, move_t move);

move_t getCurrentMove(board_t* b);

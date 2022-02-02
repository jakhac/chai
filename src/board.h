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

const Color pieceCol[13] = { BW,
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

inline bool hasBishopOrKnight(Board* b, Color color) {
	return (b->pieces[BISHOP] & b->color[color]) 
		|| (b->pieces[KNIGHT] & b->color[color]);
}

template<Color color>
inline int relSeventh() {
	return (color == WHITE) ? RANK_7 : RANK_2;
}

inline int relSquare(int sq, Color color) {
	return (color == WHITE) ? sq : mirror64[sq];
}

Bitboard getDiagPieces(Board* b, Color color);

Bitboard getVertPieces(Board* b, Color color);


/**
 * board instance used for complete program
 */
extern Board* p_board;

// Maximum score before rescale in history heuristic. Max history score needs to be less than 10000
// because move ordering scores "QUIET_SCORES=5000 + hist/10" before COUNTER_SCORE=6000.
const int HISTORY_MAX = 1000 - 1;

/**
* Store counter moves for FROM and TO square of the previous move.
*/
extern Move counterHeuristic[64][64][2];

/**
 * Push a null move the null.
 */
void pushNull(Board* b);

/**
 * Set bit at given index to 0 in side, occupied and piece bitboard.
 *
 * @param  b Board to call function.
 * @param  piece  Piece index.
 * @param  square Square to clear piece on.
 * @param  side   Color of cleared piece.
 */
void delPiece(Board* b, int piece, int square, Color side);

/**
 * Set bit at given index to 1 in side, occupied and piece bitboard.
 *
 * @param  b Board to call function.
 * @param  piece  Piece index.
 * @param  square Square to set piece on.
 * @param  side   Color of set piece.
 */
void setPiece(Board* b, int piece, int square, Color side);



/**
 * Reset board variables to default values.
 */
void reset(Board* b);

/**
 * Generate a unique zobristKey for current board.
 *
 * @param  b Board to call function.
 *
 * @returns Unique 64-bit number.
 */
Key generateZobristKey(Board* b);

/**
 * Generate a unique pawn key for current board.
 *
 * @returns Unique 64-bit number.
 */
Key generatePawnHashKey(Board* b);


int capPiece(Board* b, Move move);

bool isCapture(Board* b, Move move);

bool isCaptureOrPromotion(Board* b, Move move);

/**
 * Get king of given side as square index.
 *
 * @param  b Board to call function.
 * @param  side Side of king.
 *
 * @returns Index of king.
 */
int getKingSquare(Board* b, Color side);

/**
 * Removes both castle rights for given side.
 *
 * @param  b Board to call function.
 * @param  side Color.
 */
void clearCastlePermission(Board* b, Color side);




/**
 * Check board for valid bitboard entries and board variables.
 * 
 * @returns True if no issues have been found, else false.
 */
bool checkBoard(Board* board);

/**
 * Push move onto board. Update castle rights, enPas square, zobristKey and promotions. Pushes
 * undoMove object on undoStack for future undos. Method assumes correct pseudo-move!
 * <para>If pushed move leaves the moving side in check, move is popped from stack.</para>
 *
 * @param  b Board to call function.
 * @param  move int move.
 * @param  bool evadingCheck Set to true if this is an evading move. This skips inCheck-
 *				check after move is done.
 *
 * @returns Returns true if move was valid and does not leave king in check, else false.
 */
bool push(Board* b, Move move);

/**
 * Push rooks with castle move on board. Small checks for valid init positions of king and rook.
 *
 * @param  b Board to call function.
 * @param  clearRookSq Clear rook on this square.
 * @param  setRookSq   Set rook on this square.
 * @param  color		   Color.
 */
void pushCastle(Board* b, int clearRookSq, int setRookSq, Color color);

/**
 * Pops move from move stack and restores enPas square, castlePermission, zobristKey, captures
 * and promotions. Assert for correct zobristKey.
 *
 * @param  b Board to call function.
 *
 * @returns Undo struct.
 */
Undo pop(Board* b);

Undo popNull(Board* b);

/**
 * Reverse pushed castle move from board. Resets rook on init square.
 *
 * @param  b Board to call function.
 * @param  clearRookSq Clear rook on this square.
 * @param  setRookSq   Set rook on this square.
 * @param  color		   Color.
 */
void popCastle(Board* b, int clearRookSq, int setRookSq, Color color);


/**
 * Check if given side is currently in check.
 *
 * @param  b Board to call function.
 * @param  side The side thats possibly in check.
 *
 * @returns Returns the mask of pieces giving check.
 */
bool isCheck(Board* b, Color color);

/**
 * Check if potBlockerSq is blocking an attack to kSq by the discoverSide.
 *
 * @param  b Board to call function.
 * @param  kSq king getting checked by discovered attack
 * @param  discoverSide Side to move, moves own piece with pot discovered check
 * @param  potBlockerSq sq that potentially discovers a check when piece moves.
 *
 * @returns True if moving potBlockerSq results in a discovered check to KSq.
 */
bool sqIsBlockerForKing(Board* b, int kSq, int discoverSide, int potBlockerSq);

/**
 * Check if castle move is valid: castle permission, current check, empty squares between rook
 * and king, attack squares between rook and king.
 *
 * @param  b Board to call function.
 * @param  castle	   Castle bit from board variable.
 * @param  attackerSet Set the attacker belongs to.
 *
 * @returns True if castling move is valid, else false.
 */
bool castleValid(Board* b, int castle, Bitboard* attackerSet);


/**
 * Check before making the move if move gives check.
 * @param  b Board to call function.
 * @param  move
 */
bool checkingMove(Board* b, Move move);

Move getCurrentMove(Board* b);

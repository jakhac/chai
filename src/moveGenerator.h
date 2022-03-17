#pragma once

#include "board.h"
#include "attacks.h"

/**
 * Generate all moves for current position. Runs check evasions if inCheck is true.
 *
 * @param b
 * @param moveList
 * @param inCheck
 */
void generateMoves(Board* b, MoveList* moveList, bool inCheck);

/**
 * Generate tactical moves for quiescence search. If inCheck, all evading moves are generated.
 *
 * @param b
 * @param moveList
 * @param inCheck
 */
void generateQuiescence(Board* b, MoveList* moveList, bool inCheck);

/**
 * Generate all check evasions. All moves generated here are legal!
 *
 * @param b
 * @param moveList
 */
void generateCheckEvasions(Board* b, MoveList* moveList);

/**
 * Generate all quiet moves delivering check.
 *
 * @param b
 * @param moveList
 */
void generateQuietCheckers(Board* b, MoveList* moveList);

/**
 * Check if a given move is pseudo-legal (might still hang king).
 *
 * @param b
 * @param moveList
 */
bool isLegal(Board* b, const Move move);

/**
 * Check if the current position has a safe pawn push:
 * - Without being captured immediatly after
 * - No obvious blocks or rams in front
 *
 * @param b
 * @param side
 */
Bitboard hasSafePawnPush(Board* b, Color side);

/**
 * Check if a position that is already in check has an evading move.
 *
 * @param b
 */
bool hasEvadingMove(Board* b);

/**
 * Generate all blockers for the blocking square and add them to the moveList. Kings cannot
 * block.
 *
 * @param  b		  Board.
 * @param  moveList   Blocking moves are added to this moveList.
 * @param  blockingSq Square to be blocked.
 */
void addBlockersForSq(Board* b, MoveList* moveList, int blockingSq, Bitboard* pinnedDefenders);

/**
 * Generate all possible single white pawn pushes and add to moveList. Does not include
 * promotions, they are handled in a seperate function.
 *
 * @param  b	    Board.
 * @param  moveList moveList struct to add moves to.
 */
void whiteSinglePawnPush(Board* b, MoveList* moveList);

/**
 * Generate all possible single black pawn pushes and add to moveList. Does not include
 * promotions, they are handled in a seperate function.
 *
 * @param  b	    Board.
 * @param  moveList moveList struct to add moves to.
 */
void blackSinglePawnPush(Board* b, MoveList* moveList);

/**
 * Generate all possible single push white pawn promotions and add to moveList.
 * Only used for quiesenece move generation, since {@li whiteSinglePawnPush} whiteSinglePawnPush
 * already covers both pushes and promotions.
 *
 * @param  b	    Board.
 * @param  moveList moveList struct to add moves to.
 */
void whitePawnPushProm(Board* b, MoveList* moveList);
void whitePawnPushPromQ(Board* b, MoveList* moveList);

/**
 * Generate all possible single push black pawn promotions and add to moveList.
 * Only used for quiesenece move generation, since {@li blackSinglePawnPush} blackSinglePawnPush
 * already covers both pushes and promotions.
 *
 * @param  b	    Board.
 * @param  moveList moveList struct to add moves to.
 */
void blackPawnPushProm(Board* b, MoveList* moveList);
void blackPawnPushPromQ(Board* b, MoveList* moveList);

/**
 * Generate all possible white double pawn pushes and add to moveList.
 *
 * @param  b	    Board.
 * @param  moveList moveList struct to add moves to.
 */
void whiteDoublePawnPush(Board* b, MoveList* moveList);

/**
 * Generate all possible black double pawn pushes and add to moveList.
 *
 * @param  b	    Board.
 * @param  moveList moveList struct to add moves to.
 */
void blackDoublePawnPush(Board* b, MoveList* moveList);

/**
 * Generate all possible white pawn captures and add to moveList EP captures are included.
 *
 * @param  b	    Board.
 * @param  moveList moveList struct to add moves to.
 */
void whitePawnCaptures(Board* b, MoveList* moveList);

/**
 * Generate all possible black pawn captures and add to moveList EP captures are included.
 *
 * @param  b	    Board.
 * @param  moveList moveList struct to add moves to.
 */
void blackPawnCaptures(Board* b, MoveList* moveList);

/**
 * @brief Generate all capturing moves for given piece type.
 */
template<PieceType pType>
void addCapturesFor(Board* b, MoveList* moveList);

/**
 * @brief Generate all non-capturing moves for given piece type.
 */
template<PieceType pType>
void addNonCapturesFor(Board* b, MoveList* moveList);

/**
 * @brief Generate all king moves for given side and add to quietMoveList.
 */
void addKingMoves(Board* b, MoveList* moveList);

/**
 * @brief Generate king captures and add to capMoveList.
 */
void addKingCaptures(Board* b, MoveList* moveList);

/**
 * @brief Generate king check evasions due to double check. Combines quiet and capture moves. Castling
 * is not legal.
 */
void addKingCheckEvasions(Board* b, MoveList* moveList);

/**
 * Print all generated moves.
 *
 * @param  moveList moveList struct with moves.
 */
void printGeneratedMoves(Board* b, MoveList* moveList);

/**
 * @brief Check if this string is a valid move in this position.
 *
 * @param  b	        Board.
 * @param  stringMove   string, we assume a move.
 * @param  move         if string move is valid, the move is copied here.
 */
bool stringIsValidMove(Board* b, std::string stringMove, Move* move);


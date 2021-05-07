#pragma once

#include <vector>

#include "board.h"
#include "attacks.h"

void generateMoves(board_t* b, moveList_t* moveList, bool inCheck);

void generateQuiescence(board_t* b, moveList_t* moveList, bool inCheck);

// generates only legal moves!
void generateCheckEvasions(board_t* b, moveList_t* moveList);

void generateQuietCheckers(board_t* b, moveList_t* moveList);

bool isLegal(board_t* b, const move_t move);

bitboard_t hasSafePawnPush(board_t* b, int side);

bool hasEvadingMove(board_t* b);

/**
 * Generate all blockers for the blocking square and add them to the moveList. Kings cannot
 * block.
 *
 * @param  b		  board_t.
 * @param  moveList   Blocking moves are added to this moveList.
 * @param  blockingSq Square to be blocked.
 */
void addBlockersForSq(board_t* b, moveList_t* moveList, int blockingSq, bitboard_t* pinnedDefenders);

/**
 * Generate all possible single white pawn pushes and add to moveList. Does not include
 * promotions, they are handled in a seperate function.
 *
 * @param  b	    board_t.
 * @param  moveList moveList struct to add moves to.
 */
void whiteSinglePawnPush(board_t* b, moveList_t* moveList);

/**
 * Generate all possible single black pawn pushes and add to moveList. Does not include
 * promotions, they are handled in a seperate function.
 *
 * @param  b	    board_t.
 * @param  moveList moveList struct to add moves to.
 */
void blackSinglePawnPush(board_t* b, moveList_t* moveList);

/**
 * Generate all possible single push white pawn promotions and add to moveList.
 * Only used for quiesenece move generation, since {@li whiteSinglePawnPush} whiteSinglePawnPush
 * already covers both pushes and promotions.
 *
 * @param  b	    board_t.
 * @param  moveList moveList struct to add moves to.
 */
void whitePawnPushProm(board_t* b, moveList_t* moveList);

/**
 * Generate all possible single push black pawn promotions and add to moveList.
 * Only used for quiesenece move generation, since {@li blackSinglePawnPush} blackSinglePawnPush
 * already covers both pushes and promotions.
 *
 * @param  b	    board_t.
 * @param  moveList moveList struct to add moves to.
 */
void blackPawnPushProm(board_t* b, moveList_t* moveList);

/**
 * Generate all possible white double pawn pushes and add to moveList.
 *
 * @param  b	    board_t.
 * @param  moveList moveList struct to add moves to.
 */
void whiteDoublePawnPush(board_t* b, moveList_t* moveList);

/**
 * Generate all possible black double pawn pushes and add to moveList.
 *
 * @param  b	    board_t.
 * @param  moveList moveList struct to add moves to.
 */
void blackDoublePawnPush(board_t* b, moveList_t* moveList);

/**
 * Generate all possible white pawn captures and add to moveList EP captures are included.
 *
 * @param  b	    board_t.
 * @param  moveList moveList struct to add moves to.
 */
void whitePawnCaptures(board_t* b, moveList_t* moveList);

/**
 * Generate all possible black pawn captures and add to moveList EP captures are included.
 *
 * @param  b	    board_t.
 * @param  moveList moveList struct to add moves to.
 */
void blackPawnCaptures(board_t* b, moveList_t* moveList);

/**
 * Generate all possible knight moves and add to quietMoveList.
 *
 * @param  b	    board_t.
 * @param  moveList moveList struct to add moves to.
 */
void addKnightMoves(board_t* b, moveList_t* moveList);

/**
 * Generate all possible knight captures for given side and add to capMoveList.
 *
 * @param  b	    board_t.
 * @param  moveList moveList struct to add moves to.
 */
void addKnightCaptures(board_t* b, moveList_t* moveList);

/**
 * /// <summary>
 * Generate all king moves for given side and add to quietMoveList.
 * </summary>
 *
 * @param  b	    board_t.
 * @param  moveList moveList struct to add moves to.
 */
void addKingMoves(board_t* b, moveList_t* moveList);

/**
 * Generate king captures and add to capMoveList.
 *
 * @param  b	    board_t.
 * @param  moveList moveList struct to add moves to.
 */
void addKingCaptures(board_t* b, moveList_t* moveList);

/**
 * Generate king check evasions due to double check. Combines quiet and capture moves. Castling
 * is not legal.
 *
 * @param  b	    board_t.
 * @param  moveList moveList struct to add moves to.
 */
void addKingCheckEvasions(board_t* b, moveList_t* moveList);

/**
 * Generate all rook moves for given board and side and add to quietMoveList.
 *
 * @param  b	    board_t to search moves on.
 * @param  moveList moveList struct to add moves to.
 */
void addRookMoves(board_t* b, moveList_t* moveList);

/**
 * Generate all bishop moves for given board and side and add to quietMoveList.
 *
 * @param  b	    board_t to search moves on.
 * @param  moveList moveList struct to add moves to.
 */
void addBishopMoves(board_t* b, moveList_t* moveList);

/**
 * Generate all rook captures for given board and side and add to capMoveList.
 *
 * @param  b	    board_t to search captures on.
 * @param  moveList moveList struct to add moves to.
 */
void addRookCaptures(board_t* b, moveList_t* moveList);

/**
 * Generate all bishop captures for given board and side and add to capMoveList.
 *
 * @param  b	    board_t to search captures on.
 * @param  moveList moveList struct to add moves to.
 */
void addBishopCaptures(board_t* b, moveList_t* moveList);

/**
 * Generate all queen moves for given board and side. Add moves to quietMoveList.
 *
 * @param  b	    board_t.
 * @param  moveList moveList struct to add moves to.
 */
void addQueenMoves(board_t* b, moveList_t* moveList);

/**
 * Generate queen captures and add to capMoveList.
 *
 * @param  b	    board_t.
 * @param  moveList moveList struct to add moves to.
 */
void addQueenCaptures(board_t* b, moveList_t* moveList);

/**
 * Print all generated moves.
 *
 * @param  moveList moveList struct with moves.
 */
void printGeneratedMoves(moveList_t* moveList);





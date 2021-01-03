#pragma once

#include <vector>

#include "board.h"
#include "attacks.h"

void generateMoves(Board* b, moveList_t* moveList, bool inCheck);

void generateQuiescence(Board* b, moveList_t* moveList, bool inCheck);

void generateCheckEvasions(Board* b, moveList_t* moveList);

void generateQuietCheckers(Board* b, moveList_t* moveList);

/**
 * Generate all blockers for the blocking square and add them to the moveList. Kings cannot
 * block.
 *
 * @param  b		  Board.
 * @param  moveList   Blocking moves are added to this moveList.
 * @param  blockingSq Square to be blocked.
 */
void addBlockersForSq(Board* b, moveList_t* moveList, int blockingSq);

/**
 * Generate all possible single white pawn pushes and add to moveList. Does not include
 * promotions, they are handled in a seperate function.
 *
 * @param  b	    Board.
 * @param  moveList moveList struct to add moves to.
 */
void whiteSinglePawnPush(Board* b, moveList_t* moveList);

/**
 * Generate all possible single black pawn pushes and add to moveList. Does not include
 * promotions, they are handled in a seperate function.
 *
 * @param  b	    Board.
 * @param  moveList moveList struct to add moves to.
 */
void blackSinglePawnPush(Board* b, moveList_t* moveList);

/**
 * Generate all possible single push white pawn promotions and add to moveList.
 * Only used for quiesenece move generation, since {@li whiteSinglePawnPush} whiteSinglePawnPush
 * already covers both pushes and promotions.
 *
 * @param  b	    Board.
 * @param  moveList moveList struct to add moves to.
 */
void whitePawnPushProm(Board* b, moveList_t* moveList);

/**
 * Generate all possible single push black pawn promotions and add to moveList.
 * Only used for quiesenece move generation, since {@li blackSinglePawnPush} blackSinglePawnPush
 * already covers both pushes and promotions.
 *
 * @param  b	    Board.
 * @param  moveList moveList struct to add moves to.
 */
void blackPawnPushProm(Board* b, moveList_t* moveList);

/**
 * Generate all possible white double pawn pushes and add to moveList.
 *
 * @param  b	    Board.
 * @param  moveList moveList struct to add moves to.
 */
void whiteDoublePawnPush(Board* b, moveList_t* moveList);

/**
 * Generate all possible black double pawn pushes and add to moveList.
 *
 * @param  b	    Board.
 * @param  moveList moveList struct to add moves to.
 */
void blackDoublePawnPush(Board* b, moveList_t* moveList);

/**
 * Generate all possible white pawn captures and add to moveList EP captures are included.
 *
 * @param  b	    Board.
 * @param  moveList moveList struct to add moves to.
 */
void whitePawnCaptures(Board* b, moveList_t* moveList);

/**
 * Generate all possible black pawn captures and add to moveList EP captures are included.
 *
 * @param  b	    Board.
 * @param  moveList moveList struct to add moves to.
 */
void blackPawnCaptures(Board* b, moveList_t* moveList);

/**
 * Generate all possible knight moves and add to quietMoveList.
 *
 * @param  b	    Board.
 * @param  moveList moveList struct to add moves to.
 */
void addKnightMoves(Board* b, moveList_t* moveList);

/**
 * Generate all possible knight captures for given side and add to capMoveList.
 *
 * @param  b	    Board.
 * @param  moveList moveList struct to add moves to.
 */
void addKnightCaptures(Board* b, moveList_t* moveList);

/**
 * /// <summary>
 * Generate all king moves for given side and add to quietMoveList.
 * </summary>
 *
 * @param  b	    Board.
 * @param  moveList moveList struct to add moves to.
 */
void addKingMoves(Board* b, moveList_t* moveList);

/**
 * Generate king captures and add to capMoveList.
 *
 * @param  b	    Board.
 * @param  moveList moveList struct to add moves to.
 */
void addKingCaptures(Board* b, moveList_t* moveList);

/**
 * Generate king check evasions due to double check. Combines quiet and capture moves. Castling
 * is not legal.
 *
 * @param  b	    Board.
 * @param  moveList moveList struct to add moves to.
 */
void addKingCheckEvasions(Board* b, moveList_t* moveList);

/**
 * Generate all rook moves for given board and side and add to quietMoveList.
 *
 * @param  b	    Board to search moves on.
 * @param  moveList moveList struct to add moves to.
 */
void addRookMoves(Board* b, moveList_t* moveList);

/**
 * Generate all bishop moves for given board and side and add to quietMoveList.
 *
 * @param  b	    Board to search moves on.
 * @param  moveList moveList struct to add moves to.
 */
void addBishopMoves(Board* b, moveList_t* moveList);

/**
 * Generate all rook captures for given board and side and add to capMoveList.
 *
 * @param  b	    Board to search captures on.
 * @param  moveList moveList struct to add moves to.
 */
void addRookCaptures(Board* b, moveList_t* moveList);

/**
 * Generate all bishop captures for given board and side and add to capMoveList.
 *
 * @param  b	    Board to search captures on.
 * @param  moveList moveList struct to add moves to.
 */
void addBishopCaptures(Board* b, moveList_t* moveList);

/**
 * Generate all queen moves for given board and side. Add moves to quietMoveList.
 *
 * @param  b	    Board.
 * @param  moveList moveList struct to add moves to.
 */
void addQueenMoves(Board* b, moveList_t* moveList);

/**
 * Generate queen captures and add to capMoveList.
 *
 * @param  b	    Board.
 * @param  moveList moveList struct to add moves to.
 */
void addQueenCaptures(Board* b, moveList_t* moveList);

/**
 * Print all generated moves.
 *
 * @param  moveList moveList struct with moves.
 */
void printGeneratedMoves(moveList_t* moveList);





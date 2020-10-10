#pragma once

#include <vector>

#include "board.h"
#include "attacks.h"
#include "move.h"


void generateMoves(Board* b, MOVE_S* move_s);

/* PAWN */
void whiteSinglePawnPush(Board* b, MOVE_S* move_s);
void blackSinglePawnPush(Board* b, MOVE_S* move_s);
void whiteDoublePawnPush(Board* b, MOVE_S* move_s);

void blackDoublePawnPush(Board* b, MOVE_S* move_s);
void whitePawnCaptures(Board* b, MOVE_S* move_s);
void blackPawnCaptures(Board* b, MOVE_S* move_s);

/* KNIGHT */
/// <summary>
/// Generate all possible knight moves and add to quietMoveList.
/// </summary>
/// <param name="b">Board</param>
/// <param name="side">Side to move</param>
void addKnightMoves(Board* b, MOVE_S* move_s);

/// <summary>
/// Generate all possible knight captures for given side and add to capMoveList.
/// </summary>
/// <param name="b">Board</param>
/// <param name="side">Side to move</param>
void addKnightCaptures(Board* b, MOVE_S* move_s);

/* KING */
/// <summary>
/// Generate all king moves for given side and add to quietMoveList.
/// </summary>
/// <param name="b">Board</param>
/// <param name="side">Side to move</param>
void addKingMoves(Board* b, MOVE_S* move_s);

/// <summary>
/// Generate king captures and add to capMoveList.
/// </summary>
/// <param name="b">Board</param>
/// <param name="side">Side to move</param>
void addKingCaptures(Board* b, MOVE_S* move_s);

/// <summary>
/// Generate all rook moves for given board and side and add to quietMoveList.
/// </summary>
/// <param name="b">Board to search moves on</param>
/// <param name="side">Moving side</param>
void addRookMoves(Board* b, MOVE_S* move_s);

/// <summary>
/// Generate all bishop moves for given board and side and add to quietMoveList.
/// </summary>
/// <param name="b">Board to search moves on</param>
/// <param name="side">Moving side</param>
void addBishopMoves(Board* b, MOVE_S* move_s);

/// <summary>
/// Generate all rook captures for given board and side and add to capMoveList.
/// </summary>
/// <param name="b">Board to search captures on</param>
/// <param name="side">Moving side</param>
void addRookCaptures(Board* b, MOVE_S* move_s);

/// <summary>
/// Generate all bishop captures for given board and side and add to capMoveList.
/// </summary>
/// <param name="b">Board to search captures on</param>
/// <param name="side">Moving side</param>
void addBishopCaptures(Board* b, MOVE_S* move_s);

/// <summary>
/// Generate all queen moves for given board and side. Add moves to quietMoveList.
/// </summary>
/// <param name="b">Board</param>
/// <param name="side">Side to move</param>
void addQueenMoves(Board* b, MOVE_S* move_s);

/// <summary>
/// Generate queen captures and add to capMoveList.
/// </summary>
/// <param name="b">Board</param>
/// <param name="side">Side to move</param>
void addQueenCaptures(Board* b, MOVE_S* move_s);

/// <summary>
/// Print all generated moves.
 //</summary>
 //<param name="b">Board</param>
void printGeneratedMoves(MOVE_S* move_s);





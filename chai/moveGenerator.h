#pragma once

#include <vector>

#include "board.h"
#include "attacks.h"
#include "move.h"

void addCaptureMove(Board* b, moveList_t* move_s, int move, int movingPiece);

void initMVV_LVA();

bitboard_t getLeastValuablePiece(Board* b, bitboard_t atkSet, int side, int atkPiece);

int see(Board* b, const int move);

void generateMoves(Board* b, moveList_t* move_s);

void generateCaptures(Board* b, moveList_t* move_s);

bool moveLegal(Board* b, const int move);

/* PAWN */
void whiteSinglePawnPush(Board* b, moveList_t* move_s);
void blackSinglePawnPush(Board* b, moveList_t* move_s);
void whiteDoublePawnPush(Board* b, moveList_t* move_s);

void blackDoublePawnPush(Board* b, moveList_t* move_s);
void whitePawnCaptures(Board* b, moveList_t* move_s);
void blackPawnCaptures(Board* b, moveList_t* move_s);

/* KNIGHT */
/// <summary>
/// Generate all possible knight moves and add to quietMoveList.
/// </summary>
/// <param name="b">Board</param>
/// <param name="side">Side to move</param>
void addKnightMoves(Board* b, moveList_t* move_s);

/// <summary>
/// Generate all possible knight captures for given side and add to capMoveList.
/// </summary>
/// <param name="b">Board</param>
/// <param name="side">Side to move</param>
void addKnightCaptures(Board* b, moveList_t* move_s);

/* KING */
/// <summary>
/// Generate all king moves for given side and add to quietMoveList.
/// </summary>
/// <param name="b">Board</param>
/// <param name="side">Side to move</param>
void addKingMoves(Board* b, moveList_t* move_s);

/// <summary>
/// Generate king captures and add to capMoveList.
/// </summary>
/// <param name="b">Board</param>
/// <param name="side">Side to move</param>
void addKingCaptures(Board* b, moveList_t* move_s);

/// <summary>
/// Generate all rook moves for given board and side and add to quietMoveList.
/// </summary>
/// <param name="b">Board to search moves on</param>
/// <param name="side">Moving side</param>
void addRookMoves(Board* b, moveList_t* move_s);

/// <summary>
/// Generate all bishop moves for given board and side and add to quietMoveList.
/// </summary>
/// <param name="b">Board to search moves on</param>
/// <param name="side">Moving side</param>
void addBishopMoves(Board* b, moveList_t* move_s);

/// <summary>
/// Generate all rook captures for given board and side and add to capMoveList.
/// </summary>
/// <param name="b">Board to search captures on</param>
/// <param name="side">Moving side</param>
void addRookCaptures(Board* b, moveList_t* move_s);

/// <summary>
/// Generate all bishop captures for given board and side and add to capMoveList.
/// </summary>
/// <param name="b">Board to search captures on</param>
/// <param name="side">Moving side</param>
void addBishopCaptures(Board* b, moveList_t* move_s);

/// <summary>
/// Generate all queen moves for given board and side. Add moves to quietMoveList.
/// </summary>
/// <param name="b">Board</param>
/// <param name="side">Side to move</param>
void addQueenMoves(Board* b, moveList_t* move_s);

/// <summary>
/// Generate queen captures and add to capMoveList.
/// </summary>
/// <param name="b">Board</param>
/// <param name="side">Side to move</param>
void addQueenCaptures(Board* b, moveList_t* move_s);

/// <summary>
/// Print all generated moves.
 //</summary>
 //<param name="b">Board</param>
void printGeneratedMoves(moveList_t* move_s);





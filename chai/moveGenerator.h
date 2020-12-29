#pragma once

#include <vector>

#include "board.h"
#include "attacks.h"

extern bitboard_t FULL;
extern int MVV_LVA[13][13];
void initMVV_LVA();

bitboard_t getLeastValuablePiece(Board* b, bitboard_t atkSet, int side, int atkPiece);

int see(Board* b, const int move);

void generateMoves(Board* b, moveList_t* moveList, bool inCheck);

void generateQuiesence(Board* b, moveList_t* moveList);

void generateCheckEvasions(Board* b, moveList_t* moveList);

void addBlockersForSq(Board* b, moveList_t* moveList, int blockingSq);

bool isLegal(Board* b, move_t move, bool inCheck);

/* PAWN */
void whiteSinglePawnPush(Board* b, moveList_t* moveList);
void blackSinglePawnPush(Board* b, moveList_t* moveList);
void whiteDoublePawnPush(Board* b, moveList_t* moveList);
void blackDoublePawnPush(Board* b, moveList_t* moveList);

void whitePawnCaptures(Board* b, moveList_t* moveList);
void blackPawnCaptures(Board* b, moveList_t* moveList);

/// <summary>
/// Generate all possible knight moves and add to quietMoveList.
/// </summary>
/// <param name="b">Board</param>
/// <param name="moveList">moveList struct to add moves to</param>
void addKnightMoves(Board* b, moveList_t* moveList);

/// <summary>
/// Generate all possible knight captures for given side and add to capMoveList.
/// </summary>
/// <param name="b">Board</param>
/// <param name="moveList">moveList struct to add moves to</param>
void addKnightCaptures(Board* b, moveList_t* moveList);

/// /// <summary>
/// Generate all king moves for given side and add to quietMoveList.
/// </summary>
/// <param name="b">Board</param>
/// <param name="moveList">moveList struct to add moves to</param>
void addKingMoves(Board* b, moveList_t* moveList);

/// <summary>
/// Generate king captures and add to capMoveList.
/// </summary>
/// <param name="b">Board</param>
/// <param name="moveList">moveList struct to add moves to</param>
void addKingCaptures(Board* b, moveList_t* moveList);

/// <summary>
/// Generate king check evasions due to double check. Combines quiet and capture
/// moves. Castling is not legal.
/// </summary>
/// <param name="b">Board</param>
/// <param name="moveList">moveList struct to add moves to</param>
void addKingCheckEvasions(Board* b, moveList_t* moveList);

/// <summary>
/// Generate all rook moves for given board and side and add to quietMoveList.
/// </summary>
/// <param name="b">Board to search moves on</param>
/// <param name="moveList">moveList struct to add moves to</param>
void addRookMoves(Board* b, moveList_t* moveList);

/// <summary>
/// Generate all bishop moves for given board and side and add to quietMoveList.
/// </summary>
/// <param name="b">Board to search moves on</param>
/// <param name="moveList">moveList struct to add moves to</param>
void addBishopMoves(Board* b, moveList_t* moveList);

/// <summary>
/// Generate all rook captures for given board and side and add to capMoveList.
/// </summary>
/// <param name="b">Board to search captures on</param>
/// <param name="moveList">moveList struct to add moves to</param>
void addRookCaptures(Board* b, moveList_t* moveList);

/// <summary>
/// Generate all bishop captures for given board and side and add to capMoveList.
/// </summary>
/// <param name="b">Board to search captures on</param>
/// <param name="moveList">moveList struct to add moves to</param>
void addBishopCaptures(Board* b, moveList_t* moveList);

/// <summary>
/// Generate all queen moves for given board and side. Add moves to quietMoveList.
/// </summary>
/// <param name="b">Board</param>
/// <param name="moveList">moveList struct to add moves to</param>
void addQueenMoves(Board* b, moveList_t* moveList);

/// <summary>
/// Generate queen captures and add to capMoveList.
/// </summary>
/// <param name="b">Board</param>
/// <param name="moveList">moveList struct to add moves to</param>
void addQueenCaptures(Board* b, moveList_t* moveList);

/// <summary>
/// Print all generated moves.
///</summary>
///<param name="moveList">moveList struct with moves</param>
void printGeneratedMoves(moveList_t* moveList);





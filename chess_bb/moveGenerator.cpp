#include "moveGenerator.h"

void MoveGenerator::generatePawnMoves(Board b) {

	if (b.side == WHITE) {
		whiteSinglePawnPush(b);
		whiteDoublePawnPush(b);
	} else {
		blackSinglePawnPush(b);
		blackDoublePawnPush(b);
	}

}

/// <summary>
/// Calculate possible single push pawn moves for white.
/// </summary>
void MoveGenerator::whiteSinglePawnPush(Board board) {
	int sq;
	U64 pushedPawns = (board.getPiecesByColor(PAWN, WHITE) << 8) & ~board.occupied;

	// divide proms and normal pushes
	U64 promPawns = pushedPawns & RANK_8_HEX;
	pushedPawns = pushedPawns & ~RANK_8_HEX;

	// normal pawn pushes
	while (pushedPawns) {
		sq = board.popBit(&pushedPawns);
		moveList[moves++] = Move(MOVE(sq - 8, sq, EMPTY, EMPTY, EMPTY), 0);
	}

	// prom pawn pushes
	while (promPawns) {
		sq = board.popBit(&promPawns);
		moveList[moves++] = Move(MOVE(sq - 8, sq, EMPTY, Q, EMPTY), 0);
		moveList[moves++] = Move(MOVE(sq - 8, sq, EMPTY, R, EMPTY), 0);
		moveList[moves++] = Move(MOVE(sq - 8, sq, EMPTY, B, EMPTY), 0);
		moveList[moves++] = Move(MOVE(sq - 8, sq, EMPTY, N, EMPTY), 0);
	}
}

/// <summary>
/// Calculate possible single push pawn moves for black.
/// </summary>
void MoveGenerator::blackSinglePawnPush(Board board) {
	int sq;
	U64 pushedPawns = (board.getPiecesByColor(PAWN, BLACK) << 8) & ~board.occupied;

	// divide proms and normal pushes
	U64 promPawns = pushedPawns & RANK_1_HEX;
	pushedPawns = pushedPawns & ~RANK_1_HEX;

	// normal pawn pushes
	while (pushedPawns) {
		sq = board.popBit(&pushedPawns);
		moveList[moves++] = Move(MOVE(sq + 8, sq, EMPTY, EMPTY, EMPTY), 0);
	}

	// prom pawn pushes
	while (promPawns) {
		sq = board.popBit(&promPawns);
		moveList[moves++] = Move(MOVE(sq + 8, sq, EMPTY, q, EMPTY), 0);
		moveList[moves++] = Move(MOVE(sq + 8, sq, EMPTY, r, EMPTY), 0);
		moveList[moves++] = Move(MOVE(sq + 8, sq, EMPTY, b, EMPTY), 0);
		moveList[moves++] = Move(MOVE(sq + 8, sq, EMPTY, n, EMPTY), 0);
	}
}

/// <summary>
/// Calculate all possible moves with pawnstart for white.
/// </summary>
void MoveGenerator::whiteDoublePawnPush(Board board) {
	int sq;
	U64 pushedPawns = (board.getPiecesByColor(PAWN, WHITE) << 8) & ~board.occupied;
	pushedPawns = (pushedPawns << 8) & ~board.occupied & RANK_4_HEX;

	while (pushedPawns) {
		sq = board.popBit(&pushedPawns);
		moveList[moves++] = Move(MOVE(sq - 16, sq, EMPTY, EMPTY, MFLAGPS), 0);
	}
}

/// <summary>
/// Calculate all possible moves with pawnstart for black.
/// </summary>
void MoveGenerator::blackDoublePawnPush(Board board) {
	int sq;
	U64 pushedPawns = (board.getPiecesByColor(PAWN, BLACK) >> 8) & ~board.occupied;
	pushedPawns = (pushedPawns >> 8) & ~board.occupied & RANK_5_HEX;

	while (pushedPawns) {
		sq = board.popBit(&pushedPawns);
		moveList[moves++] = Move(MOVE(sq + 16, sq, EMPTY, EMPTY, MFLAGPS), 0);
	}
}

/// <summary>
/// Print all generated moves.
/// </summary>
void MoveGenerator::printGeneratedMoves(Board b) {

	cout << "Generated " << moves << " moves:" << endl;

	for (int i = 0; i < moves; i++) {
		b.printMove(moveList[i].move);
	}

}

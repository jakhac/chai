#include "moveGenerator.h"

void MoveGenerator::generatePawnMoves(Board b) {

	if (b.side == WHITE) {
		whiteSinglePawnPush(b);
		whiteDoublePawnPush(b);
		whitePawnAttacks(b, 7, FILE_H_HEX);
		whitePawnAttacks(b, 9, FILE_A_HEX);
	} else {
		blackSinglePawnPush(b);
		blackDoublePawnPush(b);
		blackPawnAttacks(b, 7, FILE_A_HEX);
		blackPawnAttacks(b, 9, FILE_H_HEX);

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
	U64 pushedPawns = (board.getPiecesByColor(PAWN, BLACK) >> 8) & ~board.occupied;

	board.printBitBoard(&pushedPawns);

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
/// Generate all possible east attacks for white pawns
/// </summary>
void MoveGenerator::whitePawnAttacks(Board board, int shift, U64 forbiddenFile) {
	int sq;
	U64 whitePawnsShifted = board.getPiecesByColor(PAWN, WHITE) << shift & ~forbiddenFile;
	U64 whitePawnAttacks = whitePawnsShifted  & board.color[BLACK];

	//divide in prom and non prom attacks
	U64 whitePromAttacks = whitePawnAttacks & RANK_8_HEX;
	whitePawnAttacks &= ~RANK_8_HEX;

	// en passant square
	if (whitePawnsShifted & board.setMask[board.enPas]) {
		moveList[moves++] = Move(MOVE(board.enPas - shift, board.enPas, EMPTY, EMPTY, MFLAGEP), 0);
	}

	// normal diagonal attacks
	while (whitePawnAttacks) {
		sq = board.popBit(&whitePawnAttacks);
		moveList[moves++] = Move(MOVE(sq - shift, sq, board.pieceAt(sq), EMPTY, EMPTY), 0);
	}

	// promoting attacks
	while (whitePromAttacks) {
		sq = board.popBit(&whitePromAttacks);
		moveList[moves++] = Move(MOVE(sq - shift, sq, EMPTY, Q, EMPTY), 0);
		moveList[moves++] = Move(MOVE(sq - shift, sq, EMPTY, R, EMPTY), 0);
		moveList[moves++] = Move(MOVE(sq - shift, sq, EMPTY, B, EMPTY), 0);
		moveList[moves++] = Move(MOVE(sq - shift, sq, EMPTY, N, EMPTY), 0);
	}
}

/// <summary>
/// Generate all possible east attacks for white pawns
/// </summary>
void MoveGenerator::blackPawnAttacks(Board board, int shift, U64 forbiddenFile) {
	int sq;
	U64 blackPawnsShifted = board.getPiecesByColor(PAWN, BLACK) >> shift & ~forbiddenFile;
	U64 blackPawnAttacks = blackPawnsShifted & board.color[WHITE];

	//divide in prom and non prom attacks
	U64 blackPromAttacks = blackPawnAttacks & RANK_1_HEX;
	blackPawnAttacks &= ~RANK_1_HEX;

	board.printBitBoard(&blackPawnsShifted);
	board.printBitBoard(&board.setMask[board.enPas]);

	// en passant square
	if (blackPawnsShifted & board.setMask[board.enPas]) {
		moveList[moves++] = Move(MOVE(board.enPas + shift, board.enPas, EMPTY, EMPTY, MFLAGEP), 0);
	}

	// normal diagonal attacks
	while (blackPawnAttacks) {
		sq = board.popBit(&blackPawnAttacks);
		moveList[moves++] = Move(MOVE(sq + shift, sq, board.pieceAt(sq), EMPTY, EMPTY), 0);
	}

	// promoting attacks
	while (blackPromAttacks) {
		sq = board.popBit(&blackPromAttacks);
		moveList[moves++] = Move(MOVE(sq + shift, sq, EMPTY, q, EMPTY), 0);
		moveList[moves++] = Move(MOVE(sq + shift, sq, EMPTY, r, EMPTY), 0);
		moveList[moves++] = Move(MOVE(sq + shift, sq, EMPTY, b, EMPTY), 0);
		moveList[moves++] = Move(MOVE(sq + shift, sq, EMPTY, n, EMPTY), 0);
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

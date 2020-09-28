#include "moveGenerator.h"

void MoveGenerator::generatePawnMoves(Board b) {
	if (b.side == WHITE) {
		whiteSinglePawnPush(b);
		whiteDoublePawnPush(b);
		whitePawnCaptures(b);
	} else {
		blackSinglePawnPush(b);
		blackDoublePawnPush(b);
		blackPawnCaptures(b);
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
		sq = popBit(&pushedPawns);
		quietMoveList.push_back(Move(MOVE(sq - 8, sq, EMPTY, EMPTY, EMPTY), 0));
	}

	// prom pawn pushes
	while (promPawns) {
		sq = popBit(&promPawns);
		quietMoveList.push_back(Move(MOVE(sq - 8, sq, EMPTY, Q, EMPTY), 0));
		quietMoveList.push_back(Move(MOVE(sq - 8, sq, EMPTY, R, EMPTY), 0));
		quietMoveList.push_back(Move(MOVE(sq - 8, sq, EMPTY, B, EMPTY), 0));
		quietMoveList.push_back(Move(MOVE(sq - 8, sq, EMPTY, N, EMPTY), 0));
	}
}

/// <summary>
/// Calculate possible single push pawn moves for black.
/// </summary>
void MoveGenerator::blackSinglePawnPush(Board board) {
	int sq;
	U64 pushedPawns = (board.getPiecesByColor(PAWN, BLACK) >> 8) & ~board.occupied;

	// divide proms and normal pushes
	U64 promPawns = pushedPawns & RANK_1_HEX;
	pushedPawns = pushedPawns & ~RANK_1_HEX;

	// normal pawn pushes
	while (pushedPawns) {
		sq = popBit(&pushedPawns);
		quietMoveList.push_back(Move(MOVE(sq + 8, sq, EMPTY, EMPTY, EMPTY), 0));
	}

	// prom pawn pushes
	while (promPawns) {
		sq = popBit(&promPawns);
		quietMoveList.push_back(Move(MOVE(sq + 8, sq, EMPTY, q, EMPTY), 0));
		quietMoveList.push_back(Move(MOVE(sq + 8, sq, EMPTY, r, EMPTY), 0));
		quietMoveList.push_back(Move(MOVE(sq + 8, sq, EMPTY, b, EMPTY), 0));
		quietMoveList.push_back(Move(MOVE(sq + 8, sq, EMPTY, n, EMPTY), 0));
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
		sq = popBit(&pushedPawns);
		quietMoveList.push_back(Move(MOVE(sq - 16, sq, EMPTY, EMPTY, MFLAGPS), 0));
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
		sq = popBit(&pushedPawns);
		quietMoveList.push_back(Move(MOVE(sq + 16, sq, EMPTY, EMPTY, MFLAGPS), 0));
	}
}

/// <summary>
/// Generate all possible east attacks for white pawns
/// </summary>
void MoveGenerator::whitePawnCaptures(Board board) {
	int sq, atk_sq;
	U64 atks;

	//divide in prom and non prom attacks
	U64 whitePawns = board.getPiecesByColor(PAWN, WHITE);
	U64 whitePawnProm = whitePawns & RANK_7_HEX;
	whitePawns &= ~RANK_7_HEX;

	// en passant square
	if ((whitePawns << 7) & setMask[board.enPas]) {
		capMoveList.push_back(Move(MOVE(board.enPas - 7, board.enPas, EMPTY, EMPTY, MFLAGEP), 0));
	} else if ((whitePawns << 9) & setMask[board.enPas]) {
		capMoveList.push_back(Move(MOVE(board.enPas - 9, board.enPas, EMPTY, EMPTY, MFLAGEP), 0));
	}

	while (whitePawns) {
		sq = popBit(&whitePawns);
		atks = pawnAtkMask[WHITE][sq] & board.color[BLACK];
		while (atks) {
			atk_sq = popBit(&atks);
			capMoveList.push_back(Move(MOVE(sq, atk_sq, board.pieceAt(sq), EMPTY, EMPTY), 0));
		}
	}

	// promoting attacks
	while (whitePawnProm) {
		sq = popBit(&whitePawnProm);
		atks = pawnAtkMask[WHITE][sq] & board.color[BLACK];
		while (atks) {
			atk_sq = popBit(&atks);
			capMoveList.push_back(Move(MOVE(sq, atk_sq, EMPTY, Q, EMPTY), 0));
			capMoveList.push_back(Move(MOVE(sq, atk_sq, EMPTY, R, EMPTY), 0));
			capMoveList.push_back(Move(MOVE(sq, atk_sq, EMPTY, B, EMPTY), 0));
			capMoveList.push_back(Move(MOVE(sq, atk_sq, EMPTY, N, EMPTY), 0));
		}
	}
}

/// <summary>
/// Generate all possible east attacks for white pawns
/// </summary>
void MoveGenerator::blackPawnCaptures(Board board) {
	int sq, atk_sq;
	U64 atks;

	//divide in prom and non prom attacks
	U64 blackPawns = board.getPiecesByColor(PAWN, BLACK);
	U64 blackPawnProm = blackPawns & RANK_2_HEX;
	blackPawns &= ~RANK_2_HEX;

	// en passant square
	if ((blackPawns >> 7) & setMask[board.enPas]) {
		capMoveList.push_back(Move(MOVE(board.enPas + 7, board.enPas, EMPTY, EMPTY, MFLAGEP), 0));
	} else if ((blackPawns >> 9) & setMask[board.enPas]) {
		capMoveList.push_back(Move(MOVE(board.enPas + 9, board.enPas, EMPTY, EMPTY, MFLAGEP), 0));
	}

	while (blackPawns) {
		sq = popBit(&blackPawns);
		atks = pawnAtkMask[BLACK][sq] & board.color[WHITE];
		while (atks) {
			atk_sq = popBit(&atks);
			capMoveList.push_back(Move(MOVE(sq, atk_sq, board.pieceAt(sq), EMPTY, EMPTY), 0));
		}
	}

	// promoting attacks
	while (blackPawnProm) {
		sq = popBit(&blackPawnProm);
		atks = pawnAtkMask[BLACK][sq] & board.color[WHITE];
		while (atks) {
			atk_sq = popBit(&atks);
			capMoveList.push_back(Move(MOVE(sq, atk_sq, EMPTY, q, EMPTY), 0));
			capMoveList.push_back(Move(MOVE(sq, atk_sq, EMPTY, r, EMPTY), 0));
			capMoveList.push_back(Move(MOVE(sq, atk_sq, EMPTY, b, EMPTY), 0));
			capMoveList.push_back(Move(MOVE(sq, atk_sq, EMPTY, n, EMPTY), 0));
		}
	}
}

/// <summary>
/// Generate all white quiet knight moves.
/// </summary>
void MoveGenerator::whiteKnightMoves(Board board) {
	int sq, atk_sq;
	U64 whiteKnights = board.getPiecesByColor(KNIGHT, WHITE);
	U64 atks;

	while (whiteKnights) {
		sq = popBit(&whiteKnights);
		atks = knightAtkMask[sq] & ~board.occupied;
		while (atks) {
			atk_sq = popBit(&atks);
			quietMoveList.push_back(Move(MOVE(sq, atk_sq, EMPTY, EMPTY, EMPTY), 0));
		}
	}
}

/// <summary>
/// Generate all black quiet knight moves.
/// </summary>
void MoveGenerator::blackKnightMoves(Board board) {
	int sq, atk_sq;
	U64 blackKnights = board.getPiecesByColor(KNIGHT, BLACK);
	U64 atks;

	while (blackKnights) {
		sq = popBit(&blackKnights);
		atks = knightAtkMask[sq] & ~board.occupied;
		while (atks) {
			atk_sq = popBit(&atks);
			quietMoveList.push_back(Move(MOVE(sq, atk_sq, EMPTY, EMPTY, EMPTY), 0));
		}
	}
}

/// <summary>
/// Generate white knight attacks
/// </summary>
void MoveGenerator::whiteKnightCaptures(Board board) {
	int sq, atk_sq;
	U64 whiteKnights = board.getPiecesByColor(KNIGHT, WHITE);
	U64 atks;

	while (whiteKnights) {
		sq = popBit(&whiteKnights);
		atks = knightAtkMask[sq] & board.color[BLACK];
		while (atks) {
			atk_sq = popBit(&atks);
			capMoveList.push_back(Move(MOVE(sq, atk_sq, EMPTY, EMPTY, EMPTY), 0));
		}
	}
}

/// <summary>
/// Generate black knight attacks
/// </summary>
void MoveGenerator::blackKnightCaptures(Board board) {
	int sq, atk_sq;
	U64 blackKnights = board.getPiecesByColor(KNIGHT, BLACK);
	U64 atks;

	while (blackKnights) {
		sq = popBit(&blackKnights);
		atks = knightAtkMask[sq] & board.color[WHITE];
		while (atks) {
			atk_sq = popBit(&atks);
			capMoveList.push_back(Move(MOVE(sq, atk_sq, EMPTY, EMPTY, EMPTY), 0));
		}
	}
}

/// <summary>
/// Generate all white king moves. Kings cannot move into squares attacked by opposite king.
/// King moves includes castling.
/// </summary>
void MoveGenerator::whiteKingMoves(Board board) {
	U64 wKing = board.getPiecesByColor(KING, WHITE);
	int wSq = popBit(&wKing);

	U64 bKing = board.getPiecesByColor(KING, BLACK);
	int bSq = popBit(&bKing);

	U64 kingMoves = kingAtkMask[wSq] & ~kingAtkMask[bSq] & ~board.occupied;
	int sq;

	while (kingMoves) {
		sq = popBit(&kingMoves);
		if (!squareAttacked()) continue; // TODO CHECK IF TO_SQUARE IS ATTACKED
		quietMoveList.push_back(Move(MOVE(wSq, sq, EMPTY, EMPTY, EMPTY), 0));
	}

	// TODO CHECK IF CASTLE SQUARES ARE ATTACKED
	if ((board.castlePermission & K_CASTLE) && castleValid()) {
		ASSERT(wSq == E1);
		quietMoveList.push_back(Move(MOVE(wSq, G1, EMPTY, EMPTY, MFLAGCA), 0));
	}

	// TODO CHECK IF CASTLE SQUARES ARE ATTACKED
	if ((board.castlePermission & Q_CASTLE) && castleValid()) {
		ASSERT(wSq == E1);
		quietMoveList.push_back(Move(MOVE(wSq, C1, EMPTY, EMPTY, MFLAGCA), 0));
	}
}

/// <summary>
/// Generate all black king moves. Kings cannot move into squares attacked by opposite king.
/// King moves includes castling.
/// </summary>
void MoveGenerator::blackKingMoves(Board board) {
	U64 bKing = board.getPiecesByColor(KING, BLACK);
	int bSq = popBit(&bKing);

	U64 wKing = board.getPiecesByColor(KING, WHITE);
	int wSq = popBit(&wKing);

	U64 kingMoves = kingAtkMask[bSq] & ~kingAtkMask[wSq] & ~board.occupied;
	int sq;

	while (kingMoves) {
		sq = popBit(&kingMoves);
		if (!squareAttacked()) continue; // TODO CHECK IF TO_SQUARE IS ATTACKED
		quietMoveList.push_back(Move(MOVE(bSq, sq, EMPTY, EMPTY, EMPTY), 0));
	}

	// TODO CHECK IF CASTLE SQUARES ARE ATTACKED
	if ((board.castlePermission & k_CASTLE) && castleValid()) {
		ASSERT(bSq == E8);
		quietMoveList.push_back(Move(MOVE(bSq, G8, EMPTY, EMPTY, MFLAGCA), 0));
	}

	// TODO CHECK IF CASTLE SQUARES ARE ATTACKED
	if ((board.castlePermission & q_CASTLE) && castleValid()) {
		ASSERT(bSq == E8);
		quietMoveList.push_back(Move(MOVE(bSq, C8, EMPTY, EMPTY, MFLAGCA), 0));
	}
}

/// <summary>
/// Generate all white attacking king moves.
/// </summary>
void MoveGenerator::whiteKingCaptures(Board board) {
	U64 wKing = board.getPiecesByColor(KING, WHITE);
	int wSq = popBit(&wKing);

	U64 bKing = board.getPiecesByColor(KING, BLACK);
	int bSq = popBit(&bKing);

	U64 whiteKingAttacks = kingAtkMask[wSq] & ~kingAtkMask[bSq] & board.color[BLACK];
	int sq;

	while (whiteKingAttacks) {
		sq = popBit(&whiteKingAttacks);
		if (!squareAttacked()) continue; // TODO CHECK IF TO_SQUARE IS ATTACKED
		capMoveList.push_back(Move(MOVE(bSq, sq, board.pieceAt(sq), EMPTY, EMPTY), 0));
	}
}

/// <summary>
/// Generate all black attacking king moves.
/// </summary>
void MoveGenerator::blackKingCaptures(Board board) {
	U64 bKing = board.getPiecesByColor(KING, BLACK);
	int bSq = popBit(&bKing);

	U64 wKing = board.getPiecesByColor(KING, WHITE);
	int wSq = popBit(&wKing);

	U64 blackKingAttacks = kingAtkMask[bSq] & ~kingAtkMask[wSq] & board.color[WHITE];
	int sq;

	while (blackKingAttacks) {
		sq = popBit(&blackKingAttacks);
		if (!squareAttacked()) continue; // TODO CHECK IF TO_SQUARE IS ATTACKED
		capMoveList.push_back(Move(MOVE(bSq, sq, board.pieceAt(sq), EMPTY, EMPTY), 0));
	}
}

/// <summary>
/// Print all generated moves.
/// </summary>
void MoveGenerator::printGeneratedMoves(Board b) {

	cout << "\nGenerated " << capMoveList.size() + quietMoveList.size() << " moves:" << endl;

	vector <Move> allMoves;
	allMoves.reserve(capMoveList.size() + quietMoveList.size()); // preallocate memory
	allMoves.insert(allMoves.end(), capMoveList.begin(), capMoveList.end()); // copy
	allMoves.insert(allMoves.end(), quietMoveList.begin(), quietMoveList.end()); // copy

	for (Move move : allMoves) {
		printMove(move.move);
	}
}

int MoveGenerator::castleValid() {
	return 1;
}

int MoveGenerator::squareAttacked() {
	return 1;
}

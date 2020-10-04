#include "moveGenerator.h"

void MoveGenerator::generateMoves(Board b) {
	resetMoveLists();

	const int playerToMove = b.side;

	// pawns
	addPawnMoves(b, playerToMove);

	// knights
	addKnightMoves(b, playerToMove);
	addKnightCaptures(b, playerToMove);

	// bishops
	addBishopMoves(b, playerToMove);
	addBishopCaptures(b, playerToMove);

	// king
	addKingMoves(b, playerToMove);
	addKingCaptures(b, playerToMove);

	// rooks
	addRookMoves(b, playerToMove);
	addRookCaptures(b, playerToMove);

	// queens
	addQueenMoves(b, playerToMove);
	addQueenCaptures(b, playerToMove);

}

vector<Move> MoveGenerator::getAllMoves() {
	vector <Move> allMoves;
	allMoves.reserve(capMoveList.size() + quietMoveList.size()); // preallocate memory
	allMoves.insert(allMoves.end(), capMoveList.begin(), capMoveList.end()); // copy
	allMoves.insert(allMoves.end(), quietMoveList.begin(), quietMoveList.end()); // copy

	return allMoves;
}

void MoveGenerator::resetMoveLists() {
	quietMoveList = {};
	capMoveList = {};
}

/// <summary>
/// Generate all possible pawn moves for given board. Side is retrieved with board instance.
/// </summary>
/// <param name="b">Board</param>
void MoveGenerator::addPawnMoves(Board b, const int side) {
	if (side) {
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
/// <param name="board">Board</param>
void MoveGenerator::whiteSinglePawnPush(Board board) {
	int sq;
	U64 pushedPawns = (board.getPieces(PAWN, WHITE) << 8) & ~board.occupied;

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
/// <param name="board">Board</param> 
void MoveGenerator::blackSinglePawnPush(Board board) {
	int sq;
	U64 pushedPawns = (board.getPieces(PAWN, BLACK) >> 8) & ~board.occupied;

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
/// <param name="board">Board</param>
void MoveGenerator::whiteDoublePawnPush(Board board) {
	int sq;
	U64 pushedPawns = (board.getPieces(PAWN, WHITE) << 8) & ~board.occupied;
	pushedPawns = (pushedPawns << 8) & ~board.occupied & RANK_4_HEX;

	while (pushedPawns) {
		sq = popBit(&pushedPawns);
		quietMoveList.push_back(Move(MOVE(sq - 16, sq, EMPTY, EMPTY, MFLAGPS), 0));
	}
}

/// <summary>
/// Calculate all possible moves with pawnstart for black.
/// </summary>
/// <param name="board">Board</param>
void MoveGenerator::blackDoublePawnPush(Board board) {
	int sq;
	U64 pushedPawns = (board.getPieces(PAWN, BLACK) >> 8) & ~board.occupied;
	pushedPawns = (pushedPawns >> 8) & ~board.occupied & RANK_5_HEX;

	while (pushedPawns) {
		sq = popBit(&pushedPawns);
		quietMoveList.push_back(Move(MOVE(sq + 16, sq, EMPTY, EMPTY, MFLAGPS), 0));
	}
}

/// <summary>
/// Generate all possible east attacks for white pawns
/// </summary>
/// <param name="board">Board</param>
void MoveGenerator::whitePawnCaptures(Board board) {
	int sq, atk_sq;
	U64 atks;

	//divide in prom and non prom attacks
	U64 whitePawns = board.getPieces(PAWN, WHITE);
	U64 whitePawnProm = whitePawns & RANK_7_HEX;
	whitePawns &= ~RANK_7_HEX;

	// en passant square
	if ((whitePawns << 7 & ~FILE_H_HEX) & setMask[board.enPas]) {
		capMoveList.push_back(Move(MOVE(board.enPas - 7, board.enPas, EMPTY, EMPTY, MFLAGEP), 0));
	}
	if ((whitePawns << 9 & ~FILE_A_HEX) & setMask[board.enPas]) {
		capMoveList.push_back(Move(MOVE(board.enPas - 9, board.enPas, EMPTY, EMPTY, MFLAGEP), 0));
	}

	while (whitePawns) {
		sq = popBit(&whitePawns);
		atks = pawnAtkMask[WHITE][sq] & board.color[BLACK];
		while (atks) {
			atk_sq = popBit(&atks);
			capMoveList.push_back(Move(MOVE(sq, atk_sq, board.pieceAt(atk_sq), EMPTY, EMPTY), 0));
		}
	}

	// promoting attacks
	while (whitePawnProm) {
		sq = popBit(&whitePawnProm);
		atks = pawnAtkMask[WHITE][sq] & board.color[BLACK];
		while (atks) {
			atk_sq = popBit(&atks);
			capMoveList.push_back(Move(MOVE(sq, atk_sq, board.pieceAt(atk_sq), Q, EMPTY), 0));
			capMoveList.push_back(Move(MOVE(sq, atk_sq, board.pieceAt(atk_sq), R, EMPTY), 0));
			capMoveList.push_back(Move(MOVE(sq, atk_sq, board.pieceAt(atk_sq), B, EMPTY), 0));
			capMoveList.push_back(Move(MOVE(sq, atk_sq, board.pieceAt(atk_sq), N, EMPTY), 0));
		}
	}
}

/// <summary>
/// Generate all possible east attacks for white pawns.
/// </summary>
/// <param name="board">Board</param>
void MoveGenerator::blackPawnCaptures(Board board) {
	int sq, atk_sq;
	U64 atks;

	//divide in prom and non prom attacks
	U64 blackPawns = board.getPieces(PAWN, BLACK);
	U64 blackPawnProm = blackPawns & RANK_2_HEX;
	blackPawns &= ~RANK_2_HEX;

	// en passant square
	if ((blackPawns >> 7 & ~FILE_A_HEX) & setMask[board.enPas]) {
		capMoveList.push_back(Move(MOVE(board.enPas + 7, board.enPas, EMPTY, EMPTY, MFLAGEP), 0));
	}
	if ((blackPawns >> 9 & ~FILE_H_HEX) & setMask[board.enPas]) {
		capMoveList.push_back(Move(MOVE(board.enPas + 9, board.enPas, EMPTY, EMPTY, MFLAGEP), 0));
	}

	while (blackPawns) {
		sq = popBit(&blackPawns);
		atks = pawnAtkMask[BLACK][sq] & board.color[WHITE];
		while (atks) {
			atk_sq = popBit(&atks);
			capMoveList.push_back(Move(MOVE(sq, atk_sq, board.pieceAt(atk_sq), EMPTY, EMPTY), 0));
		}
	}

	// promoting attacks
	while (blackPawnProm) {
		sq = popBit(&blackPawnProm);
		atks = pawnAtkMask[BLACK][sq] & board.color[WHITE];
		while (atks) {
			atk_sq = popBit(&atks);
			capMoveList.push_back(Move(MOVE(sq, atk_sq, board.pieceAt(atk_sq), q, EMPTY), 0));
			capMoveList.push_back(Move(MOVE(sq, atk_sq, board.pieceAt(atk_sq), r, EMPTY), 0));
			capMoveList.push_back(Move(MOVE(sq, atk_sq, board.pieceAt(atk_sq), b, EMPTY), 0));
			capMoveList.push_back(Move(MOVE(sq, atk_sq, board.pieceAt(atk_sq), n, EMPTY), 0));
		}
	}
}

void MoveGenerator::addKnightMoves(Board b, const int side) {
	int sq, atk_sq;
	U64 knights = b.getPieces(KNIGHT, side);
	U64 atks;

	while (knights) {
		sq = popBit(&knights);
		atks = knightAtkMask[sq] & ~b.occupied;
		while (atks) {
			atk_sq = popBit(&atks);
			quietMoveList.push_back(Move(MOVE(sq, atk_sq, EMPTY, EMPTY, EMPTY), 0));
		}
	}
}

void MoveGenerator::addKnightCaptures(Board b, const int side) {
	int sq, atk_sq;
	U64 knights = b.getPieces(KNIGHT, side);
	U64 atks;

	while (knights) {
		sq = popBit(&knights);
		atks = knightAtkMask[sq] & b.color[side ^ 1];
		while (atks) {
			atk_sq = popBit(&atks);
			capMoveList.push_back(Move(MOVE(sq, atk_sq, b.pieceAt(atk_sq), EMPTY, EMPTY), 0));
		}
	}
}

void MoveGenerator::addKingMoves(Board b, const int side) {
	if (side == WHITE) addWhiteKingMoves(b);
	else addBlackKingMoves(b);
}

void MoveGenerator::addWhiteKingMoves(Board board) {
	U64 wKing = board.getPieces(KING, WHITE);
	int wSq = popBit(&wKing);

	U64 bKing = board.getPieces(KING, BLACK);
	int bSq = popBit(&bKing);

	U64 kingMoves = kingAtkMask[wSq] & ~kingAtkMask[bSq] & ~board.occupied;
	int sq;

	while (kingMoves) {
		sq = popBit(&kingMoves);

		// check for non attacked square
		if (board.squareAttacked(sq, board.side ^ 1)) continue;
		quietMoveList.push_back(Move(MOVE(wSq, sq, EMPTY, EMPTY, EMPTY), 0));
	}

	if (board.castleValid(K_CASTLE)) {
		ASSERT(wSq == E1);
		quietMoveList.push_back(Move(MOVE(wSq, G1, EMPTY, EMPTY, MFLAGCA), 0));
	}

	if (board.castleValid(Q_CASTLE)) {
		ASSERT(wSq == E1);
		quietMoveList.push_back(Move(MOVE(wSq, C1, EMPTY, EMPTY, MFLAGCA), 0));
	}
}

void MoveGenerator::addBlackKingMoves(Board board) {
	U64 bKing = board.getPieces(KING, BLACK);
	int bSq = popBit(&bKing);

	U64 wKing = board.getPieces(KING, WHITE);
	int wSq = popBit(&wKing);

	U64 kingMoves = kingAtkMask[bSq] & ~kingAtkMask[wSq] & ~board.occupied;
	int sq;

	while (kingMoves) {
		sq = popBit(&kingMoves);

		// check for non attacked square
		if (board.squareAttacked(sq, board.side ^ 1)) continue;
		quietMoveList.push_back(Move(MOVE(bSq, sq, EMPTY, EMPTY, EMPTY), 0));
	}

	if (board.castleValid(k_CASTLE)) {
		ASSERT(bSq == E8);
		quietMoveList.push_back(Move(MOVE(bSq, G8, EMPTY, EMPTY, MFLAGCA), 0));
	}

	if (board.castleValid(q_CASTLE)) {
		ASSERT(bSq == E8);
		quietMoveList.push_back(Move(MOVE(bSq, C8, EMPTY, EMPTY, MFLAGCA), 0));
	}
}

void MoveGenerator::addKingCaptures(Board b, const int side) {
	if (side == WHITE) addWhiteKingCaptures(b);
	else addBlackKingCaptures(b);
}

void MoveGenerator::addWhiteKingCaptures(Board board) {
	U64 wKing = board.getPieces(KING, WHITE);
	int wSq = popBit(&wKing);

	U64 bKing = board.getPieces(KING, BLACK);
	int bSq = popBit(&bKing);

	U64 whiteKingAttacks = kingAtkMask[wSq] & ~kingAtkMask[bSq] & board.color[BLACK];
	int atk_sq;

	while (whiteKingAttacks) {
		atk_sq = popBit(&whiteKingAttacks);

		// check for non attacked square
		if (board.squareAttacked(atk_sq, board.side ^ 1)) continue;
		capMoveList.push_back(Move(MOVE(wSq, atk_sq, board.pieceAt(atk_sq), EMPTY, EMPTY), 0));
	}
}

void MoveGenerator::addBlackKingCaptures(Board board) {
	U64 bKing = board.getPieces(KING, BLACK);
	int bSq = popBit(&bKing);

	U64 wKing = board.getPieces(KING, WHITE);
	int wSq = popBit(&wKing);

	U64 blackKingAttacks = kingAtkMask[bSq] & ~kingAtkMask[wSq] & board.color[WHITE];
	int atk_sq;

	while (blackKingAttacks) {
		atk_sq = popBit(&blackKingAttacks);

		// check for non attacked square
		if (board.squareAttacked(atk_sq, board.side ^ 1)) continue;
		capMoveList.push_back(Move(MOVE(bSq, atk_sq, board.pieceAt(atk_sq), EMPTY, EMPTY), 0));
	}
}

void MoveGenerator::addRookMoves(Board b, const int side) {
	int sq, atk_sq;
	U64 attackSet;
	U64 rooks = b.getPieces(ROOK, side);
	while (rooks) {
		sq = popBit(&rooks);
		attackSet = lookUpRookMoves(sq, b.occupied);
		ASSERT(attackSet == calculateRookMoves(sq, b.occupied));
		attackSet &= ~b.occupied;
		while (attackSet) {
			atk_sq = popBit(&attackSet);
			quietMoveList.push_back(Move(MOVE(sq, atk_sq, EMPTY, EMPTY, EMPTY), 0));
		}
	}
}

void MoveGenerator::addBishopMoves(Board b, const int side) {
	int sq, atk_sq;
	U64 bishops = b.getPieces(BISHOP, side);
	U64 attackSet;
	while (bishops) {
		sq = popBit(&bishops);
		attackSet = lookUpBishopMoves(sq, b.occupied);
		attackSet &= ~b.occupied;

		while (attackSet) {
			atk_sq = popBit(&attackSet);
			quietMoveList.push_back(Move(MOVE(sq, atk_sq, EMPTY, EMPTY, EMPTY), 0));
		}
	}
}

void MoveGenerator::addRookCaptures(Board b, const int side) {
	int sq, atk_sq;
	U64 captureSet;
	U64 rooks = b.getPieces(ROOK, side);
	while (rooks) {
		sq = popBit(&rooks);
		captureSet = lookUpRookMoves(sq, b.occupied);
		captureSet = captureSet & b.color[side ^ 1];

		while (captureSet) {
			atk_sq = popBit(&captureSet);
			capMoveList.push_back(Move(MOVE(sq, atk_sq, b.pieceAt(atk_sq), EMPTY, EMPTY), 0));
		}
	}
}

void MoveGenerator::addBishopCaptures(Board b, const int side) {
	int sq, atk_sq;
	U64 captureSet;
	U64 bishops = b.getPieces(BISHOP, side);
	while (bishops) {
		sq = popBit(&bishops);
		captureSet = lookUpBishopMoves(sq, b.occupied);
		captureSet &= b.color[side ^ 1];

		while (captureSet) {
			atk_sq = popBit(&captureSet);
			capMoveList.push_back(Move(MOVE(sq, atk_sq, b.pieceAt(atk_sq), EMPTY, EMPTY), 0));
		}
	}

}

void MoveGenerator::addQueenMoves(Board b, const int side) {
	int sq, atk_sq;
	U64 attackSet;
	U64 queen = b.getPieces(QUEEN, side);
	while (queen) {
		sq = popBit(&queen);
		attackSet = lookUpRookMoves(sq, b.occupied) ^ lookUpBishopMoves(sq, b.occupied);
		attackSet &= ~b.occupied;

		while (attackSet) {
			atk_sq = popBit(&attackSet);
			quietMoveList.push_back(Move(MOVE(sq, atk_sq, EMPTY, EMPTY, EMPTY), 0));
		}
	}
}

void MoveGenerator::addQueenCaptures(Board b, const int side) {
	int sq, atk_sq;
	U64 attackSet;
	U64 queen = b.getPieces(QUEEN, side);
	while (queen) {
		sq = popBit(&queen);
		attackSet = lookUpRookMoves(sq, b.occupied) ^ lookUpBishopMoves(sq, b.occupied);
		attackSet &= b.color[side ^ 1];

		while (attackSet) {
			atk_sq = popBit(&attackSet);
			quietMoveList.push_back(Move(MOVE(sq, atk_sq, b.pieceAt(atk_sq), EMPTY, EMPTY), 0));
		}
	}
}

void MoveGenerator::printGeneratedMoves(Board b) {

	cout << "\nGenerated " << capMoveList.size() + quietMoveList.size() << " moves for: " << colorString[b.side] << endl;

	vector <Move> allMoves;
	allMoves.reserve(capMoveList.size() + quietMoveList.size()); // preallocate memory
	allMoves.insert(allMoves.end(), capMoveList.begin(), capMoveList.end()); // copy
	allMoves.insert(allMoves.end(), quietMoveList.begin(), quietMoveList.end()); // copy

	for (Move move : allMoves) {
		printMove(move.move);
	}
}
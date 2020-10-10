#include "moveGenerator.h"

void generateMoves(Board* b, MOVE_S* move_s) {
	move_s->attackedSquares = b->attackerSet(b->side^1);

	 //pawns
	if (b->side == WHITE) {
		whiteSinglePawnPush(b, move_s);
		whiteDoublePawnPush(b, move_s);
		whitePawnCaptures(b, move_s);
	} else {
		blackSinglePawnPush(b, move_s);
		blackDoublePawnPush(b, move_s);
		blackPawnCaptures(b, move_s);
	}

	// knights
	addKnightMoves(b, move_s);
	addKnightCaptures(b, move_s);

	// bishops
	addBishopMoves(b, move_s);
	addBishopCaptures(b, move_s);

	// king
	addKingMoves(b, move_s);
	addKingCaptures(b, move_s);

	// rooks
	addRookMoves(b, move_s);
	addRookCaptures(b, move_s);

	// queens
	addQueenMoves(b, move_s);
	addQueenCaptures(b, move_s);

}

void resetMoveLists() {
	//quietMoveList = {};
	//capMoveList = {};
}

/// <summary>
/// Calculate possible single push pawn moves for white.
/// </summary>
/// <param name="board">Board</param>
void whiteSinglePawnPush(Board* b, MOVE_S* move_s) {
	int sq;
	U64 pushedPawns = (b->getPieces(PAWN, WHITE) << 8) & ~b->occupied;

	// divide proms and normal pushes
	U64 promPawns = pushedPawns & RANK_8_HEX;
	pushedPawns = pushedPawns & ~RANK_8_HEX;

	// normal pawn pushes
	while (pushedPawns) {
		sq = popBit(&pushedPawns);
		move_s->moveList[move_s->moveCounter++] = MOVE(sq - 8, sq, EMPTY, EMPTY, EMPTY);
	}

	// prom pawn pushes
	while (promPawns) {
		sq = popBit(&promPawns);
		move_s->moveList[move_s->moveCounter++] = MOVE(sq - 8, sq, EMPTY, Q, EMPTY);
		move_s->moveList[move_s->moveCounter++] = MOVE(sq - 8, sq, EMPTY, R, EMPTY);
		move_s->moveList[move_s->moveCounter++] = MOVE(sq - 8, sq, EMPTY, B, EMPTY);
		move_s->moveList[move_s->moveCounter++] = MOVE(sq - 8, sq, EMPTY, N, EMPTY);
	}
}

/// <summary>
/// Calculate possible single push pawn moves for black.
/// </summary>
/// <param name="board">Board</param> 
void blackSinglePawnPush(Board* board, MOVE_S* move_s) {
	int sq;
	U64 pushedPawns = (board->getPieces(PAWN, BLACK) >> 8) & ~board->occupied;

	// divide proms and normal pushes
	U64 promPawns = pushedPawns & RANK_1_HEX;
	pushedPawns = pushedPawns & ~RANK_1_HEX;

	// normal pawn pushes
	while (pushedPawns) {
		sq = popBit(&pushedPawns);
		move_s->moveList[move_s->moveCounter++] = MOVE(sq + 8, sq, EMPTY, EMPTY, EMPTY);
	}

	// prom pawn pushes
	while (promPawns) {
		sq = popBit(&promPawns);
		move_s->moveList[move_s->moveCounter++] = MOVE(sq + 8, sq, EMPTY, q, EMPTY);
		move_s->moveList[move_s->moveCounter++] = MOVE(sq + 8, sq, EMPTY, r, EMPTY);
		move_s->moveList[move_s->moveCounter++] = MOVE(sq + 8, sq, EMPTY, b, EMPTY);
		move_s->moveList[move_s->moveCounter++] = MOVE(sq + 8, sq, EMPTY, n, EMPTY);
	}
}

/// <summary>
/// Calculate all possible moves with pawnstart for white.
/// </summary>
/// <param name="board">Board</param>
void whiteDoublePawnPush(Board* board, MOVE_S* move_s) {
	int sq;
	U64 pushedPawns = (board->getPieces(PAWN, WHITE) << 8) & ~board->occupied;
	pushedPawns = (pushedPawns << 8) & ~board->occupied & RANK_4_HEX;

	while (pushedPawns) {
		sq = popBit(&pushedPawns);
		move_s->moveList[move_s->moveCounter++] = MOVE(sq - 16, sq, EMPTY, EMPTY, MFLAGPS);
	}
}

/// <summary>
/// Calculate all possible moves with pawnstart for black.
/// </summary>
/// <param name="board">Board</param>
void blackDoublePawnPush(Board* board, MOVE_S* move_s) {
	int sq;
	U64 pushedPawns = (board->getPieces(PAWN, BLACK) >> 8) & ~board->occupied;
	pushedPawns = (pushedPawns >> 8) & ~board->occupied & RANK_5_HEX;

	while (pushedPawns) {
		sq = popBit(&pushedPawns);
		move_s->moveList[move_s->moveCounter++] = MOVE(sq + 16, sq, EMPTY, EMPTY, MFLAGPS);
	}
}

/// <summary>
/// Generate all possible east attacks for white pawns
/// </summary>
/// <param name="board">Board</param>
void whitePawnCaptures(Board* board, MOVE_S* move_s) {
	int sq, atk_sq;
	U64 atks;

	//divide in prom and non prom attacks
	U64 whitePawns = board->getPieces(PAWN, WHITE);
	U64 whitePawnProm = whitePawns & RANK_7_HEX;
	whitePawns &= ~RANK_7_HEX;

	// en passant square
	if ((whitePawns << 7 & ~FILE_H_HEX) & setMask[board->enPas]) {
		move_s->moveList[move_s->moveCounter++] = MOVE(board->enPas - 7, board->enPas, EMPTY, EMPTY, MFLAGEP);
	}
	if ((whitePawns << 9 & ~FILE_A_HEX) & setMask[board->enPas]) {
		move_s->moveList[move_s->moveCounter++] = MOVE(board->enPas - 9, board->enPas, EMPTY, EMPTY, MFLAGEP);
	}

	while (whitePawns) {
		sq = popBit(&whitePawns);
		atks = pawnAtkMask[WHITE][sq] & board->color[BLACK];
		while (atks) {
			atk_sq = popBit(&atks);
			move_s->moveList[move_s->moveCounter++] = MOVE(sq, atk_sq, board->pieceAt(atk_sq), EMPTY, EMPTY);
		}
	}

	// promoting attacks
	while (whitePawnProm) {
		sq = popBit(&whitePawnProm);
		atks = pawnAtkMask[WHITE][sq] & board->color[BLACK];
		while (atks) {
			atk_sq = popBit(&atks);
			move_s->moveList[move_s->moveCounter++] = MOVE(sq, atk_sq, board->pieceAt(atk_sq), Q, EMPTY);
			move_s->moveList[move_s->moveCounter++] = MOVE(sq, atk_sq, board->pieceAt(atk_sq), R, EMPTY);
			move_s->moveList[move_s->moveCounter++] = MOVE(sq, atk_sq, board->pieceAt(atk_sq), B, EMPTY);
			move_s->moveList[move_s->moveCounter++] = MOVE(sq, atk_sq, board->pieceAt(atk_sq), N, EMPTY);
		}
	}
}

/// <summary>
/// Generate all possible east attacks for white pawns.
/// </summary>
/// <param name="board">Board</param>
void blackPawnCaptures(Board* board, MOVE_S* move_s) {
	int sq, atk_sq;
	U64 atks;

	//divide in prom and non prom attacks
	U64 blackPawns = board->getPieces(PAWN, BLACK);
	U64 blackPawnProm = blackPawns & RANK_2_HEX;
	blackPawns &= ~RANK_2_HEX;

	// en passant square
	if ((blackPawns >> 7 & ~FILE_A_HEX) & setMask[board->enPas]) {
		move_s->moveList[move_s->moveCounter++] = MOVE(board->enPas + 7, board->enPas, EMPTY, EMPTY, MFLAGEP);
	}
	if ((blackPawns >> 9 & ~FILE_H_HEX) & setMask[board->enPas]) {
		move_s->moveList[move_s->moveCounter++] = MOVE(board->enPas + 9, board->enPas, EMPTY, EMPTY, MFLAGEP);
	}

	while (blackPawns) {
		sq = popBit(&blackPawns);
		atks = pawnAtkMask[BLACK][sq] & board->color[WHITE];
		while (atks) {
			atk_sq = popBit(&atks);
			move_s->moveList[move_s->moveCounter++] = MOVE(sq, atk_sq, board->pieceAt(atk_sq), EMPTY, EMPTY);
		}
	}

	// promoting attacks
	while (blackPawnProm) {
		sq = popBit(&blackPawnProm);
		atks = pawnAtkMask[BLACK][sq] & board->color[WHITE];
		while (atks) {
			atk_sq = popBit(&atks);
			move_s->moveList[move_s->moveCounter++] = MOVE(sq, atk_sq, board->pieceAt(atk_sq), q, EMPTY);
			move_s->moveList[move_s->moveCounter++] = MOVE(sq, atk_sq, board->pieceAt(atk_sq), r, EMPTY);
			move_s->moveList[move_s->moveCounter++] = MOVE(sq, atk_sq, board->pieceAt(atk_sq), b, EMPTY);
			move_s->moveList[move_s->moveCounter++] = MOVE(sq, atk_sq, board->pieceAt(atk_sq), n, EMPTY);
		}
	}
}

void addKnightMoves(Board* b, MOVE_S* move_s) {
	int sq, atk_sq;
	U64 knights = b->getPieces(KNIGHT, b->side);
	U64 atks;

	while (knights) {
		sq = popBit(&knights);
		atks = knightAtkMask[sq] & ~b->occupied;
		while (atks) {
			atk_sq = popBit(&atks);
			move_s->moveList[move_s->moveCounter++] = MOVE(sq, atk_sq, EMPTY, EMPTY, EMPTY);
		}
	}
}

void addKnightCaptures(Board* b, MOVE_S* move_s) {
	int sq, atk_sq;
	U64 knights = b->getPieces(KNIGHT, b->side);
	U64 atks;

	while (knights) {
		sq = popBit(&knights);
		atks = knightAtkMask[sq] & b->color[b->side ^ 1];
		while (atks) {
			atk_sq = popBit(&atks);
			move_s->moveList[move_s->moveCounter++] = MOVE(sq, atk_sq, b->pieceAt(atk_sq), EMPTY, EMPTY);
		}
	}
}

void addKingMoves(Board* b, MOVE_S* move_s) {
	int sq, kSq = b->getKingSquare(b->side);
	U64 kingMoves = kingAtkMask[kSq] & ~move_s->attackedSquares & ~b->occupied;

	while (kingMoves) {
		sq = popBit(&kingMoves);
		move_s->moveList[move_s->moveCounter++] = MOVE(kSq, sq, EMPTY, EMPTY, EMPTY);
	}

	switch (b->side) {
		case WHITE:	
			if (b->castleValid(K_CASTLE, &move_s->attackedSquares)) {
				ASSERT(kSq == E1);
				move_s->moveList[move_s->moveCounter++] = MOVE(kSq, G1, EMPTY, EMPTY, MFLAGCA);
			}
			if (b->castleValid(Q_CASTLE, &move_s->attackedSquares)) {
				ASSERT(kSq == E1);
				move_s->moveList[move_s->moveCounter++] = MOVE(kSq, C1, EMPTY, EMPTY, MFLAGCA);
			}
			break;
		case BLACK:
			if (b->castleValid(k_CASTLE, &move_s->attackedSquares)) {
				ASSERT(kSq == E8);
				move_s->moveList[move_s->moveCounter++] = MOVE(kSq, G8, EMPTY, EMPTY, MFLAGCA);
			}

			if (b->castleValid(q_CASTLE, &move_s->attackedSquares)) {
				ASSERT(kSq == E8);
				move_s->moveList[move_s->moveCounter++] = MOVE(kSq, C8, EMPTY, EMPTY, MFLAGCA);
			}
			break;
		default: break;
	}
}

void addKingCaptures(Board* b, MOVE_S* move_s) {
	int atk_sq, kSq = b->getKingSquare(b->side);
	U64 whiteKingAttacks = kingAtkMask[kSq] & ~move_s->attackedSquares & b->color[b->side^1];

	while (whiteKingAttacks) {
		atk_sq = popBit(&whiteKingAttacks);
		move_s->moveList[move_s->moveCounter++] = MOVE(kSq, atk_sq, b->pieceAt(atk_sq), EMPTY, EMPTY);
	}
}

void addRookMoves(Board* b, MOVE_S* move_s) {
	int sq, atk_sq;
	U64 attackSet;
	U64 rooks = b->getPieces(ROOK, b->side);
	while (rooks) {
		sq = popBit(&rooks);
		attackSet = lookUpRookMoves(sq, b->occupied);
		ASSERT(attackSet == calculateRookMoves(sq, b->occupied));
		attackSet &= ~b->occupied;
		while (attackSet) {
			atk_sq = popBit(&attackSet);
			move_s->moveList[move_s->moveCounter++] = MOVE(sq, atk_sq, EMPTY, EMPTY, EMPTY);
		}
	}
}

void addBishopMoves(Board* b, MOVE_S* move_s) {
	int sq, atk_sq;
	U64 bishops = b->getPieces(BISHOP, b->side);
	U64 attackSet;
	while (bishops) {
		sq = popBit(&bishops);
		attackSet = lookUpBishopMoves(sq, b->occupied);
		attackSet &= ~b->occupied;

		while (attackSet) {
			atk_sq = popBit(&attackSet);
			move_s->moveList[move_s->moveCounter++] = MOVE(sq, atk_sq, EMPTY, EMPTY, EMPTY);
		}
	}
}

void addRookCaptures(Board* b, MOVE_S* move_s) {
	int sq, atk_sq;
	U64 captureSet;
	U64 rooks = b->getPieces(ROOK, b->side);
	while (rooks) {
		sq = popBit(&rooks);
		captureSet = lookUpRookMoves(sq, b->occupied);
		captureSet = captureSet & b->color[b->side ^ 1];

		while (captureSet) {
			atk_sq = popBit(&captureSet);
			move_s->moveList[move_s->moveCounter++] = MOVE(sq, atk_sq, b->pieceAt(atk_sq), EMPTY, EMPTY);
		}
	}
}

void addBishopCaptures(Board* b, MOVE_S* move_s) {
	int sq, atk_sq;
	U64 captureSet;
	U64 bishops = b->getPieces(BISHOP, b->side);
	while (bishops) {
		sq = popBit(&bishops);
		captureSet = lookUpBishopMoves(sq, b->occupied);
		captureSet &= b->color[b->side ^ 1];

		while (captureSet) {
			atk_sq = popBit(&captureSet);
			move_s->moveList[move_s->moveCounter++] = MOVE(sq, atk_sq, b->pieceAt(atk_sq), EMPTY, EMPTY);
		}
	}

}

void addQueenMoves(Board* b, MOVE_S* move_s) {
	int sq, atk_sq;
	U64 attackSet;
	U64 queen = b->getPieces(QUEEN, b->side);
	while (queen) {
		sq = popBit(&queen);
		attackSet = lookUpRookMoves(sq, b->occupied) ^ lookUpBishopMoves(sq, b->occupied);
		attackSet &= ~b->occupied;

		while (attackSet) {
			atk_sq = popBit(&attackSet);
			move_s->moveList[move_s->moveCounter++] = MOVE(sq, atk_sq, EMPTY, EMPTY, EMPTY);
		}
	}
}

void addQueenCaptures(Board* b, MOVE_S* move_s) {
	int sq, atk_sq;
	U64 attackSet;
	U64 queen = b->getPieces(QUEEN, b->side);
	while (queen) {
		sq = popBit(&queen);
		attackSet = lookUpRookMoves(sq, b->occupied) ^ lookUpBishopMoves(sq, b->occupied);
		attackSet &= b->color[b->side ^ 1];

		while (attackSet) {
			atk_sq = popBit(&attackSet);
			move_s->moveList[move_s->moveCounter++] = MOVE(sq, atk_sq, b->pieceAt(atk_sq), EMPTY, EMPTY);
		}
	}
}

void printGeneratedMoves(MOVE_S* move_s) {

	cout << "\nGenerated " << move_s->moveCounter << " moves: " << endl;

	for (int i = 0; i < move_s->moveCounter; i++) {
		printMove(move_s->moveList[i]);
	}
}
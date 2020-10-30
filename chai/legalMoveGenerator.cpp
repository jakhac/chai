#include "legalMoveGenerator.h"

void _generateMoves(Board* b, MOVE_S* move_s) {
	move_s->attackedSquares = b->attackerSet(b->side^1);

	//pawns
	if (b->side == WHITE) {
		_whiteSinglePawnPush(b, move_s);
		_whiteDoublePawnPush(b, move_s);
		_whitePawnCaptures(b, move_s);
	} else {
		_blackSinglePawnPush(b, move_s);
		_blackDoublePawnPush(b, move_s);
		_blackPawnCaptures(b, move_s);
	}

	//knights
	_addKnightMoves(b, move_s);
	_addKnightCaptures(b, move_s);

	// bishops
	_addBishopMoves(b, move_s);
	_addBishopCaptures(b, move_s);

	// king
	_addKingMoves(b, move_s);
	_addKingCaptures(b, move_s);

	// rooks
	_addRookMoves(b, move_s);
	_addRookCaptures(b, move_s);

	// queens
	_addQueenMoves(b, move_s);
	_addQueenCaptures(b, move_s);

	// if check filter moves
	if (b->getPieces(KING, b->side) & move_s->attackedSquares) {
		int validMoves = 0;
		for (int i = 0; i < move_s->moveCounter; i++) {
			b->push(move_s->moveList[i]);
			if (b->isCheck(b->side ^ 1)) {
				// no add
			} else {
				move_s->moveList[validMoves++] = move_s->moveList[i];
			}
			b->pop();
		}
		move_s->moveCounter = validMoves;
	}

	//MOVE_S pseudo[1];
	//generateMoves(b, pseudo);
	//int legalMovesCounter = 0;
	//int legalMoves[MAX_POSITION_MOVES];
	//for (int i = 0; i < pseudo->moveCounter; i++) {
	//	b->push(pseudo->moveList[i]);
	//	if (b->isCheck(b->side ^ 1)) {
	//		// non valid move leaves side in check
	//	} else {
	//		legalMoves[legalMovesCounter++] = pseudo->moveList[i];
	//	}
	//	b->pop();
	//}
	//for (int i = 0; i < legalMovesCounter; i++) {
	//	bool found = false;
	//	int findMe = legalMoves[i];
	//	for (int j = 0; j < move_s->moveCounter; j++) {
	//		if (findMe == move_s->moveList[j]) {
	//			found = true;
	//		}
	//	}
	//	if (!found) {
	//		cout << "Should be " << endl;
	//		for (int i = 0; i < legalMovesCounter; i++) {
	//			printMove(legalMoves[i]);
	//		}
	//		cout << "\n\nBut is" << endl;
	//		for (int i = 0; i < move_s->moveCounter; i++) {
	//			printMove(move_s->moveList[i]);
	//		}
	//		exit(1);
	//	}
	//}

}

/// <summary>
/// Calculate possible single push pawn moves for white.
/// </summary>
/// <param name="board">Board</param>
void _whiteSinglePawnPush(Board* board, MOVE_S* move_s) {
	int sq, kSq = board->getKingSquare(WHITE);
	U64 pinners = board->pinner(kSq, WHITE);
	U64 pinned = board->pinned(kSq, WHITE);

	U64 whitePawns = board->getPieces(PAWN, WHITE);
	U64 potPinned = whitePawns & pinned;
	whitePawns = whitePawns & ~pinned;

	// iterate pinned pawns
	//while (potPinned) {
	//	potPinnedSq = popBit(&potPinned);
	//	if (FILE_LIST[squareToFile[potPinnedSq]] & pinners) {
	//		whitePawns |= setMask[potPinnedSq];
	//	}
	//}

	while (potPinned) {
		U64 _pinners = pinners;
		sq = popBit(&potPinned);

		// for each pawn try to find pinner
		while (_pinners) {
			int pinnerSq = popBit(&_pinners);
			U64 attackLine = obstructed(kSq, pinnerSq) | setMask[kSq] | setMask[pinnerSq];

			// pinned piece found, check for same file (diagonal pins do not allow pushes)
			if ((setMask[sq] & attackLine) && squareToFile[pinnerSq] == squareToFile[sq]) {
				whitePawns |= setMask[sq];
			}
		}
	}

	U64 pushedPawns = (whitePawns << 8) & ~board->occupied;

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
void _blackSinglePawnPush(Board* board, MOVE_S* move_s) {
	int sq, kSq = board->getKingSquare(BLACK);

	U64 pinners = board->pinner(kSq, BLACK);
	U64 pinned = board->pinned(kSq, BLACK);
	U64 blackPawns = board->getPieces(PAWN, BLACK);
	U64 potPinned = blackPawns & pinned;
	blackPawns &= ~pinned;

	 //if there is a pinner on same file as pinned pawn, push is allowed
	//while (potPinned) {
	//	int potPinnedSq = popBit(&potPinned);
	//	if (FILE_LIST[squareToFile[potPinnedSq]] & pinners) {
	//		blackPawns |= setMask[potPinnedSq];
	//	}
	//}

	while (potPinned) {
		U64 _pinners = pinners;
		sq = popBit(&potPinned);

		// for each pawn try to find pinner
		while (_pinners) {
			int pinnerSq = popBit(&_pinners);
			U64 attackLine = obstructed(kSq, pinnerSq) | setMask[kSq] | setMask[pinnerSq];

			// pinned piece found, check for same file (diagonal pins do not allow pushes)
			if ((setMask[sq] & attackLine) && squareToFile[pinnerSq]  == squareToFile[sq]) {
				blackPawns |= setMask[sq];
			}
		}
	}

	U64 pushedPawns = (blackPawns >> 8) & ~board->occupied;

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
void _whiteDoublePawnPush(Board* board, MOVE_S* move_s) {
	int potPinnedSq, sq, kSq = board->getKingSquare(WHITE);
	U64 pinners = board->pinner(kSq, WHITE);
	U64 pinned = board->pinned(kSq, WHITE);

	U64 whitePawns = board->getPieces(PAWN, WHITE);
	U64 potPinned = whitePawns & pinned;
	whitePawns &= ~pinned;

	// iterate pinned pawns
	while (potPinned) {
		potPinnedSq = popBit(&potPinned);

		// if there is a pinner on same file as pinned pawn, push is allowed
		if (FILE_LIST[squareToFile[potPinnedSq]] & pinners) {
			whitePawns |= setMask[potPinnedSq];
		}
	}

	U64 pushedPawns = (whitePawns << 8) & ~board->occupied;
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
void _blackDoublePawnPush(Board* board, MOVE_S* move_s) {
	int sq, kSq = board->getKingSquare(BLACK);

	U64 pinners = board->pinner(kSq, BLACK);
	U64 pinned = board->pinned(kSq, BLACK);
	U64 blackPawns = board->getPieces(PAWN, BLACK);
	U64 potPinned = blackPawns & pinned;
	blackPawns &= ~pinned;

	// if there is a pinner on same file as pinned pawn, push is allowed
	while (potPinned) {
		int potPinnedSq = popBit(&potPinned);
		if (FILE_LIST[squareToFile[potPinnedSq]] & pinners) {
			blackPawns |= setMask[potPinnedSq];
		}
	}

	U64 pushedPawns = (blackPawns >> 8) & ~board->occupied;
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
void _whitePawnCaptures(Board* board, MOVE_S* move_s) {
	int sq, atk_sq, kSq = board->getKingSquare(WHITE);

	U64 atks;
	U64 pinners = board->pinner(kSq, WHITE);
	U64 pinned = board->pinned(kSq, WHITE);
	U64 whitePawns = board->getPieces(PAWN, WHITE);
	U64 potPinned = whitePawns & pinned;
	whitePawns &= ~pinned;

	//iterate pinned pawns
	while (potPinned) {
		U64 _pinners = pinners;
		sq = popBit(&potPinned);

		// for each pawn try to find pinner
		while (_pinners) {
			int pinnerSq = popBit(&_pinners);
			U64 attackLine = obstructed(kSq, pinnerSq) | setMask[kSq] | setMask[pinnerSq];

			// pinned piece found
			if (setMask[sq] & attackLine) {
				if (pawnAtkMask[WHITE][sq] & setMask[pinnerSq]) {
					// capture pinner
					move_s->moveList[move_s->moveCounter++] = MOVE(sq, pinnerSq, board->pieceAt(pinnerSq), EMPTY, EMPTY);
					_pinners = 0ULL;
				}
			}
		}
	}

	//divide in prom and non prom attacks
	U64 whitePawnProm = whitePawns & RANK_7_HEX;
	whitePawns &= ~RANK_7_HEX;

	// en passant square
	if ((whitePawns << 7 & ~FILE_H_HEX) & setMask[board->enPas]) {
		// set white pawn on enPas square and checkif it is pinned
		board->clearPiece(PAWN, board->enPas - 8, BLACK);
		board->setPiece(PAWN, board->enPas, WHITE);

		if (!(board->squareAttackedBy(kSq, BLACK))) {
			move_s->moveList[move_s->moveCounter++] = MOVE(board->enPas - 7, board->enPas, EMPTY, EMPTY, MFLAGEP);
		}

		board->setPiece(PAWN, board->enPas - 8, BLACK);
		board->clearPiece(PAWN, board->enPas, WHITE);
	}
	if ((whitePawns << 9 & ~FILE_A_HEX) & setMask[board->enPas]) {
		// set white pawn on enPas square and checkif it is pinned
		board->clearPiece(PAWN, board->enPas - 8, BLACK);
		board->setPiece(PAWN, board->enPas, WHITE);

		if (!(board->squareAttackedBy(kSq, BLACK))) {
			move_s->moveList[move_s->moveCounter++] = MOVE(board->enPas - 9, board->enPas, EMPTY, EMPTY, MFLAGEP);
		}

		board->setPiece(PAWN, board->enPas - 8, BLACK);
		board->clearPiece(PAWN, board->enPas, WHITE);
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
void _blackPawnCaptures(Board* board, MOVE_S* move_s) {
	int sq, atk_sq, kSq = board->getKingSquare(BLACK);

	U64 atks;
	U64 pinners = board->pinner(kSq, BLACK);
	U64 pinned = board->pinned(kSq, BLACK);
	U64 blackPawns = board->getPieces(PAWN, BLACK);
	U64 potPinned = blackPawns & pinned;
	blackPawns &= ~pinned;

	//iterate pinned pawns
	while (potPinned) {
		U64 _pinners = pinners;
		sq = popBit(&potPinned);

		// for each pawn try to find pinner
		while (_pinners) {
			int pinnerSq = popBit(&_pinners);
			U64 attackLine = obstructed(kSq, pinnerSq) | setMask[kSq] | setMask[pinnerSq];

			// pinned piece found
			if (setMask[sq] & attackLine) {
				if (pawnAtkMask[BLACK][sq] & setMask[pinnerSq]) {
					// capture pinner
					move_s->moveList[move_s->moveCounter++] = MOVE(sq, pinnerSq, board->pieceAt(pinnerSq), EMPTY, EMPTY);
					_pinners = 0ULL;
				}
			}
		}
	}

	//divide in prom and non prom attacks
	U64 blackPawnProm = blackPawns & RANK_2_HEX;
	blackPawns &= ~RANK_2_HEX;

	// en passant square
	if ((blackPawns >> 7 & ~FILE_A_HEX) & setMask[board->enPas]) {
		board->clearPiece(PAWN, board->enPas + 8, WHITE);
		board->setPiece(PAWN, board->enPas, BLACK);

		if (!(board->squareAttackedBy(kSq, WHITE))) {
			move_s->moveList[move_s->moveCounter++] = MOVE(board->enPas + 7, board->enPas, EMPTY, EMPTY, MFLAGEP);
		}

		board->setPiece(PAWN, board->enPas + 8, WHITE);
		board->clearPiece(PAWN, board->enPas, BLACK);
	}
	if ((blackPawns >> 9 & ~FILE_H_HEX) & setMask[board->enPas]) {
		board->clearPiece(PAWN, board->enPas + 8, WHITE);
		board->setPiece(PAWN, board->enPas, BLACK);

		if (!(board->squareAttackedBy(kSq, WHITE))) {
			move_s->moveList[move_s->moveCounter++] = MOVE(board->enPas + 9, board->enPas, EMPTY, EMPTY, MFLAGEP);
		}

		board->setPiece(PAWN, board->enPas + 8, WHITE);
		board->clearPiece(PAWN, board->enPas, BLACK);
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

void _addKnightMoves(Board* b, MOVE_S* move_s) {
	int sq, atk_sq;

	// LEGAL ADD leave pinned knights out of move gen, since the cant block / capture
	U64 knights = b->getPieces(KNIGHT, b->side) & ~b->pinned(b->getKingSquare(b->side), b->side);
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

void _addKnightCaptures(Board* b, MOVE_S* move_s) {
	int sq, atk_sq;

	// LEGAL ADD leave pinned knights out of move gen, since the cant block / capture
	U64 knights = b->getPieces(KNIGHT, b->side) & ~b->pinned(b->getKingSquare(b->side), b->side);
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

void _addKingMoves(Board* b, MOVE_S* move_s) {
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

void _addKingCaptures(Board* b, MOVE_S* move_s) {
	int atk_sq, kSq = b->getKingSquare(b->side);
	U64 whiteKingAttacks = kingAtkMask[kSq] & ~move_s->attackedSquares & b->color[b->side ^ 1];

	while (whiteKingAttacks) {
		atk_sq = popBit(&whiteKingAttacks);
		move_s->moveList[move_s->moveCounter++] = MOVE(kSq, atk_sq, b->pieceAt(atk_sq), EMPTY, EMPTY);
	}
}

//void _addWhiteKingCaptures(Board board) {
//	U64 wKing = board.getPieces(KING, WHITE);
//	int wSq = popBit(&wKing);
//
//	U64 bKing = board.getPieces(KING, BLACK);
//	int bSq = popBit(&bKing);
//
//	U64 whiteKingAttacks = kingAtkMask[wSq] & ~kingAtkMask[bSq] & board.color[BLACK];
//	int atk_sq;
//
//	while (whiteKingAttacks) {
//		atk_sq = popBit(&whiteKingAttacks);
//
//		// check for non attacked square
//		if (board.squareAttacked(atk_sq, board.side ^ 1)) continue;
//		_capMoveList.push_back(Move(MOVE(wSq, atk_sq, board.pieceAt(atk_sq), EMPTY, EMPTY), 0));
//	}
//}
//
//void _addBlackKingCaptures(Board board) {
//	U64 bKing = board.getPieces(KING, BLACK);
//	int bSq = popBit(&bKing);
//
//	U64 wKing = board.getPieces(KING, WHITE);
//	int wSq = popBit(&wKing);
//
//	U64 blackKingAttacks = kingAtkMask[bSq] & ~kingAtkMask[wSq] & board.color[WHITE];
//	int atk_sq;
//
//	while (blackKingAttacks) {
//		atk_sq = popBit(&blackKingAttacks);
//
//		// check for non attacked square
//		if (board.squareAttacked(atk_sq, board.side ^ 1)) continue;
//		_capMoveList.push_back(Move(MOVE(bSq, atk_sq, board.pieceAt(atk_sq), EMPTY, EMPTY), 0));
//	}
//}

void _addRookMoves(Board* b, MOVE_S* move_s) {
	int sq, atk_sq, kSq = b->getKingSquare(b->side);
	U64 attackSet;
	U64 pinned = b->pinned(kSq, b->side);

	// LEGAL ADD, only use non pinned rooks in std gen
	U64 rooks = b->getPieces(ROOK, b->side) & ~pinned;
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

	// pinned rooks only move in pinned direction
	rooks = b->getPieces(ROOK, b->side) & pinned;
	while (rooks) {
		sq = popBit(&rooks);
		attackSet = lookUpRookMoves(sq, b->occupied) & lineBB[sq][kSq];
		attackSet &= ~b->occupied;
		while (attackSet) {
			atk_sq = popBit(&attackSet);
			move_s->moveList[move_s->moveCounter++] = MOVE(sq, atk_sq, EMPTY, EMPTY, EMPTY);
		}
	}
}

void _addBishopMoves(Board* b, MOVE_S* move_s) {
	int sq, atk_sq, kSq = b->getKingSquare(b->side);
	U64 pinned = b->pinned(kSq, b->side);

	U64 bishops = b->getPieces(BISHOP, b->side) & ~pinned;
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

	// pinned bishops only move in pinned direction
	bishops = b->getPieces(BISHOP, b->side) & pinned;
	while (bishops) {
		sq = popBit(&bishops);
		attackSet = lookUpBishopMoves(sq, b->occupied) & lineBB[sq][kSq];
		attackSet &= ~b->occupied;
		while (attackSet) {
			atk_sq = popBit(&attackSet);
			move_s->moveList[move_s->moveCounter++] = MOVE(sq, atk_sq, EMPTY, EMPTY, EMPTY);
		}
	}
}

void _addRookCaptures(Board* b, MOVE_S* move_s) {
	int sq, atk_sq, kSq = b->getKingSquare(b->side);
	U64 attackSet;
	U64 pinned = b->pinned(kSq, b->side);

	// LEGAL ADD, only use non pinned rooks in std gen
	U64 rooks = b->getPieces(ROOK, b->side) & ~pinned;
	while (rooks) {
		sq = popBit(&rooks);
		attackSet = lookUpRookMoves(sq, b->occupied);
		ASSERT(attackSet == calculateRookMoves(sq, b->occupied));
		attackSet &= b->color[b->side ^ 1];
		while (attackSet) {
			atk_sq = popBit(&attackSet);
			move_s->moveList[move_s->moveCounter++] = MOVE(sq, atk_sq, b->pieceAt(atk_sq), EMPTY, EMPTY);
		}
	}

	// pinned rooks only move in pinned direction
	rooks = b->getPieces(ROOK, b->side) & pinned;
	while (rooks) {
		sq = popBit(&rooks);
		attackSet = lookUpRookMoves(sq, b->occupied) & lineBB[sq][kSq];
		attackSet &= b->color[b->side ^ 1];
		while (attackSet) {
			atk_sq = popBit(&attackSet);
			move_s->moveList[move_s->moveCounter++] = MOVE(sq, atk_sq, b->pieceAt(atk_sq), EMPTY, EMPTY);
		}
	}
}

void _addBishopCaptures(Board* b, MOVE_S* move_s) {
	int sq, atk_sq, kSq = b->getKingSquare(b->side);
	U64 attackSet;
	U64 pinned = b->pinned(kSq, b->side);

	U64 bishops = b->getPieces(BISHOP, b->side) & ~pinned;
	while (bishops) {
		sq = popBit(&bishops);
		attackSet = lookUpBishopMoves(sq, b->occupied);
		attackSet &= b->color[b->side ^ 1];

		while (attackSet) {
			atk_sq = popBit(&attackSet);
			move_s->moveList[move_s->moveCounter++] = MOVE(sq, atk_sq, b->pieceAt(atk_sq), EMPTY, EMPTY);
		}
	}

	// pinned bishops only move in pinned direction
	bishops = b->getPieces(BISHOP, b->side) & pinned;
	while (bishops) {
		sq = popBit(&bishops);
		attackSet = lookUpBishopMoves(sq, b->occupied) & lineBB[sq][kSq];
		attackSet &= b->color[b->side ^ 1];
		while (attackSet) {
			atk_sq = popBit(&attackSet);
			move_s->moveList[move_s->moveCounter++] = MOVE(sq, atk_sq, b->pieceAt(atk_sq), EMPTY, EMPTY);
		}
	}
}

void _addQueenMoves(Board* b, MOVE_S* move_s) {
	int sq, atk_sq, kSq = b->getKingSquare(b->side);
	U64 attackSet;
	U64 pinned = b->pinned(kSq, b->side);

	U64 queens = b->getPieces(QUEEN, b->side) & ~pinned;
	while (queens) {
		sq = popBit(&queens);
		attackSet = lookUpRookMoves(sq, b->occupied) ^ lookUpBishopMoves(sq, b->occupied);
		attackSet &= ~b->occupied;

		while (attackSet) {
			atk_sq = popBit(&attackSet);
			move_s->moveList[move_s->moveCounter++] = MOVE(sq, atk_sq, EMPTY, EMPTY, EMPTY);
		}
	}

	queens = b->getPieces(QUEEN, b->side) & pinned;
	while (queens) {
		sq = popBit(&queens);
		attackSet = lookUpRookMoves(sq, b->occupied) ^ lookUpBishopMoves(sq, b->occupied);
		attackSet &= ~b->occupied & lineBB[sq][kSq];

		while (attackSet) {
			atk_sq = popBit(&attackSet);
			move_s->moveList[move_s->moveCounter++] = MOVE(sq, atk_sq, EMPTY, EMPTY, EMPTY);
		}
	}
}

void _addQueenCaptures(Board* b, MOVE_S* move_s) {
	int sq, atk_sq, kSq = b->getKingSquare(b->side);
	U64 attackSet;
	U64 pinned = b->pinned(kSq, b->side);

	U64 queens = b->getPieces(QUEEN, b->side) & ~pinned;
	while (queens) {
		sq = popBit(&queens);
		attackSet = lookUpRookMoves(sq, b->occupied) ^ lookUpBishopMoves(sq, b->occupied);
		attackSet &= b->color[b->side ^ 1];

		while (attackSet) {
			atk_sq = popBit(&attackSet);
			move_s->moveList[move_s->moveCounter++] = MOVE(sq, atk_sq, b->pieceAt(atk_sq), EMPTY, EMPTY);
		}
	}

	queens = b->getPieces(QUEEN, b->side) & pinned;
	while (queens) {
		sq = popBit(&queens);
		attackSet = lookUpRookMoves(sq, b->occupied) ^ lookUpBishopMoves(sq, b->occupied);
		attackSet &= b->color[b->side ^ 1] & lineBB[sq][kSq];

		while (attackSet) {
			atk_sq = popBit(&attackSet);
			move_s->moveList[move_s->moveCounter++] = MOVE(sq, atk_sq, b->pieceAt(atk_sq), EMPTY, EMPTY);
		}
	}
}

void _printGeneratedMoves(MOVE_S* move_s) {

	cout << "\nGenerated " << move_s->moveCounter << " moves: " << endl;

	for (int i = 0; i < move_s->moveCounter; i++) {
		printMove(move_s->moveList[i]);
	}
}
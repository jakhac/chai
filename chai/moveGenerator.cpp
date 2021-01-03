#include "moveGenerator.h"

void generateMoves(Board* b, moveList_t* moveList, bool inCheck) {
	moveList->cnt = 0;
	moveList->attackedSquares = b->attackerSet(b->side ^ 1);

	if (inCheck) {
		generateCheckEvasions(b, moveList);
		return;
	}
	Assert(!b->isCheck(b->side));


	if (b->side == WHITE) {
		whiteSinglePawnPush(b, moveList);
		whitePawnPushProm(b, moveList);
		whiteDoublePawnPush(b, moveList);
		whitePawnCaptures(b, moveList);
	} else {
		blackSinglePawnPush(b, moveList);
		blackPawnPushProm(b, moveList);
		blackDoublePawnPush(b, moveList);
		blackPawnCaptures(b, moveList);
	}

	addKnightMoves(b, moveList);
	addKnightCaptures(b, moveList);

	addBishopMoves(b, moveList);
	addBishopCaptures(b, moveList);

	addKingMoves(b, moveList);
	addKingCaptures(b, moveList);

	addRookMoves(b, moveList);
	addRookCaptures(b, moveList);

	addQueenMoves(b, moveList);
	addQueenCaptures(b, moveList);
}

void generateQuiescence(Board* b, moveList_t* moveList, bool inCheck) {
	moveList->cnt = 0;
	moveList->attackedSquares = b->attackerSet(b->side ^ 1);

	if (inCheck) {
		generateCheckEvasions(b, moveList);
		return;
	}
	Assert(!b->isCheck(b->side));

	if (b->side == WHITE) {
		whitePawnPushProm(b, moveList);
		whitePawnCaptures(b, moveList);

	} else {
		blackPawnPushProm(b, moveList);
		blackPawnCaptures(b, moveList);
	}

	addKnightCaptures(b, moveList);
	addBishopCaptures(b, moveList);
	addRookCaptures(b, moveList);
	addQueenCaptures(b, moveList);
	addKingCaptures(b, moveList);
}

void generateCheckEvasions(Board* b, moveList_t* moveList) {
	int kSq = b->getKingSquare(b->side);
	int blockerSq;
	bitboard_t attacker = b->squareAttackedBy(kSq, b->side ^ 1);

	Assert(b->isCheck(b->side));
	Assert(countBits(attacker) > 0);

	// Single Check: blocks or captures by non-pinned pieces are possible
	if (countBits(attacker) < 2) {
		int attackerSq = bitscanForward(attacker);
		int attackerPiece = b->pieceAt(attackerSq);

		// If sliding piece: Get attacking line -> check if a pieceType atkMask can block
		if (pieceRook[attackerPiece] || pieceBishopQueen[attackerPiece]) {
			bitboard_t attackingLine = obstructed(kSq, attackerSq) & ~b->occupied;

			while (attackingLine) {
				blockerSq = popBit(&attackingLine);
				addBlockersForSq(b, moveList, blockerSq);
			}
		}

		// Always: squareAttackedBy -> calculate attacks to piece that delivers check
		bitboard_t defender = b->squareAttackedBy(attackerSq, b->side);
		bitboard_t promDefends = 0ULL;

		// defenders that promote with capture
		if (attacker & (RANK_1_HEX | RANK_8_HEX)) {
			promDefends = defender & b->pieces[PAWN] & (RANK_2_HEX | RANK_7_HEX);
		}

		defender &= ~promDefends;
		int defenderSq;
		while (defender) {
			defenderSq = popBit(&defender);
			moveList->moves[moveList->cnt++] = serializeMove(defenderSq, attackerSq, attackerPiece, EMPTY, EMPTY);
		}

		// Promoting with capturing the checking piece
		while (promDefends) {
			int pieceOffset = (b->side == WHITE) ? 0 : 6;
			defenderSq = popBit(&promDefends);

			moveList->moves[moveList->cnt++] = serializeMove(defenderSq, attackerSq, attackerPiece, Q + pieceOffset, EMPTY);
			moveList->moves[moveList->cnt++] = serializeMove(defenderSq, attackerSq, attackerPiece, R + pieceOffset, EMPTY);
			moveList->moves[moveList->cnt++] = serializeMove(defenderSq, attackerSq, attackerPiece, B + pieceOffset, EMPTY);
			moveList->moves[moveList->cnt++] = serializeMove(defenderSq, attackerSq, attackerPiece, N + pieceOffset, EMPTY);
		}

		// if pawn is threatening check but can be captured en passant
		if (b->enPas) {
			if (b->side == WHITE) {
				if (b->enPas == attackerSq + 8 && b->pieceAt(attackerSq - 1) == P) {
					moveList->moves[moveList->cnt++] = serializeMove(attackerSq - 1, b->enPas, EMPTY, EMPTY, MFLAG_EP);
				}
				if (b->enPas == attackerSq + 8 && b->pieceAt(attackerSq + 1) == P) {
					moveList->moves[moveList->cnt++] = serializeMove(attackerSq + 1, b->enPas, EMPTY, EMPTY, MFLAG_EP);
				}
			} else {
				if (b->enPas == attackerSq - 8 && b->pieceAt(attackerSq - 1) == p) {
					moveList->moves[moveList->cnt++] = serializeMove(attackerSq - 1, b->enPas, EMPTY, EMPTY, MFLAG_EP);
				}
				if (b->enPas == attackerSq - 8 && b->pieceAt(attackerSq + 1) == p) {
					moveList->moves[moveList->cnt++] = serializeMove(attackerSq + 1, b->enPas, EMPTY, EMPTY, MFLAG_EP);
				}
			}
		}
	}

	// Always add king evasions to non-attacked squares
	addKingCheckEvasions(b, moveList);
}

void generateQuietCheckers(Board* b, moveList_t* moveList) {
	Assert(!b->isCheck(b->side));

	int sq, atk_sq;
	int oppkSq = b->getKingSquare(b->side ^ 1);
	int ownkSq = b->getKingSquare(b->side);
	bitboard_t pieces;
	bitboard_t kingBoard = setMask[oppkSq];

	// Pawn pushes that check the king
	bitboard_t pawns;
	if (b->side == WHITE) {
		pawns = (b->getPieces(PAWN, WHITE) << 8) & ~b->occupied;

		pieces = pawns;
		while (pieces) {
			sq = popBit(&pieces);
			if (pawnAtkMask[WHITE][sq] & kingBoard) {
				moveList->moves[moveList->cnt++] = serializeMove(sq - 8, sq, EMPTY, EMPTY, EMPTY);
			}
		}

		pieces = (pawns << 8) & ~b->occupied;
		while (pieces) {
			sq = popBit(&pieces);
			if (pawnAtkMask[WHITE][sq] & kingBoard) {
				moveList->moves[moveList->cnt++] = serializeMove(sq - 16, sq, EMPTY, EMPTY, EMPTY);
			}
		}
	} else {
		pawns = (b->getPieces(PAWN, BLACK) >> 8) & ~b->occupied;

		pieces = pawns;
		while (pieces) {
			sq = popBit(&pieces);
			if (pawnAtkMask[BLACK][sq] & kingBoard) {
				moveList->moves[moveList->cnt++] = serializeMove(sq + 8, sq, EMPTY, EMPTY, EMPTY);
			}
		}

		pieces = (pawns << 8) & ~b->occupied;
		while (pieces) {
			sq = popBit(&pieces);
			if (pawnAtkMask[BLACK][sq] & kingBoard) {
				moveList->moves[moveList->cnt++] = serializeMove(sq + 16, sq, EMPTY, EMPTY, EMPTY);
			}
		}
	}

	// Sliders that attack king: check if a piece can slide to kingAtkLines -> found quiet check
	bitboard_t kingAtkLines = lookUpBishopMoves(oppkSq, b->occupied);
	pieces = b->getPieces(BISHOP, b->side) | b->getPieces(QUEEN, b->side);

	bitboard_t diagMoves;
	while (pieces) {
		sq = popBit(&pieces);
		diagMoves = lookUpBishopMoves(sq, b->occupied) & ~b->occupied;
		diagMoves &= kingAtkLines;

		while (diagMoves) {
			atk_sq = popBit(&diagMoves);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, EMPTY, EMPTY, EMPTY);
		}
	}

	kingAtkLines = lookUpRookMoves(oppkSq, b->occupied);
	bitboard_t vertHoriMoves;
	pieces = b->getPieces(ROOK, b->side) | b->getPieces(QUEEN, b->side);
	while (pieces) {
		sq = popBit(&pieces);
		vertHoriMoves = lookUpRookMoves(sq, b->occupied) & ~b->occupied;
		vertHoriMoves &= kingAtkLines;

		while (vertHoriMoves) {
			atk_sq = popBit(&vertHoriMoves);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, EMPTY, EMPTY, EMPTY);
		}
	}

	// Knight moves that check king
	bitboard_t knightChecks;
	bitboard_t kingKnightPattern = knightAtkMask[oppkSq];
	pieces = b->getPieces(KNIGHT, b->side);
	while (pieces) {
		sq = popBit(&pieces);
		knightChecks = knightAtkMask[sq] & kingKnightPattern & ~b->occupied;

		while (knightChecks) {
			atk_sq = popBit(&knightChecks);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, EMPTY, EMPTY, EMPTY);
		}
	}
}

void addBlockersForSq(Board* b, moveList_t* moveList, int blockingSq) {
	bitboard_t blocker = b->blockerSet(b->side, blockingSq);

	int piece, sq;
	int flag;
	while (blocker) {
		flag = 0;
		sq = popBit(&blocker);
		piece = b->pieceAt(sq);
		Assert(piece != k && piece != K);
		Assert(pieceValid(b->pieceAt(sq)));

		if ((piece == P || piece == p) && abs(sq - blockingSq) == 16) {
			flag |= MFLAG_PS;
		}

		moveList->moves[moveList->cnt++] = serializeMove(sq, blockingSq, EMPTY, EMPTY, flag);
	}

}

// to validate hash move
bool isLegal(Board* b, const move_t move) {
	bool inCheck = b->isCheck(b->side);
	moveList_t moveList[1];
	generateMoves(b, moveList, inCheck);

	for (int i = 0; i < moveList->cnt; i++) {
		if (moveList->moves[i] == move) {
			return true;
		}
	}

	return false;
}

void whiteSinglePawnPush(Board* board, moveList_t* moveList) {
	int sq;
	bitboard_t pushedPawns = (board->getPieces(PAWN, WHITE) << 8) & ~board->occupied;

	// divide proms and normal pushes
	bitboard_t promPawns = pushedPawns & RANK_8_HEX;
	pushedPawns = pushedPawns & ~RANK_8_HEX;


	// normal pawn pushes
	while (pushedPawns) {
		sq = popBit(&pushedPawns);
		moveList->moves[moveList->cnt++] = serializeMove(sq - 8, sq, EMPTY, EMPTY, EMPTY);
	}
}

void blackSinglePawnPush(Board* board, moveList_t* moveList) {
	int sq;
	bitboard_t pushedPawns = (board->getPieces(PAWN, BLACK) >> 8) & ~board->occupied;


	// divide proms and normal pushes
	bitboard_t promPawns = pushedPawns & RANK_1_HEX;
	pushedPawns = pushedPawns & ~RANK_1_HEX;

	// normal pawn pushes
	while (pushedPawns) {
		sq = popBit(&pushedPawns);
		moveList->moves[moveList->cnt++] = serializeMove(sq + 8, sq, EMPTY, EMPTY, EMPTY);
	}
}

void whitePawnPushProm(Board* b, moveList_t* moveList) {
	bitboard_t pawns = b->getPieces(PAWN, WHITE);
	pawns = ((pawns & RANK_7_HEX) << 8) & ~b->occupied;

	int sq;
	while (pawns) {
		sq = popBit(&pawns);
		moveList->moves[moveList->cnt++] = serializeMove(sq - 8, sq, EMPTY, Q, EMPTY);
		moveList->moves[moveList->cnt++] = serializeMove(sq - 8, sq, EMPTY, R, EMPTY);
		moveList->moves[moveList->cnt++] = serializeMove(sq - 8, sq, EMPTY, B, EMPTY);
		moveList->moves[moveList->cnt++] = serializeMove(sq - 8, sq, EMPTY, N, EMPTY);
	}
}

void blackPawnPushProm(Board* board, moveList_t* moveList) {
	bitboard_t pawns = board->getPieces(PAWN, BLACK);
	pawns = ((pawns & RANK_2_HEX) >> 8) & ~board->occupied;

	int sq;
	while (pawns) {
		sq = popBit(&pawns);
		moveList->moves[moveList->cnt++] = serializeMove(sq + 8, sq, EMPTY, q, EMPTY);
		moveList->moves[moveList->cnt++] = serializeMove(sq + 8, sq, EMPTY, r, EMPTY);
		moveList->moves[moveList->cnt++] = serializeMove(sq + 8, sq, EMPTY, b, EMPTY);
		moveList->moves[moveList->cnt++] = serializeMove(sq + 8, sq, EMPTY, n, EMPTY);
	}
}

void whiteDoublePawnPush(Board* board, moveList_t* moveList) {
	int sq;
	bitboard_t pushedPawns = (board->getPieces(PAWN, WHITE) << 8) & ~board->occupied;
	pushedPawns = (pushedPawns << 8) & ~board->occupied & RANK_4_HEX;

	while (pushedPawns) {
		sq = popBit(&pushedPawns);
		moveList->moves[moveList->cnt++] = serializeMove(sq - 16, sq, EMPTY, EMPTY, MFLAG_PS);
	}
}

void blackDoublePawnPush(Board* board, moveList_t* moveList) {
	int sq;
	bitboard_t pushedPawns = (board->getPieces(PAWN, BLACK) >> 8) & ~board->occupied;
	pushedPawns = (pushedPawns >> 8) & ~board->occupied & RANK_5_HEX;

	while (pushedPawns) {
		sq = popBit(&pushedPawns);
		moveList->moves[moveList->cnt++] = serializeMove(sq + 16, sq, EMPTY, EMPTY, MFLAG_PS);
	}
}

void whitePawnCaptures(Board* board, moveList_t* moveList) {
	int sq, atk_sq;
	bitboard_t atks;

	//divide in prom and non prom attacks
	bitboard_t whitePawns = board->getPieces(PAWN, WHITE);
	bitboard_t whitePawnProm = whitePawns & RANK_7_HEX;
	whitePawns &= ~RANK_7_HEX;

	// en passant square
	if ((whitePawns << 7 & ~FILE_H_HEX) & setMask[board->enPas]) {
		moveList->moves[moveList->cnt++] = serializeMove(board->enPas - 7, board->enPas, EMPTY, EMPTY, MFLAG_EP);
	}
	if ((whitePawns << 9 & ~FILE_A_HEX) & setMask[board->enPas]) {
		moveList->moves[moveList->cnt++] = serializeMove(board->enPas - 9, board->enPas, EMPTY, EMPTY, MFLAG_EP);
	}

	while (whitePawns) {
		sq = popBit(&whitePawns);
		atks = pawnAtkMask[WHITE][sq] & board->color[BLACK];
		while (atks) {
			atk_sq = popBit(&atks);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, board->pieceAt(atk_sq), EMPTY, EMPTY);
		}
	}

	// promoting attacks
	while (whitePawnProm) {
		sq = popBit(&whitePawnProm);
		atks = pawnAtkMask[WHITE][sq] & board->color[BLACK];
		while (atks) {
			atk_sq = popBit(&atks);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, board->pieceAt(atk_sq), Q, EMPTY);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, board->pieceAt(atk_sq), R, EMPTY);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, board->pieceAt(atk_sq), B, EMPTY);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, board->pieceAt(atk_sq), N, EMPTY);
		}
	}
}

void blackPawnCaptures(Board* board, moveList_t* moveList) {
	int sq, atk_sq;
	bitboard_t atks;

	//divide in prom and non prom attacks
	bitboard_t blackPawns = board->getPieces(PAWN, BLACK);
	bitboard_t blackPawnProm = blackPawns & RANK_2_HEX;
	blackPawns &= ~RANK_2_HEX;

	// en passant square
	if ((blackPawns >> 7 & ~FILE_A_HEX) & setMask[board->enPas]) {
		moveList->moves[moveList->cnt++] = serializeMove(board->enPas + 7, board->enPas, EMPTY, EMPTY, MFLAG_EP);
	}
	if ((blackPawns >> 9 & ~FILE_H_HEX) & setMask[board->enPas]) {
		moveList->moves[moveList->cnt++] = serializeMove(board->enPas + 9, board->enPas, EMPTY, EMPTY, MFLAG_EP);
	}

	while (blackPawns) {
		sq = popBit(&blackPawns);
		atks = pawnAtkMask[BLACK][sq] & board->color[WHITE];
		while (atks) {
			atk_sq = popBit(&atks);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, board->pieceAt(atk_sq), EMPTY, EMPTY);
		}
	}

	// promoting attacks
	while (blackPawnProm) {
		sq = popBit(&blackPawnProm);
		atks = pawnAtkMask[BLACK][sq] & board->color[WHITE];
		while (atks) {
			atk_sq = popBit(&atks);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, board->pieceAt(atk_sq), q, EMPTY);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, board->pieceAt(atk_sq), r, EMPTY);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, board->pieceAt(atk_sq), b, EMPTY);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, board->pieceAt(atk_sq), n, EMPTY);
		}
	}
}

void addKnightMoves(Board* b, moveList_t* moveList) {
	int sq, atk_sq;
	int piece = (b->side == color_t::WHITE) ? N : n;
	bitboard_t knights = b->getPieces(KNIGHT, b->side);
	bitboard_t atks;

	while (knights) {
		sq = popBit(&knights);
		atks = knightAtkMask[sq] & ~b->occupied;
		while (atks) {
			atk_sq = popBit(&atks);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, EMPTY, EMPTY, EMPTY);
		}
	}
}

void addKnightCaptures(Board* b, moveList_t* moveList) {
	int sq, atk_sq;
	int piece = (b->side == color_t::WHITE) ? N : n;
	bitboard_t knights = b->getPieces(KNIGHT, b->side);
	bitboard_t atks;

	while (knights) {
		sq = popBit(&knights);
		atks = knightAtkMask[sq] & b->color[b->side ^ 1];
		while (atks) {
			atk_sq = popBit(&atks);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, b->pieceAt(atk_sq), EMPTY, EMPTY);
		}
	}
}

void addKingMoves(Board* b, moveList_t* moveList) {
	int sq, kSq = b->getKingSquare(b->side);
	int piece = (b->side == WHITE) ? K : k;
	bitboard_t kingMoves = kingAtkMask[kSq] & ~moveList->attackedSquares & ~b->occupied;

	while (kingMoves) {
		sq = popBit(&kingMoves);
		moveList->moves[moveList->cnt++] = serializeMove(kSq, sq, EMPTY, EMPTY, EMPTY);
	}

	switch (b->side) {
		case WHITE:
			if (b->castleValid(K_CASTLE, &moveList->attackedSquares)) {
				Assert(kSq == E1);
				moveList->moves[moveList->cnt++] = serializeMove(kSq, G1, EMPTY, EMPTY, MFLAG_CAS);
			}
			if (b->castleValid(Q_CASTLE, &moveList->attackedSquares)) {
				Assert(kSq == E1);
				moveList->moves[moveList->cnt++] = serializeMove(kSq, C1, EMPTY, EMPTY, MFLAG_CAS);
			}
			break;
		case BLACK:
			if (b->castleValid(k_CASTLE, &moveList->attackedSquares)) {
				Assert(kSq == E8);
				moveList->moves[moveList->cnt++] = serializeMove(kSq, G8, EMPTY, EMPTY, MFLAG_CAS);
			}

			if (b->castleValid(q_CASTLE, &moveList->attackedSquares)) {
				Assert(kSq == E8);
				moveList->moves[moveList->cnt++] = serializeMove(kSq, C8, EMPTY, EMPTY, MFLAG_CAS);
			}
			break;
		default: break;
	}
}

void addKingCaptures(Board* b, moveList_t* moveList) {
	int piece = (b->side == WHITE) ? K : k;
	int atk_sq, kSq = b->getKingSquare(b->side);
	bitboard_t whiteKingAttacks = kingAtkMask[kSq] & ~moveList->attackedSquares & b->color[b->side ^ 1];

	while (whiteKingAttacks) {
		atk_sq = popBit(&whiteKingAttacks);
		moveList->moves[moveList->cnt++] = serializeMove(kSq, atk_sq, b->pieceAt(atk_sq), EMPTY, EMPTY);
	}
}

void addRookMoves(Board* b, moveList_t* moveList) {
	int sq, atk_sq;
	int piece = (b->side == WHITE) ? R : r;

	bitboard_t attackSet;
	bitboard_t rooks = b->getPieces(ROOK, b->side);
	while (rooks) {
		sq = popBit(&rooks);
		attackSet = lookUpRookMoves(sq, b->occupied);
		Assert(attackSet == calculateRookMoves(sq, b->occupied));
		attackSet &= ~b->occupied;
		while (attackSet) {
			atk_sq = popBit(&attackSet);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, EMPTY, EMPTY, EMPTY);
		}
	}
}

void addBishopMoves(Board* b, moveList_t* moveList) {
	int sq, atk_sq;
	int piece = (b->side == WHITE) ? B : piece_t::b;

	bitboard_t bishops = b->getPieces(BISHOP, b->side);
	bitboard_t attackSet;
	while (bishops) {
		sq = popBit(&bishops);
		attackSet = lookUpBishopMoves(sq, b->occupied);
		attackSet &= ~b->occupied;

		while (attackSet) {
			atk_sq = popBit(&attackSet);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, EMPTY, EMPTY, EMPTY);
		}
	}
}

void addRookCaptures(Board* b, moveList_t* moveList) {
	int piece = (b->side == WHITE) ? R : r;
	int sq, atk_sq;
	bitboard_t captureSet;
	bitboard_t rooks = b->getPieces(ROOK, b->side);
	while (rooks) {
		sq = popBit(&rooks);
		captureSet = lookUpRookMoves(sq, b->occupied);
		captureSet = captureSet & b->color[b->side ^ 1];

		while (captureSet) {
			atk_sq = popBit(&captureSet);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, b->pieceAt(atk_sq), EMPTY, EMPTY);
		}
	}
}

void addBishopCaptures(Board* board, moveList_t* moveList) {
	int piece = (board->side == WHITE) ? B : b;
	int sq, atk_sq;
	bitboard_t captureSet;
	bitboard_t bishops = board->getPieces(BISHOP, board->side);
	while (bishops) {
		sq = popBit(&bishops);
		captureSet = lookUpBishopMoves(sq, board->occupied);
		captureSet &= board->color[board->side ^ 1];

		while (captureSet) {
			atk_sq = popBit(&captureSet);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, board->pieceAt(atk_sq), EMPTY, EMPTY);
		}
	}

}

void addQueenMoves(Board* b, moveList_t* moveList) {
	int sq, atk_sq;
	int piece = (b->side == WHITE) ? Q : q;

	bitboard_t attackSet;
	bitboard_t queen = b->getPieces(QUEEN, b->side);
	while (queen) {
		sq = popBit(&queen);
		attackSet = lookUpRookMoves(sq, b->occupied) ^ lookUpBishopMoves(sq, b->occupied);
		attackSet &= ~b->occupied;

		while (attackSet) {
			atk_sq = popBit(&attackSet);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, EMPTY, EMPTY, EMPTY);
		}
	}
}

void addQueenCaptures(Board* b, moveList_t* moveList) {
	int piece = (b->side == WHITE) ? Q : q;
	int sq, atk_sq;
	bitboard_t attackSet;
	bitboard_t queen = b->getPieces(QUEEN, b->side);
	while (queen) {
		sq = popBit(&queen);
		attackSet = lookUpRookMoves(sq, b->occupied) ^ lookUpBishopMoves(sq, b->occupied);
		attackSet &= b->color[b->side ^ 1];

		while (attackSet) {
			atk_sq = popBit(&attackSet);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, b->pieceAt(atk_sq), EMPTY, EMPTY);
		}
	}
}

void printGeneratedMoves(moveList_t* moveList) {

	cout << "\nGenerated " << moveList->cnt << " moves: " << endl;

	for (int i = 0; i < moveList->cnt; i++) {
		cout << i << ". " << getStringMove(moveList->moves[i]) << " score: " << moveList->scores[i] << endl;
	}
}

void addKingCheckEvasions(Board* b, moveList_t* moveList) {
	Assert(moveList->attackedSquares == b->attackerSet(b->side ^ 1));
	int piece = (b->side) == WHITE ? K : k;
	int kSq = b->getKingSquare(b->side);
	int to;

	b->clearPiece(piece, kSq, b->side);
	bitboard_t atkSquares = b->attackerSet(b->side ^ 1);
	b->setPiece(piece, kSq, b->side);

	bitboard_t quietSquares = kingAtkMask[kSq] & ~atkSquares & ~b->occupied;
	bitboard_t capSquares = kingAtkMask[kSq] & ~atkSquares & b->color[b->side ^ 1];

	// generate quiet check evasions
	while (quietSquares) {
		to = popBit(&quietSquares);
		moveList->moves[moveList->cnt++] = serializeMove(kSq, to, EMPTY, EMPTY, EMPTY);
	}

	// generate capturing check evasions
	while (capSquares) {
		to = popBit(&capSquares);
		moveList->moves[moveList->cnt++] = serializeMove(kSq, to, b->pieceAt(to), EMPTY, EMPTY);
	}
}

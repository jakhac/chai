#include "moveGenerator.h"

void generateMoves(board_t* b, moveList_t* moveList, bool inCheck) {
	moveList->cnt = 0;
	moveList->attackedSquares = attackerSet(b, b->side ^ 1);

	if (inCheck) {
		generateCheckEvasions(b, moveList);
		return;
	}
	Assert(!isCheck(b, b->side));


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

void generateQuiescence(board_t* b, moveList_t* moveList, bool inCheck) {
	moveList->cnt = 0;
	moveList->attackedSquares = attackerSet(b, b->side ^ 1);

	if (inCheck) {
		generateCheckEvasions(b, moveList);
		return;
	}
	Assert(!isCheck(b, b->side));

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

void generateCheckEvasions(board_t* b, moveList_t* moveList) {
	int kSq = getKingSquare(b, b->side);
	int blockerSq;
	bitboard_t attacker = squareAttackedBy(b, kSq, b->side ^ 1);

	Assert(isCheck(b, b->side));
	Assert(countBits(attacker) > 0);

	// Single Check: blocks or captures by non-pinned pieces are possible
	if (countBits(attacker) < 2) {
		int attackerSq = bitscanForward(attacker);
		int attackerPiece = pieceAt(b, attackerSq);

		// If sliding piece: Get attacking line -> check if a pieceType atkMask can block
		if (pieceRook[attackerPiece] || pieceBishopQueen[attackerPiece]) {
			bitboard_t attackingLine = obstructed(kSq, attackerSq) & ~b->occupied;

			while (attackingLine) {

				blockerSq = popBit(&attackingLine);
				addBlockersForSq(b, moveList, blockerSq);
			}
		}

		// Always: squareAttackedBy -> calculate attacks to piece that delivers check
		bitboard_t defender = squareAttackedBy(b, attackerSq, b->side);
		bitboard_t promDefends = 0ULL;

		// defenders that promote with capture
		if (attacker & (RANK_1_HEX | RANK_8_HEX)) {
			promDefends = defender & b->pieces[Piece::PAWN] & (RANK_2_HEX | RANK_7_HEX);
		}

		defender &= ~promDefends;
		int defenderSq;
		while (defender) {
			defenderSq = popBit(&defender);
			moveList->moves[moveList->cnt++] = serializeMove(defenderSq, attackerSq, attackerPiece, Piece::EMPTY, Piece::EMPTY);
		}

		// Promoting with capturing the checking piece
		while (promDefends) {
			int pieceOffset = (b->side == WHITE) ? 0 : 6;
			defenderSq = popBit(&promDefends);

			moveList->moves[moveList->cnt++] = serializeMove(defenderSq, attackerSq, attackerPiece, Piece::Q + pieceOffset, Piece::EMPTY);
			moveList->moves[moveList->cnt++] = serializeMove(defenderSq, attackerSq, attackerPiece, Piece::R + pieceOffset, Piece::EMPTY);
			moveList->moves[moveList->cnt++] = serializeMove(defenderSq, attackerSq, attackerPiece, Piece::B + pieceOffset, Piece::EMPTY);
			moveList->moves[moveList->cnt++] = serializeMove(defenderSq, attackerSq, attackerPiece, Piece::N + pieceOffset, Piece::EMPTY);
		}

		// if pawn is threatening check but can be captured en passant
		if (b->enPas) {
			if (b->side == WHITE) {
				if (b->enPas == attackerSq + 8 && pieceAt(b, attackerSq - 1) == Piece::P) {
					moveList->moves[moveList->cnt++] = serializeMove(attackerSq - 1, b->enPas, Piece::EMPTY, Piece::EMPTY, MFLAG_EP);
				}
				if (b->enPas == attackerSq + 8 && pieceAt(b, attackerSq + 1) == Piece::P) {
					moveList->moves[moveList->cnt++] = serializeMove(attackerSq + 1, b->enPas, Piece::EMPTY, Piece::EMPTY, MFLAG_EP);
				}
			} else {
				if (b->enPas == attackerSq - 8 && pieceAt(b, attackerSq - 1) == Piece::p) {
					moveList->moves[moveList->cnt++] = serializeMove(attackerSq - 1, b->enPas, Piece::EMPTY, Piece::EMPTY, MFLAG_EP);
				}
				if (b->enPas == attackerSq - 8 && pieceAt(b, attackerSq + 1) == Piece::p) {
					moveList->moves[moveList->cnt++] = serializeMove(attackerSq + 1, b->enPas, Piece::EMPTY, Piece::EMPTY, MFLAG_EP);
				}
			}
		}
	}

	// Always add king evasions to non-attacked squares
	addKingCheckEvasions(b, moveList);
}

void generateQuietCheckers(board_t* b, moveList_t* moveList) {
	Assert(!isCheck(b, b->side));

	int sq, atk_sq;
	int oppkSq = getKingSquare(b, b->side ^ 1);
	int ownkSq = getKingSquare(b, b->side);
	bitboard_t pieces;
	bitboard_t kingBoard = setMask[oppkSq];

	// Pawn pushes that check the king
	bitboard_t pawns;
	if (b->side == WHITE) {
		pawns = (getPieces(b, Piece::PAWN, WHITE) << 8) & ~b->occupied;

		pieces = pawns;
		while (pieces) {
			sq = popBit(&pieces);
			if (pawnAtkMask[WHITE][sq] & kingBoard) {
				moveList->moves[moveList->cnt++] = serializeMove(sq - 8, sq, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY);
			}
		}

		pieces = (pawns << 8) & ~b->occupied;
		while (pieces) {
			sq = popBit(&pieces);
			if (pawnAtkMask[WHITE][sq] & kingBoard) {
				moveList->moves[moveList->cnt++] = serializeMove(sq - 16, sq, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY);
			}
		}
	} else {
		pawns = (getPieces(b, Piece::PAWN, BLACK) >> 8) & ~b->occupied;

		pieces = pawns;
		while (pieces) {
			sq = popBit(&pieces);
			if (pawnAtkMask[BLACK][sq] & kingBoard) {
				moveList->moves[moveList->cnt++] = serializeMove(sq + 8, sq, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY);
			}
		}

		pieces = (pawns << 8) & ~b->occupied;
		while (pieces) {
			sq = popBit(&pieces);
			if (pawnAtkMask[BLACK][sq] & kingBoard) {
				moveList->moves[moveList->cnt++] = serializeMove(sq + 16, sq, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY);
			}
		}
	}

	// Sliders that attack king: check if a piece can slide to kingAtkLines -> found quiet check
	bitboard_t kingAtkLines = lookUpBishopMoves(oppkSq, b->occupied);
	pieces = getPieces(b, Piece::BISHOP, b->side) | getPieces(b, Piece::QUEEN, b->side);

	bitboard_t diagMoves;
	while (pieces) {
		sq = popBit(&pieces);
		diagMoves = lookUpBishopMoves(sq, b->occupied) & ~b->occupied;
		diagMoves &= kingAtkLines;

		while (diagMoves) {
			atk_sq = popBit(&diagMoves);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY);
		}
	}

	kingAtkLines = lookUpRookMoves(oppkSq, b->occupied);
	bitboard_t vertHoriMoves;
	pieces = getPieces(b, Piece::ROOK, b->side) | getPieces(b, Piece::QUEEN, b->side);
	while (pieces) {
		sq = popBit(&pieces);
		vertHoriMoves = lookUpRookMoves(sq, b->occupied) & ~b->occupied;
		vertHoriMoves &= kingAtkLines;

		while (vertHoriMoves) {
			atk_sq = popBit(&vertHoriMoves);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY);
		}
	}

	// Knight moves that check king
	bitboard_t knightChecks;
	bitboard_t kingKnightPattern = knightAtkMask[oppkSq];
	pieces = getPieces(b, Piece::KNIGHT, b->side);
	while (pieces) {
		sq = popBit(&pieces);
		knightChecks = knightAtkMask[sq] & kingKnightPattern & ~b->occupied;

		while (knightChecks) {
			atk_sq = popBit(&knightChecks);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY);
		}
	}
}

void addBlockersForSq(board_t* b, moveList_t* moveList, int blockingSq) {
	bitboard_t blocker = blockerSet(b, b->side, blockingSq);

	int piece, sq;
	int flag;
	while (blocker) {


		flag = 0;
		sq = popBit(&blocker);
		piece = pieceAt(b, sq);
		Assert(piece != Piece::k && piece != Piece::K);
		Assert(pieceValid(pieceAt(b, sq)));

		if ((piece == Piece::P || piece == Piece::p) && abs(sq - blockingSq) == 16) {
			flag |= MFLAG_PS;
		}

		moveList->moves[moveList->cnt++] = serializeMove(sq, blockingSq, Piece::EMPTY, Piece::EMPTY, flag);
	}

}

// to validate hash move
bool isLegal(board_t* b, const move_t move) {
	bool inCheck = isCheck(b, b->side);
	moveList_t moveList[1];
	generateMoves(b, moveList, inCheck);

	for (int i = 0; i < moveList->cnt; i++) {
		if (moveList->moves[i] == move) {
			return true;
		}
	}

	return false;
}

bitboard_t hasSafePawnPush(board_t* b, int side) {
	bitboard_t safeSquares = 0ULL;
	bitboard_t pawns = getPieces(b, Piece::PAWN, side);
	bitboard_t oppPawns = getPieces(b, Piece::PAWN, side ^ 1);

	int sq;
	bitboard_t defendedSquares = 0ULL;
	while (oppPawns) {
		sq = popBit(&oppPawns);
		defendedSquares |= pawnAtkMask[side ^ 1][sq];
	}

	// Look for: single pawn pushes, non blocked and not defended by opposite pawns
	if (side == WHITE) {
		safeSquares |= (pawns << 8) & ~b->occupied & ~defendedSquares;

		pawns &= RANK_2_HEX;
		safeSquares |= (((pawns << 8) & ~b->occupied) << 8) & ~defendedSquares;
	} else {
		safeSquares |= (pawns >> 8) & ~b->occupied & ~defendedSquares;

		pawns &= RANK_7_HEX;
		safeSquares |= (((pawns >> 8) & ~b->occupied) >> 8) & ~defendedSquares;
	}

	return safeSquares;
}

void whiteSinglePawnPush(board_t* board, moveList_t* moveList) {
	int sq;
	bitboard_t pushedPawns = (getPieces(board, Piece::PAWN, WHITE) << 8) & ~board->occupied;

	// divide proms and normal pushes
	bitboard_t promPawns = pushedPawns & RANK_8_HEX;
	pushedPawns = pushedPawns & ~RANK_8_HEX;


	// normal pawn pushes
	while (pushedPawns) {
		sq = popBit(&pushedPawns);
		moveList->moves[moveList->cnt++] = serializeMove(sq - 8, sq, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY);
	}
}

void blackSinglePawnPush(board_t* board, moveList_t* moveList) {
	int sq;
	bitboard_t pushedPawns = (getPieces(board, Piece::PAWN, BLACK) >> 8) & ~board->occupied;


	// divide proms and normal pushes
	bitboard_t promPawns = pushedPawns & RANK_1_HEX;
	pushedPawns = pushedPawns & ~RANK_1_HEX;

	// normal pawn pushes
	while (pushedPawns) {
		sq = popBit(&pushedPawns);
		moveList->moves[moveList->cnt++] = serializeMove(sq + 8, sq, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY);
	}
}

void whitePawnPushProm(board_t* b, moveList_t* moveList) {
	bitboard_t pawns = getPieces(b, Piece::PAWN, WHITE);
	pawns = ((pawns & RANK_7_HEX) << 8) & ~b->occupied;

	int sq;
	while (pawns) {
		sq = popBit(&pawns);
		moveList->moves[moveList->cnt++] = serializeMove(sq - 8, sq, Piece::EMPTY, Piece::Q, Piece::EMPTY);
		moveList->moves[moveList->cnt++] = serializeMove(sq - 8, sq, Piece::EMPTY, Piece::R, Piece::EMPTY);
		moveList->moves[moveList->cnt++] = serializeMove(sq - 8, sq, Piece::EMPTY, Piece::B, Piece::EMPTY);
		moveList->moves[moveList->cnt++] = serializeMove(sq - 8, sq, Piece::EMPTY, Piece::N, Piece::EMPTY);
	}
}

void blackPawnPushProm(board_t* board, moveList_t* moveList) {
	bitboard_t pawns = getPieces(board, Piece::PAWN, BLACK);
	pawns = ((pawns & RANK_2_HEX) >> 8) & ~board->occupied;

	int sq;
	while (pawns) {
		sq = popBit(&pawns);
		moveList->moves[moveList->cnt++] = serializeMove(sq + 8, sq, Piece::EMPTY, Piece::q, Piece::EMPTY);
		moveList->moves[moveList->cnt++] = serializeMove(sq + 8, sq, Piece::EMPTY, Piece::r, Piece::EMPTY);
		moveList->moves[moveList->cnt++] = serializeMove(sq + 8, sq, Piece::EMPTY, Piece::b, Piece::EMPTY);
		moveList->moves[moveList->cnt++] = serializeMove(sq + 8, sq, Piece::EMPTY, Piece::n, Piece::EMPTY);
	}
}

void whiteDoublePawnPush(board_t* board, moveList_t* moveList) {
	int sq;
	bitboard_t pushedPawns = (getPieces(board, Piece::PAWN, WHITE) << 8) & ~board->occupied;
	pushedPawns = (pushedPawns << 8) & ~board->occupied & RANK_4_HEX;

	while (pushedPawns) {
		sq = popBit(&pushedPawns);
		moveList->moves[moveList->cnt++] = serializeMove(sq - 16, sq, Piece::EMPTY, Piece::EMPTY, MFLAG_PS);
	}
}

void blackDoublePawnPush(board_t* board, moveList_t* moveList) {
	int sq;
	bitboard_t pushedPawns = (getPieces(board, Piece::PAWN, BLACK) >> 8) & ~board->occupied;
	pushedPawns = (pushedPawns >> 8) & ~board->occupied & RANK_5_HEX;

	while (pushedPawns) {
		sq = popBit(&pushedPawns);
		moveList->moves[moveList->cnt++] = serializeMove(sq + 16, sq, Piece::EMPTY, Piece::EMPTY, MFLAG_PS);
	}
}

void whitePawnCaptures(board_t* board, moveList_t* moveList) {
	int sq, atk_sq;
	bitboard_t atks;

	//divide in prom and non prom attacks
	bitboard_t whitePawns = getPieces(board, Piece::PAWN, WHITE);
	bitboard_t whitePawnProm = whitePawns & RANK_7_HEX;
	whitePawns &= ~RANK_7_HEX;

	// en passant square
	if ((whitePawns << 7 & ~FILE_H_HEX) & setMask[board->enPas]) {
		moveList->moves[moveList->cnt++] = serializeMove(board->enPas - 7, board->enPas, Piece::EMPTY, Piece::EMPTY, MFLAG_EP);
	}
	if ((whitePawns << 9 & ~FILE_A_HEX) & setMask[board->enPas]) {
		moveList->moves[moveList->cnt++] = serializeMove(board->enPas - 9, board->enPas, Piece::EMPTY, Piece::EMPTY, MFLAG_EP);
	}

	while (whitePawns) {
		sq = popBit(&whitePawns);
		atks = pawnAtkMask[WHITE][sq] & board->color[BLACK];
		while (atks) {
			atk_sq = popBit(&atks);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, pieceAt(board, atk_sq), Piece::EMPTY, Piece::EMPTY);
		}
	}

	// promoting attacks
	while (whitePawnProm) {
		sq = popBit(&whitePawnProm);
		atks = pawnAtkMask[WHITE][sq] & board->color[BLACK];
		while (atks) {
			atk_sq = popBit(&atks);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, pieceAt(board, atk_sq), Piece::Q, Piece::EMPTY);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, pieceAt(board, atk_sq), Piece::R, Piece::EMPTY);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, pieceAt(board, atk_sq), Piece::B, Piece::EMPTY);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, pieceAt(board, atk_sq), Piece::N, Piece::EMPTY);
		}
	}
}

void blackPawnCaptures(board_t* board, moveList_t* moveList) {
	int sq, atk_sq;
	bitboard_t atks;

	//divide in prom and non prom attacks
	bitboard_t blackPawns = getPieces(board, Piece::PAWN, BLACK);
	bitboard_t blackPawnProm = blackPawns & RANK_2_HEX;
	blackPawns &= ~RANK_2_HEX;

	// en passant square
	if ((blackPawns >> 7 & ~FILE_A_HEX) & setMask[board->enPas]) {
		moveList->moves[moveList->cnt++] = serializeMove(board->enPas + 7, board->enPas, Piece::EMPTY, Piece::EMPTY, MFLAG_EP);
	}
	if ((blackPawns >> 9 & ~FILE_H_HEX) & setMask[board->enPas]) {
		moveList->moves[moveList->cnt++] = serializeMove(board->enPas + 9, board->enPas, Piece::EMPTY, Piece::EMPTY, MFLAG_EP);
	}

	while (blackPawns) {
		sq = popBit(&blackPawns);
		atks = pawnAtkMask[BLACK][sq] & board->color[WHITE];
		while (atks) {
			atk_sq = popBit(&atks);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, pieceAt(board, atk_sq), Piece::EMPTY, Piece::EMPTY);
		}
	}

	// promoting attacks
	while (blackPawnProm) {
		sq = popBit(&blackPawnProm);
		atks = pawnAtkMask[BLACK][sq] & board->color[WHITE];
		while (atks) {
			atk_sq = popBit(&atks);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, pieceAt(board, atk_sq), Piece::q, Piece::EMPTY);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, pieceAt(board, atk_sq), Piece::r, Piece::EMPTY);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, pieceAt(board, atk_sq), Piece::b, Piece::EMPTY);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, pieceAt(board, atk_sq), Piece::n, Piece::EMPTY);
		}
	}
}

void addKnightMoves(board_t* b, moveList_t* moveList) {
	int sq, atk_sq;
	int piece = (b->side == color_t::WHITE) ? Piece::N : Piece::n;
	bitboard_t knights = getPieces(b, Piece::KNIGHT, b->side);
	bitboard_t atks;

	while (knights) {
		sq = popBit(&knights);
		atks = knightAtkMask[sq] & ~b->occupied;
		while (atks) {
			atk_sq = popBit(&atks);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY);
		}
	}
}

void addKnightCaptures(board_t* b, moveList_t* moveList) {
	int sq, atk_sq;
	int piece = (b->side == color_t::WHITE) ? Piece::N : Piece::n;
	bitboard_t knights = getPieces(b, Piece::KNIGHT, b->side);
	bitboard_t atks;

	while (knights) {
		sq = popBit(&knights);
		atks = knightAtkMask[sq] & b->color[b->side ^ 1];
		while (atks) {
			atk_sq = popBit(&atks);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, pieceAt(b, atk_sq), Piece::EMPTY, Piece::EMPTY);
		}
	}
}

void addKingMoves(board_t* b, moveList_t* moveList) {
	int sq, kSq = getKingSquare(b, b->side);
	int piece = (b->side == WHITE) ? Piece::K : Piece::k;
	bitboard_t kingMoves = kingAtkMask[kSq] & ~moveList->attackedSquares & ~b->occupied;

	while (kingMoves) {
		sq = popBit(&kingMoves);
		moveList->moves[moveList->cnt++] = serializeMove(kSq, sq, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY);
	}

	switch (b->side) {
		case WHITE:
			if (castleValid(b, K_CASTLE, &moveList->attackedSquares)) {
				Assert(kSq == E1);
				moveList->moves[moveList->cnt++] = serializeMove(kSq, G1, Piece::EMPTY, Piece::EMPTY, MFLAG_CAS);
			}
			if (castleValid(b, Q_CASTLE, &moveList->attackedSquares)) {
				Assert(kSq == E1);
				moveList->moves[moveList->cnt++] = serializeMove(kSq, C1, Piece::EMPTY, Piece::EMPTY, MFLAG_CAS);
			}
			break;
		case BLACK:
			if (castleValid(b, k_CASTLE, &moveList->attackedSquares)) {
				Assert(kSq == E8);
				moveList->moves[moveList->cnt++] = serializeMove(kSq, G8, Piece::EMPTY, Piece::EMPTY, MFLAG_CAS);
			}

			if (castleValid(b, q_CASTLE, &moveList->attackedSquares)) {
				Assert(kSq == E8);
				moveList->moves[moveList->cnt++] = serializeMove(kSq, C8, Piece::EMPTY, Piece::EMPTY, MFLAG_CAS);
			}
			break;
		default: break;
	}
}

void addKingCaptures(board_t* b, moveList_t* moveList) {
	int piece = (b->side == WHITE) ? Piece::K : Piece::k;
	int atk_sq, kSq = getKingSquare(b, b->side);
	bitboard_t whiteKingAttacks = kingAtkMask[kSq] & ~moveList->attackedSquares & b->color[b->side ^ 1];

	while (whiteKingAttacks) {
		atk_sq = popBit(&whiteKingAttacks);
		moveList->moves[moveList->cnt++] = serializeMove(kSq, atk_sq, pieceAt(b, atk_sq), Piece::EMPTY, Piece::EMPTY);
	}
}

void addRookMoves(board_t* b, moveList_t* moveList) {
	int sq, atk_sq;
	int piece = (b->side == WHITE) ? Piece::R : Piece::r;

	bitboard_t attackSet;
	bitboard_t rooks = getPieces(b, Piece::ROOK, b->side);
	while (rooks) {
		sq = popBit(&rooks);
		attackSet = lookUpRookMoves(sq, b->occupied);
		Assert(attackSet == calculateRookMoves(sq, b->occupied));
		attackSet &= ~b->occupied;
		while (attackSet) {
			atk_sq = popBit(&attackSet);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY);
		}
	}
}

void addBishopMoves(board_t* b, moveList_t* moveList) {
	int sq, atk_sq;
	int piece = (b->side == WHITE) ? Piece::B : Piece::b;

	bitboard_t bishops = getPieces(b, Piece::BISHOP, b->side);
	bitboard_t attackSet;
	while (bishops) {
		sq = popBit(&bishops);
		attackSet = lookUpBishopMoves(sq, b->occupied);
		attackSet &= ~b->occupied;

		while (attackSet) {
			atk_sq = popBit(&attackSet);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY);
		}
	}
}

void addRookCaptures(board_t* b, moveList_t* moveList) {
	int piece = (b->side == WHITE) ? Piece::R : Piece::r;
	int sq, atk_sq;
	bitboard_t captureSet;
	bitboard_t rooks = getPieces(b, Piece::ROOK, b->side);
	while (rooks) {
		sq = popBit(&rooks);
		captureSet = lookUpRookMoves(sq, b->occupied);
		captureSet = captureSet & b->color[b->side ^ 1];

		while (captureSet) {
			atk_sq = popBit(&captureSet);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, pieceAt(b, atk_sq), Piece::EMPTY, Piece::EMPTY);
		}
	}
}

void addBishopCaptures(board_t* board, moveList_t* moveList) {
	int piece = (board->side == WHITE) ? Piece::B : Piece::b;
	int sq, atk_sq;
	bitboard_t captureSet;
	bitboard_t bishops = getPieces(board, Piece::BISHOP, board->side);
	while (bishops) {
		sq = popBit(&bishops);
		captureSet = lookUpBishopMoves(sq, board->occupied);
		captureSet &= board->color[board->side ^ 1];

		while (captureSet) {
			atk_sq = popBit(&captureSet);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, pieceAt(board, atk_sq), Piece::EMPTY, Piece::EMPTY);
		}
	}

}

void addQueenMoves(board_t* b, moveList_t* moveList) {
	int sq, atk_sq;
	int piece = (b->side == WHITE) ? Piece::Q : Piece::q;

	bitboard_t attackSet;
	bitboard_t queen = getPieces(b, Piece::QUEEN, b->side);
	while (queen) {
		sq = popBit(&queen);
		attackSet = lookUpRookMoves(sq, b->occupied) ^ lookUpBishopMoves(sq, b->occupied);
		attackSet &= ~b->occupied;

		while (attackSet) {
			atk_sq = popBit(&attackSet);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY);
		}
	}
}

void addQueenCaptures(board_t* b, moveList_t* moveList) {
	int piece = (b->side == WHITE) ? Piece::Q : Piece::q;
	int sq, atk_sq;
	bitboard_t attackSet;
	bitboard_t queen = getPieces(b, Piece::QUEEN, b->side);
	while (queen) {
		sq = popBit(&queen);
		attackSet = lookUpRookMoves(sq, b->occupied) ^ lookUpBishopMoves(sq, b->occupied);
		attackSet &= b->color[b->side ^ 1];

		while (attackSet) {
			atk_sq = popBit(&attackSet);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, pieceAt(b, atk_sq), Piece::EMPTY, Piece::EMPTY);
		}
	}
}

void printGeneratedMoves(moveList_t* moveList) {

	cout << "\nGenerated " << moveList->cnt << " moves: " << endl;

	for (int i = 0; i < moveList->cnt; i++) {
		cout << i << ". " << getStringMove(moveList->moves[i]) << " score: " << moveList->scores[i] << endl;
	}
}

void addKingCheckEvasions(board_t* b, moveList_t* moveList) {
	Assert(moveList->attackedSquares == attackerSet(b, b->side ^ 1));
	int piece = (b->side) == WHITE ? Piece::K : Piece::k;
	int kSq = getKingSquare(b, b->side);
	int to;

	clearPiece(b, piece, kSq, b->side);
	bitboard_t atkSquares = attackerSet(b, b->side ^ 1);
	setPiece(b, piece, kSq, b->side);

	bitboard_t quietSquares = kingAtkMask[kSq] & ~atkSquares & ~b->occupied;
	bitboard_t capSquares = kingAtkMask[kSq] & ~atkSquares & b->color[b->side ^ 1];

	// generate quiet check evasions
	while (quietSquares) {
		to = popBit(&quietSquares);
		moveList->moves[moveList->cnt++] = serializeMove(kSq, to, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY);
	}

	// generate capturing check evasions
	while (capSquares) {
		to = popBit(&capSquares);
		moveList->moves[moveList->cnt++] = serializeMove(kSq, to, pieceAt(b, to), Piece::EMPTY, Piece::EMPTY);
	}
}

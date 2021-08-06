#include "moveGenerator.h"

using namespace chai;

void generateMoves(board_t* b, moveList_t* moveList, bool inCheck) {
	moveList->cnt = 0;
	moveList->attackedSquares = attackerSet(b, b->stm ^ 1);

	if (inCheck) {
		generateCheckEvasions(b, moveList);
		return;
	}
	Assert(!isCheck(b, b->stm));


	if (b->stm == WHITE) {
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
	moveList->attackedSquares = attackerSet(b, b->stm ^ 1);

	if (inCheck) {
		generateCheckEvasions(b, moveList);
		return;
	}
	Assert(!isCheck(b, b->stm));

	if (b->stm == WHITE) {
		whitePawnPushPromQ(b, moveList);
		whitePawnCaptures(b, moveList);

	} else {
		blackPawnPushPromQ(b, moveList);
		blackPawnCaptures(b, moveList);
	}

	addKnightCaptures(b, moveList);
	addBishopCaptures(b, moveList);
	addRookCaptures(b, moveList);
	addQueenCaptures(b, moveList);
	addKingCaptures(b, moveList);
}

void generateCheckEvasions(board_t* b, moveList_t* moveList) {
	// Note, no need to generate attackerSet as this function is called in two cases:
	//		1. genQui / genAll: Then attackerSet is already calculated
	//		2. hasEvadingMove(): attackerSet is calculated there

	int kSq = getKingSquare(b, b->stm);
	int blockerSq;
	bitboard_t pinnedDefender = getPinned(b, kSq, b->stm);
	bitboard_t attacker = squareAttackedBy(b, kSq, b->stm ^ 1);

	Assert(isCheck(b, b->stm));
	Assert(popCount(attacker) > 0);

	// Step 1: Single check
	// Either block only sliding attacker ...
	// ... or capture checker with non-pinned pieces
	if (popCount(attacker) < 2) {
		// The attacker, that exclusivly checks stm->king
		int attackerSq = getLSB(attacker);
		int attackerPiece = pieceAt(b, attackerSq);

		// Step 1.1: Blocking
		// If attacker is sliding piece: Get attacking line -> check if a pieceType atkMask can block
		if (pieceRook[attackerPiece] || pieceBishopQueen[attackerPiece]) {
			bitboard_t attackingLine = obstructed(kSq, attackerSq) & ~b->occupied;

			while (attackingLine) {
				blockerSq = popLSB(&attackingLine);
				addBlockersForSq(b, moveList, blockerSq, &pinnedDefender);
			}
		}

		// Step 1.2: Capture checker
		// Calculate attacks to square that delivers check (by non-pinned pieces!)
		bitboard_t defender = squareAttackedBy(b, attackerSq, b->stm) & ~pinnedDefender;
		bitboard_t promDefends = 0ULL;

		// defenders that defend while promoting with capture
		if (attacker & (RANK_1_HEX | RANK_8_HEX)) {
			promDefends = defender & b->pieces[PAWN] & (RANK_2_HEX | RANK_7_HEX);
		}

		// Remove promotions from standard captures
		defender &= ~promDefends;

		// Non-Promoting moves, that capture the checker
		int defenderSq;
		while (defender) {
			defenderSq = popLSB(&defender);
			moveList->moves[moveList->cnt++] = serializeMove(defenderSq, attackerSq, NORMAL_MOVE, Pieces::NO_PIECE);
		}

		// Promoting while capturing the checking piece
		while (promDefends) {
			defenderSq = popLSB(&promDefends);

			moveList->moves[moveList->cnt++] = serializeMove(defenderSq, attackerSq, PROM_MOVE, PROM_TO_QUEEN);
			moveList->moves[moveList->cnt++] = serializeMove(defenderSq, attackerSq, PROM_MOVE, PROM_TO_ROOK);
			moveList->moves[moveList->cnt++] = serializeMove(defenderSq, attackerSq, PROM_MOVE, PROM_TO_BISHOP);
			moveList->moves[moveList->cnt++] = serializeMove(defenderSq, attackerSq, PROM_MOVE, PROM_TO_KNIGHT);
		}

		// if pawn is threatening check but can be captured en passant
		if (b->enPas != DEFAULT_EP_SQ) {
			if (b->stm == WHITE) {
				if (b->enPas == attackerSq + 8
					&& pieceAt(b, attackerSq - 1) == Pieces::P
					&& (setMask[attackerSq - 1] & ~pinnedDefender)) {
					moveList->moves[moveList->cnt++] = serializeMove(attackerSq - 1, b->enPas, EP_MOVE, Pieces::NO_PIECE);
				}
				if (b->enPas == attackerSq + 8
					&& pieceAt(b, attackerSq + 1) == Pieces::P
					&& (setMask[attackerSq + 1] & ~pinnedDefender)) {
					moveList->moves[moveList->cnt++] = serializeMove(attackerSq + 1, b->enPas, EP_MOVE, Pieces::NO_PIECE);
				}
			} else {
				if (b->enPas == attackerSq - 8
					&& pieceAt(b, attackerSq - 1) == Pieces::p
					&& (setMask[attackerSq - 1] & ~pinnedDefender)) {
					moveList->moves[moveList->cnt++] = serializeMove(attackerSq - 1, b->enPas, EP_MOVE, Pieces::NO_PIECE);
				}
				if (b->enPas == attackerSq - 8
					&& pieceAt(b, attackerSq + 1) == Pieces::p
					&& (setMask[attackerSq + 1] & ~pinnedDefender)) {
					moveList->moves[moveList->cnt++] = serializeMove(attackerSq + 1, b->enPas, EP_MOVE, Pieces::NO_PIECE);
				}
			}
		}
	}

	// Step 2. Always add king evasions to non-attacked squares
	addKingCheckEvasions(b, moveList);
}

void addBlockersForSq(board_t* b, moveList_t* moveList, int blockingSq, bitboard_t* pinnedDefenders) {
	// Set of squares with pieces, that can block blockingSq
	// Pawns are only pushed, atkMask for pawns is not included in blockerSet
	bitboard_t blocker = blockerSet(b, b->stm, blockingSq) & ~(*pinnedDefenders);

	// Remove promBlockers from standard blocker set
	bitboard_t promRank = (b->stm == WHITE) ? RANK_7_HEX : RANK_2_HEX;
	bitboard_t promBlocker = getPieces(b, chai::PAWN, b->stm) & blocker & promRank;
	blocker &= ~promBlocker;

	int piece, sq;
	int flag;

	// Step 1:
	// Add non-capture promotion blocker
	while (promBlocker) {
		sq = popLSB(&promBlocker);
		moveList->moves[moveList->cnt++] = serializeMove(sq, blockingSq, PROM_MOVE, PROM_TO_QUEEN);
		moveList->moves[moveList->cnt++] = serializeMove(sq, blockingSq, PROM_MOVE, PROM_TO_ROOK);
		moveList->moves[moveList->cnt++] = serializeMove(sq, blockingSq, PROM_MOVE, PROM_TO_BISHOP);
		moveList->moves[moveList->cnt++] = serializeMove(sq, blockingSq, PROM_MOVE, PROM_TO_KNIGHT);
	}

	// Step 2:
	// All blocks, that do not promote
	while (blocker) {
		flag = 0;
		sq = popLSB(&blocker);
		piece = pieceAt(b, sq);
		Assert(piece != Pieces::k && piece != Pieces::K);
		Assert(pieceValid(pieceAt(b, sq)));

		if ((piece == Pieces::P || piece == Pieces::p)
			&& b->enPas != DEFAULT_EP_SQ
			&& sq == b->enPas) {
			flag = EP_MOVE;
		}

		moveList->moves[moveList->cnt++] = serializeMove(sq, blockingSq, flag, Pieces::NO_PIECE);
	}

}

void generateQuietCheckers(board_t* b, moveList_t* moveList) {
	Assert(!isCheck(b, b->stm));

	int sq, atk_sq;
	int oppkSq = getKingSquare(b, b->stm ^ 1);

	bitboard_t pieces;
	bitboard_t kingBoard = (1ULL << oppkSq);

	// Pawn pushes that check the king
	bitboard_t pawns;
	if (b->stm == WHITE) {
		pawns = (getPieces(b, PAWN, WHITE) << 8) & ~b->occupied;

		pieces = pawns;
		while (pieces) {
			sq = popLSB(&pieces);
			if (pawnAtkMask[WHITE][sq] & kingBoard) {
				moveList->moves[moveList->cnt++] = serializeMove(sq - 8, sq, NORMAL_MOVE, Pieces::NO_PIECE);
			}
		}

		pieces = (pawns << 8) & ~b->occupied & RANK_4_HEX;
		while (pieces) {
			sq = popLSB(&pieces);
			if (pawnAtkMask[WHITE][sq] & kingBoard) {
				moveList->moves[moveList->cnt++] = serializeMove(sq - 16, sq, NORMAL_MOVE, Pieces::NO_PIECE);
			}
		}
	} else {
		pawns = (getPieces(b, PAWN, BLACK) >> 8) & ~b->occupied;

		pieces = pawns;
		while (pieces) {
			sq = popLSB(&pieces);
			if (pawnAtkMask[BLACK][sq] & kingBoard) {
				moveList->moves[moveList->cnt++] = serializeMove(sq + 8, sq, NORMAL_MOVE, Pieces::NO_PIECE);
			}
		}

		pieces = (pawns << 8) & ~b->occupied & RANK_5_HEX;
		while (pieces) {
			sq = popLSB(&pieces);
			if (pawnAtkMask[BLACK][sq] & kingBoard) {
				moveList->moves[moveList->cnt++] = serializeMove(sq + 16, sq, NORMAL_MOVE, Pieces::NO_PIECE);
			}
		}
	}

	// Sliders that attack king: check if a piece can slide to kingAtkLines -> found quiet check
	bitboard_t kingAtkLines = lookUpBishopMoves(oppkSq, b->occupied);
	pieces = getPieces(b, BISHOP, b->stm) | getPieces(b, QUEEN, b->stm);

	bitboard_t diagMoves;
	while (pieces) {
		sq = popLSB(&pieces);
		diagMoves = lookUpBishopMoves(sq, b->occupied) & ~b->occupied;
		diagMoves &= kingAtkLines;

		while (diagMoves) {
			atk_sq = popLSB(&diagMoves);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, NORMAL_MOVE, Pieces::NO_PIECE);
		}
	}

	kingAtkLines = lookUpRookMoves(oppkSq, b->occupied);
	bitboard_t vertHoriMoves;
	pieces = getPieces(b, ROOK, b->stm) | getPieces(b, QUEEN, b->stm);
	while (pieces) {
		sq = popLSB(&pieces);
		vertHoriMoves = lookUpRookMoves(sq, b->occupied) & ~b->occupied;
		vertHoriMoves &= kingAtkLines;

		while (vertHoriMoves) {
			atk_sq = popLSB(&vertHoriMoves);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, NORMAL_MOVE, Pieces::NO_PIECE);
		}
	}

	// Knight moves that check king
	bitboard_t knightChecks;
	bitboard_t kingKnightPattern = knightAtkMask[oppkSq];
	pieces = getPieces(b, KNIGHT, b->stm);
	while (pieces) {
		sq = popLSB(&pieces);
		knightChecks = knightAtkMask[sq] & kingKnightPattern & ~b->occupied;

		while (knightChecks) {
			atk_sq = popLSB(&knightChecks);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, NORMAL_MOVE, Pieces::NO_PIECE);
		}
	}
}

bool isLegal(board_t* b, const move_t move) {
	bool inCheck = isCheck(b, b->stm);
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
	bitboard_t pawns = getPieces(b, PAWN, side);
	bitboard_t oppPawns = getPieces(b, PAWN, side ^ 1);

	int sq;
	bitboard_t defendedSquares = 0ULL;
	while (oppPawns) {
		sq = popLSB(&oppPawns);
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

bool hasEvadingMove(board_t* b) {
	Assert(isCheck(b, b->stm));

	moveList_t moveList;
	generateCheckEvasions(b, &moveList);
	return moveList.cnt > 0;
}

void whiteSinglePawnPush(board_t* board, moveList_t* moveList) {
	int sq;
	bitboard_t pushedPawns = (getPieces(board, PAWN, WHITE) << 8) & ~board->occupied;

	// divide proms and normal pushes
	// bitboard_t promPawns = pushedPawns & RANK_8_HEX;
	pushedPawns = pushedPawns & ~RANK_8_HEX;

	// normal pawn pushes
	while (pushedPawns) {
		sq = popLSB(&pushedPawns);
		moveList->moves[moveList->cnt++] = serializeMove(sq - 8, sq, NORMAL_MOVE, Pieces::NO_PIECE);
	}
}

void blackSinglePawnPush(board_t* board, moveList_t* moveList) {
	int sq;
	bitboard_t pushedPawns = (getPieces(board, PAWN, BLACK) >> 8) & ~board->occupied;

	// divide proms and normal pushes
	// bitboard_t promPawns = pushedPawns & RANK_1_HEX;
	pushedPawns = pushedPawns & ~RANK_1_HEX;

	// normal pawn pushes
	while (pushedPawns) {
		sq = popLSB(&pushedPawns);
		moveList->moves[moveList->cnt++] = serializeMove(sq + 8, sq, NORMAL_MOVE, Pieces::NO_PIECE);
	}
}

void whitePawnPushProm(board_t* b, moveList_t* moveList) {
	bitboard_t pawns = getPieces(b, PAWN, WHITE);
	pawns = ((pawns & RANK_7_HEX) << 8) & ~b->occupied;

	int sq;
	while (pawns) {
		sq = popLSB(&pawns);
		moveList->moves[moveList->cnt++] = serializeMove(sq - 8, sq, PROM_MOVE, PROM_TO_QUEEN);
		moveList->moves[moveList->cnt++] = serializeMove(sq - 8, sq, PROM_MOVE, PROM_TO_ROOK);
		moveList->moves[moveList->cnt++] = serializeMove(sq - 8, sq, PROM_MOVE, PROM_TO_BISHOP);
		moveList->moves[moveList->cnt++] = serializeMove(sq - 8, sq, PROM_MOVE, PROM_TO_KNIGHT);
	}
}

void whitePawnPushPromQ(board_t* b, moveList_t* moveList) {
	bitboard_t pawns = getPieces(b, PAWN, WHITE);
	pawns = ((pawns & RANK_7_HEX) << 8) & ~b->occupied;

	int sq;
	while (pawns) {
		sq = popLSB(&pawns);
		moveList->moves[moveList->cnt++] = serializeMove(sq - 8, sq, PROM_MOVE, PROM_TO_QUEEN);
	}
}

void blackPawnPushProm(board_t* board, moveList_t* moveList) {
	bitboard_t pawns = getPieces(board, PAWN, BLACK);
	pawns = ((pawns & RANK_2_HEX) >> 8) & ~board->occupied;

	int sq;
	while (pawns) {
		sq = popLSB(&pawns);
		moveList->moves[moveList->cnt++] = serializeMove(sq + 8, sq, PROM_MOVE, PROM_TO_QUEEN);
		moveList->moves[moveList->cnt++] = serializeMove(sq + 8, sq, PROM_MOVE, PROM_TO_ROOK);
		moveList->moves[moveList->cnt++] = serializeMove(sq + 8, sq, PROM_MOVE, PROM_TO_BISHOP);
		moveList->moves[moveList->cnt++] = serializeMove(sq + 8, sq, PROM_MOVE, PROM_TO_KNIGHT);
	}
}

void blackPawnPushPromQ(board_t* board, moveList_t* moveList) {
	bitboard_t pawns = getPieces(board, PAWN, BLACK);
	pawns = ((pawns & RANK_2_HEX) >> 8) & ~board->occupied;

	int sq;
	while (pawns) {
		sq = popLSB(&pawns);
		moveList->moves[moveList->cnt++] = serializeMove(sq + 8, sq, PROM_MOVE, PROM_TO_QUEEN);
	}
}

void whiteDoublePawnPush(board_t* board, moveList_t* moveList) {
	int sq;
	bitboard_t pushedPawns = (getPieces(board, PAWN, WHITE) << 8) & ~board->occupied;
	pushedPawns = (pushedPawns << 8) & ~board->occupied & RANK_4_HEX;

	while (pushedPawns) {
		sq = popLSB(&pushedPawns);
		moveList->moves[moveList->cnt++] = serializeMove(sq - 16, sq, NORMAL_MOVE, Pieces::NO_PIECE);
	}
}

void blackDoublePawnPush(board_t* board, moveList_t* moveList) {
	int sq;
	bitboard_t pushedPawns = (getPieces(board, PAWN, BLACK) >> 8) & ~board->occupied;
	pushedPawns = (pushedPawns >> 8) & ~board->occupied & RANK_5_HEX;

	while (pushedPawns) {
		sq = popLSB(&pushedPawns);
		moveList->moves[moveList->cnt++] = serializeMove(sq + 16, sq, NORMAL_MOVE, Pieces::NO_PIECE);
	}
}

void whitePawnCaptures(board_t* board, moveList_t* moveList) {
	int sq, atk_sq;
	bitboard_t atks;

	//divide in prom and non prom attacks
	bitboard_t whitePawns = getPieces(board, PAWN, WHITE);
	bitboard_t whitePawnProm = whitePawns & RANK_7_HEX;
	whitePawns &= ~RANK_7_HEX;

	// en passant square
	if (board->enPas != DEFAULT_EP_SQ) {
		if ((whitePawns << 7 & ~FILE_H_HEX) & (1ULL << board->enPas)) {
			moveList->moves[moveList->cnt++] = serializeMove(board->enPas - 7, board->enPas, EP_MOVE, Pieces::NO_PIECE);
		}
		if ((whitePawns << 9 & ~FILE_A_HEX) & (1ULL << board->enPas)) {
			moveList->moves[moveList->cnt++] = serializeMove(board->enPas - 9, board->enPas, EP_MOVE, Pieces::NO_PIECE);
		}
	}

	while (whitePawns) {
		sq = popLSB(&whitePawns);
		atks = pawnAtkMask[WHITE][sq] & board->color[BLACK];
		while (atks) {
			atk_sq = popLSB(&atks);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, NORMAL_MOVE, Pieces::NO_PIECE);
		}
	}

	// promoting attacks
	while (whitePawnProm) {
		sq = popLSB(&whitePawnProm);
		atks = pawnAtkMask[WHITE][sq] & board->color[BLACK];
		while (atks) {
			atk_sq = popLSB(&atks);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, PROM_MOVE, PROM_TO_QUEEN);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, PROM_MOVE, PROM_TO_ROOK);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, PROM_MOVE, PROM_TO_BISHOP);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, PROM_MOVE, PROM_TO_KNIGHT);
		}
	}
}

void blackPawnCaptures(board_t* board, moveList_t* moveList) {
	int sq, atk_sq;
	bitboard_t atks;

	//divide in prom and non prom attacks
	bitboard_t blackPawns = getPieces(board, PAWN, BLACK);
	bitboard_t blackPawnProm = blackPawns & RANK_2_HEX;
	blackPawns &= ~RANK_2_HEX;

	// en passant square
	if (board->enPas != DEFAULT_EP_SQ) {
		if ((blackPawns >> 7 & ~FILE_A_HEX) & (1ULL << board->enPas)) {
			moveList->moves[moveList->cnt++] = serializeMove(board->enPas + 7, board->enPas, EP_MOVE, Pieces::NO_PIECE);
		}
		if ((blackPawns >> 9 & ~FILE_H_HEX) & (1ULL << board->enPas)) {
			moveList->moves[moveList->cnt++] = serializeMove(board->enPas + 9, board->enPas, EP_MOVE, Pieces::NO_PIECE);
		}
	}

	while (blackPawns) {
		sq = popLSB(&blackPawns);
		atks = pawnAtkMask[BLACK][sq] & board->color[WHITE];
		while (atks) {
			atk_sq = popLSB(&atks);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, NORMAL_MOVE, Pieces::NO_PIECE);
		}
	}

	// promoting attacks
	while (blackPawnProm) {
		sq = popLSB(&blackPawnProm);
		atks = pawnAtkMask[BLACK][sq] & board->color[WHITE];
		while (atks) {
			atk_sq = popLSB(&atks);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, PROM_MOVE, PROM_TO_QUEEN);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, PROM_MOVE, PROM_TO_ROOK);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, PROM_MOVE, PROM_TO_BISHOP);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, PROM_MOVE, PROM_TO_KNIGHT);
		}
	}
}

void addKnightMoves(board_t* b, moveList_t* moveList) {
	int sq, atk_sq;
	bitboard_t knights = getPieces(b, KNIGHT, b->stm);
	bitboard_t atks;

	while (knights) {
		sq = popLSB(&knights);
		atks = knightAtkMask[sq] & ~b->occupied;
		while (atks) {
			atk_sq = popLSB(&atks);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, NORMAL_MOVE, Pieces::NO_PIECE);
		}
	}
}

void addKnightCaptures(board_t* b, moveList_t* moveList) {
	int sq, atk_sq;
	bitboard_t knights = getPieces(b, KNIGHT, b->stm);
	bitboard_t atks;

	while (knights) {
		sq = popLSB(&knights);
		atks = knightAtkMask[sq] & b->color[b->stm ^ 1];
		while (atks) {
			atk_sq = popLSB(&atks);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, NORMAL_MOVE, Pieces::NO_PIECE);
		}
	}
}

void addKingMoves(board_t* b, moveList_t* moveList) {
	int sq, kSq = getKingSquare(b, b->stm);
	bitboard_t kingMoves = kingAtkMask[kSq] & ~moveList->attackedSquares & ~b->occupied;

	while (kingMoves) {
		sq = popLSB(&kingMoves);
		moveList->moves[moveList->cnt++] = serializeMove(kSq, sq, NORMAL_MOVE, Pieces::NO_PIECE);
	}

	switch (b->stm) {
		case WHITE:
			if (castleValid(b, K_CASTLE, &moveList->attackedSquares)) {
				Assert(kSq == E1);
				moveList->moves[moveList->cnt++] = serializeMove(kSq, G1, CASTLE_MOVE, Pieces::NO_PIECE);
			}
			if (castleValid(b, Q_CASTLE, &moveList->attackedSquares)) {
				Assert(kSq == E1);
				moveList->moves[moveList->cnt++] = serializeMove(kSq, C1, CASTLE_MOVE, Pieces::NO_PIECE);
			}
			break;
		case BLACK:
			if (castleValid(b, k_CASTLE, &moveList->attackedSquares)) {
				Assert(kSq == E8);
				moveList->moves[moveList->cnt++] = serializeMove(kSq, G8, CASTLE_MOVE, Pieces::NO_PIECE);
			}

			if (castleValid(b, q_CASTLE, &moveList->attackedSquares)) {
				Assert(kSq == E8);
				moveList->moves[moveList->cnt++] = serializeMove(kSq, C8, CASTLE_MOVE, Pieces::NO_PIECE);
			}
			break;
	}
}

void addKingCaptures(board_t* b, moveList_t* moveList) {
	int atk_sq, kSq = getKingSquare(b, b->stm);
	bitboard_t whiteKingAttacks = kingAtkMask[kSq] & ~moveList->attackedSquares & b->color[b->stm ^ 1];

	while (whiteKingAttacks) {
		atk_sq = popLSB(&whiteKingAttacks);
		moveList->moves[moveList->cnt++] = serializeMove(kSq, atk_sq, NORMAL_MOVE, Pieces::NO_PIECE);
	}
}

void addRookMoves(board_t* b, moveList_t* moveList) {
	int sq, atk_sq;

	bitboard_t attackSet;
	bitboard_t rooks = getPieces(b, ROOK, b->stm);
	while (rooks) {
		sq = popLSB(&rooks);
		attackSet = lookUpRookMoves(sq, b->occupied);
		Assert(attackSet == calculateRookMoves(sq, b->occupied));
		attackSet &= ~b->occupied;

		while (attackSet) {
			atk_sq = popLSB(&attackSet);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, NORMAL_MOVE, Pieces::NO_PIECE);
		}
	}
}

void addBishopMoves(board_t* b, moveList_t* moveList) {
	int sq, atk_sq;

	bitboard_t bishops = getPieces(b, BISHOP, b->stm);
	bitboard_t attackSet;
	while (bishops) {
		sq = popLSB(&bishops);
		attackSet = lookUpBishopMoves(sq, b->occupied);
		attackSet &= ~b->occupied;

		while (attackSet) {
			atk_sq = popLSB(&attackSet);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, NORMAL_MOVE, Pieces::NO_PIECE);
		}
	}
}

void addRookCaptures(board_t* b, moveList_t* moveList) {
	int sq, atk_sq;
	bitboard_t captureSet;
	bitboard_t rooks = getPieces(b, ROOK, b->stm);
	while (rooks) {
		sq = popLSB(&rooks);
		captureSet = lookUpRookMoves(sq, b->occupied);
		captureSet = captureSet & b->color[b->stm ^ 1];

		while (captureSet) {
			atk_sq = popLSB(&captureSet);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, NORMAL_MOVE, Pieces::NO_PIECE);
		}
	}
}

void addBishopCaptures(board_t* board, moveList_t* moveList) {
	int sq, atk_sq;
	bitboard_t captureSet;
	bitboard_t bishops = getPieces(board, BISHOP, board->stm);
	while (bishops) {
		sq = popLSB(&bishops);
		captureSet = lookUpBishopMoves(sq, board->occupied);
		captureSet &= board->color[board->stm ^ 1];

		while (captureSet) {
			atk_sq = popLSB(&captureSet);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, NORMAL_MOVE, Pieces::NO_PIECE);
		}
	}

}

void addQueenMoves(board_t* b, moveList_t* moveList) {
	int sq, atk_sq;

	bitboard_t attackSet;
	bitboard_t queen = getPieces(b, QUEEN, b->stm);
	while (queen) {
		sq = popLSB(&queen);
		attackSet = lookUpRookMoves(sq, b->occupied) ^ lookUpBishopMoves(sq, b->occupied);
		attackSet &= ~b->occupied;

		while (attackSet) {
			atk_sq = popLSB(&attackSet);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, NORMAL_MOVE, Pieces::NO_PIECE);
		}
	}
}

void addQueenCaptures(board_t* b, moveList_t* moveList) {
	int sq, atk_sq;
	bitboard_t attackSet;
	bitboard_t queen = getPieces(b, QUEEN, b->stm);
	while (queen) {
		sq = popLSB(&queen);
		attackSet = lookUpRookMoves(sq, b->occupied) ^ lookUpBishopMoves(sq, b->occupied);
		attackSet &= b->color[b->stm ^ 1];

		while (attackSet) {
			atk_sq = popLSB(&attackSet);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, NORMAL_MOVE, Pieces::NO_PIECE);
		}
	}
}

void printGeneratedMoves(board_t* b, moveList_t* moveList) {

	cout << "\nGenerated " << moveList->cnt << " moves: " << endl;

	for (int i = 0; i < moveList->cnt; i++) {
		cout << i << ". " << getStringMove(b, moveList->moves[i]) << " score: " << moveList->scores[i] << endl;
	}
}

void addKingCheckEvasions(board_t* b, moveList_t* moveList) {
	//Assert(moveList->attackedSquares == attackerSet(b, b->stm ^ 1));
	int piece = (b->stm) == WHITE ? Pieces::K : Pieces::k;
	int kSq = getKingSquare(b, b->stm);
	int to;

	clearPiece(b, piece, kSq, b->stm);
	bitboard_t atkSquares = attackerSet(b, b->stm ^ 1);
	setPiece(b, piece, kSq, b->stm);

	bitboard_t quietSquares = kingAtkMask[kSq] & ~atkSquares & ~b->occupied;
	bitboard_t capSquares = kingAtkMask[kSq] & ~atkSquares & b->color[b->stm ^ 1];

	// generate quiet check evasions
	while (quietSquares) {
		to = popLSB(&quietSquares);
		moveList->moves[moveList->cnt++] = serializeMove(kSq, to, NORMAL_MOVE, Pieces::NO_PIECE);
	}

	// generate capturing check evasions
	while (capSquares) {
		to = popLSB(&capSquares);
		moveList->moves[moveList->cnt++] = serializeMove(kSq, to, NORMAL_MOVE, Pieces::NO_PIECE);
	}
}

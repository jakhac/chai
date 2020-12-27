#include "moveGenerator.h"

// cap piece ][ moving piece
int MVV_LVA[13][13];

/*
* Move Ordering:
* 1. Hash Move from PV / HashTable
*	 CAPTURES, PROMS
* 2. Good captures (caps with SEE > 0)
* 4. Promoting pawn with capture
* 5. Promoting pawn
* 6. Equal captures (caps with SEE = 0)
*	 QUIET
* 7. Both standard killer moves (quiet)
* 8. Rest of quiet moves ordered by history heuristic
*	 LOSING CAPS
* 9. Losing captures
*/

void generateMoves(Board* b, moveList_t* moveList) {
	moveList->cnt = 0;
	moveList->attackedSquares = b->attackerSet(b->side ^ 1);

	//pawns
	if (b->side == WHITE) {
		whiteSinglePawnPush(b, moveList);
		whiteDoublePawnPush(b, moveList);
		whitePawnCaptures(b, moveList);
	} else {
		blackSinglePawnPush(b, moveList);
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

bool isLegal(Board* b, move_t move, bitboard_t attackedSquares, bool inCheck) {
	ASSERT(attackedSquares == b->attackerSet(b->side ^ 1));

	// if inCheck, push move and test for legality
	if (inCheck) {
		if (!b->push(move)) {
			return false;
		}
		b->pop();
		return true;
	}

	int kSq = b->getKingSquare(b->side);
	int from = fromSq(move);
	int to = toSq(move);

	// if king is moving, check if toSq is attacked by opp
	if (from == kSq) {
		return !(attackedSquares & setMask[toSq(move)]);
	}

	// if moving piece is pinned to king, check if toSq is within pinning line + pinner
	bitboard_t pinned = b->pinned(kSq, b->side);
	if (pinned & setMask[fromSq(move)]) {

		bitboard_t pinner = b->pinner(kSq, b->side);
		bitboard_t pinningLine;
		int pinnerSq;

		// find pinner for moving piece
		while (&pinner) {
			pinnerSq = popBit(&pinner);
			pinningLine = line_bb(pinnerSq, kSq);

			if (pinningLine & setMask[from]) {
				break;
			}
		}
		// check if pinned piece is moving along pinning line
		return pinningLine & setMask[to];
	}

	return true;
}

void generateCaptures(Board* b, moveList_t* moveList) {
	moveList->attackedSquares = b->attackerSet(b->side ^ 1);

	if (b->side == WHITE) {
		whitePawnCaptures(b, moveList);
	} else {
		blackPawnCaptures(b, moveList);
	}

	addKnightCaptures(b, moveList);
	addBishopCaptures(b, moveList);
	addRookCaptures(b, moveList);
	addQueenCaptures(b, moveList);
	addKingCaptures(b, moveList);
}

bool moveLegal(Board* b, const move_t move) {

	moveList_t moveList[1];
	generateMoves(b, moveList);

	for (int i = 0; i < moveList->cnt; i++) {
		if (move == moveList->moves[i]) {
			if (b->push(move)) {
				// if move is pseudo legal and legal
				b->pop();
				return true;
			} else {
				// if move is pseudo legal but not legal
				return false;
			}
		}
	}
	return false;
}

//// TODO rewrite with switch case?
//void addQuietMove(Board* b, moveList_t* moveList, move_t move, int piece) {
//	ASSERT(pieceValid(piece));
//
//	if (b->killer[0][b->ply] == move) {
//		moveList->scores[moveList->cnt] = KILLER_SCORE_1;
//	} else if (b->killer[1][b->ply] == move) {
//		moveList->scores[moveList->cnt] = KILLER_SCORE_2;
//	} else if (b->mateKiller[b->ply] == move) {
//		moveList->scores[moveList->cnt] = MATE_KILLER;
//	} else if (b->ply > 0 && b->undoHistory[b->ply - 1].move != NULL_MOVE) {
//
//		// TODO use prevMove in struct for move gen in , e.g. moveList similar to attackedSquares
//		move_t prevMove = b->undoHistory[b->ply - 1].move;
//		move_t counterMove = b->counterHeuristic[fromSq(prevMove)][toSq(prevMove)][b->side];
//		if (counterMove == move) {
//			moveList->scores[moveList->cnt] = COUNTER_SCORE;
//		}
//
//	} else {
//		int histScore = b->histHeuristic[piece][toSq(move)];
//		moveList->scores[moveList->cnt] = QUIET_SCORE + (histScore / 250);
//		ASSERT(QUIET_SCORE + (histScore / 250) < KILLER_SCORE_2);
//	}
//
//	moveList->moves[moveList->cnt] = move;
//	moveList->cnt++;
//}
//
//void addCaptureMove(Board* b, moveList_t* moveList, int move, int movingPiece) {
//	ASSERT(pieceValid(capPiece(move)));
//
//	int moveScore = 0;
//	int seeScore = see(b, move);
//	if (seeScore > 0) {
//		moveScore = GOOD_CAPTURE + MVV_LVA[capPiece(move)][movingPiece];
//	} else if (seeScore == 0) {
//		moveScore = EQUAL_CAPTURE + MVV_LVA[capPiece(move)][movingPiece];
//	} else {
//		moveScore = MVV_LVA[capPiece(move)][movingPiece];
//	}
//
//	moveList->scores[moveList->cnt] = moveScore;
//	moveList->moves[moveList->cnt] = move;
//	moveList->cnt++;
//}
//
//void addEnPassantMove(Board* b, moveList_t* moveList, int move) {
//	moveList->scores[moveList->cnt] = GOOD_CAPTURE + 105;
//	moveList->moves[moveList->cnt] = move;
//	moveList->cnt++;
//}
//
//void addPromotion(Board* b, moveList_t* moveList, move_t move) {
//	ASSERT(MCHECK_PROM & move);
//	moveList->scores[moveList->cnt] = PROMOTION;
//	moveList->moves[moveList->cnt] = move;
//	moveList->cnt++;
//}
//
//void addPromotingCapture(Board* b, moveList_t* moveList, move_t move, int movingPiece) {
//	ASSERT(MCHECK_PROM & move);
//	ASSERT(MCHECK_CAP & move);
//
//	moveList->scores[moveList->cnt] = PROMOTING_CAPTURE + movingPiece;
//	moveList->moves[moveList->cnt] = move;
//	moveList->cnt++;
//}
//
//void addCastleMove(Board* b, moveList_t* moveList, move_t move) {
//	moveList->scores[moveList->cnt] = QUIET_SCORE + CASTLE_SCORE;
//	moveList->moves[moveList->cnt] = move;
//	moveList->cnt++;
//}

void initMVV_LVA() {
	int attacker;
	int victim;
	for (attacker = P; attacker <= k; attacker++) {
		for (victim = P; victim <= k; victim++) {
			MVV_LVA[victim][attacker] = victimScore[victim] + 6 - (victimScore[attacker] / 100);
		}
	}
}

// 20percent see function
bitboard_t getLeastValuablePiece(Board* b, bitboard_t attadef, int side, int attackerPiece) {
	attadef &= b->color[side];
	if (!attadef) {
		return 0ULL;
	}

	bitboard_t lva;
	for (int i = PAWN; i <= KING; i++) {
		lva = attadef & b->pieces[i];
		if (lva) {
			return setMask[bitscanForward(lva)];
		}
	}

	ASSERT(false);
}

int see(Board* b, const int move) {
	int to = toSq(move);
	int attackerPiece = b->pieceAt(fromSq(move));
	int gain[32], d = 0, side = b->side;

	bitboard_t occ = b->occupied;
	bitboard_t mayXray = b->pieces[PAWN] | b->pieces[BISHOP] | b->pieces[ROOK] | b->pieces[QUEEN];
	bitboard_t attadef = b->squareAttacked(to);
	bitboard_t from = setMask[fromSq(move)];
	bitboard_t used = 0ULL, discovered = 0ULL;

	gain[d] = pieceScores[capPiece(move)];
	do {
		ASSERT(pieceValid(attackerPiece));
		d++; // next depth and side
		gain[d] = -gain[d - 1] + pieceScores[attackerPiece]; // speculative store, if defended

		if (max(-gain[d - 1], gain[d]) < 0) break; // pruning does not influence the result

		attadef ^= from; // reset bit in set to traverse
		occ ^= from; // reset bit in temporary occupancy (for magic move generation)
		used |= from;

		if (from & mayXray) {
			discovered = 0ULL;
			discovered |= lookUpBishopMoves(to, occ) & (b->getPieces(QUEEN, side) | b->getPieces(BISHOP, side));
			discovered |= lookUpRookMoves(to, occ) & (b->getPieces(QUEEN, side) | b->getPieces(ROOK, side));
			attadef |= discovered & ~used;
		}

		side ^= 1;
		from = getLeastValuablePiece(b, attadef, side, attackerPiece);
		attackerPiece = b->pieceAt(bitscanForward(from));

	} while (from && attackerPiece);
	while (--d) {
		gain[d - 1] = -max(-gain[d - 1], gain[d]);
	}

	if (MFLAG_EP & move) gain[0] += 100;

	return gain[0];
}

/// <summary>
/// Calculate possible single push pawn moves for white.
/// </summary>
/// <param name="board">Board</param>
void whiteSinglePawnPush(Board* b, moveList_t* moveList) {
	int sq;
	bitboard_t pushedPawns = (b->getPieces(PAWN, WHITE) << 8) & ~b->occupied;

	// divide proms and normal pushes
	bitboard_t promPawns = pushedPawns & RANK_8_HEX;
	pushedPawns = pushedPawns & ~RANK_8_HEX;

	// normal pawn pushes
	while (pushedPawns) {
		sq = popBit(&pushedPawns);
		moveList->moves[moveList->cnt++] = serializeMove(sq - 8, sq, EMPTY, EMPTY, EMPTY);

		//addQuietMove(b, moveList, serializeMove(sq - 8, sq, EMPTY, EMPTY, EMPTY), P);
	}

	// prom pawn pushes
	while (promPawns) {
		sq = popBit(&promPawns);
		moveList->moves[moveList->cnt++] = serializeMove(sq - 8, sq, EMPTY, Q, EMPTY);
		moveList->moves[moveList->cnt++] = serializeMove(sq - 8, sq, EMPTY, R, EMPTY);
		moveList->moves[moveList->cnt++] = serializeMove(sq - 8, sq, EMPTY, B, EMPTY);
		moveList->moves[moveList->cnt++] = serializeMove(sq - 8, sq, EMPTY, N, EMPTY);

		//addPromotion(b, moveList, serializeMove(sq - 8, sq, EMPTY, Q, EMPTY));
		//addPromotion(b, moveList, serializeMove(sq - 8, sq, EMPTY, R, EMPTY));
		//addPromotion(b, moveList, serializeMove(sq - 8, sq, EMPTY, B, EMPTY));
		//addPromotion(b, moveList, serializeMove(sq - 8, sq, EMPTY, N, EMPTY));
	}
}

/// <summary>
/// Calculate possible single push pawn moves for black.
/// </summary>
/// <param name="board">Board</param> 
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

		//addQuietMove(board, moveList, serializeMove(sq + 8, sq, EMPTY, EMPTY, EMPTY), p);
	}

	// prom pawn pushes
	while (promPawns) {
		sq = popBit(&promPawns);
		moveList->moves[moveList->cnt++] = serializeMove(sq + 8, sq, EMPTY, q, EMPTY);
		moveList->moves[moveList->cnt++] = serializeMove(sq + 8, sq, EMPTY, r, EMPTY);
		moveList->moves[moveList->cnt++] = serializeMove(sq + 8, sq, EMPTY, b, EMPTY);
		moveList->moves[moveList->cnt++] = serializeMove(sq + 8, sq, EMPTY, n, EMPTY);

		//addPromotion(board, moveList, serializeMove(sq + 8, sq, EMPTY, q, EMPTY));
		//addPromotion(board, moveList, serializeMove(sq + 8, sq, EMPTY, r, EMPTY));
		//addPromotion(board, moveList, serializeMove(sq + 8, sq, EMPTY, b, EMPTY));
		//addPromotion(board, moveList, serializeMove(sq + 8, sq, EMPTY, n, EMPTY));
	}
}

/// <summary>
/// Calculate all possible moves with pawnstart for white.
/// </summary>
/// <param name="board">Board</param>
void whiteDoublePawnPush(Board* board, moveList_t* moveList) {
	int sq;
	bitboard_t pushedPawns = (board->getPieces(PAWN, WHITE) << 8) & ~board->occupied;
	pushedPawns = (pushedPawns << 8) & ~board->occupied & RANK_4_HEX;

	while (pushedPawns) {
		sq = popBit(&pushedPawns);
		moveList->moves[moveList->cnt++] = serializeMove(sq - 16, sq, EMPTY, EMPTY, MFLAG_PS);

		//addQuietMove(board, moveList, serializeMove(sq - 16, sq, EMPTY, EMPTY, MFLAG_PS), P);
	}
}

/// <summary>
/// Calculate all possible moves with pawnstart for black.
/// </summary>
/// <param name="board">Board</param>
void blackDoublePawnPush(Board* board, moveList_t* moveList) {
	int sq;
	bitboard_t pushedPawns = (board->getPieces(PAWN, BLACK) >> 8) & ~board->occupied;
	pushedPawns = (pushedPawns >> 8) & ~board->occupied & RANK_5_HEX;

	while (pushedPawns) {
		sq = popBit(&pushedPawns);
		moveList->moves[moveList->cnt++] = serializeMove(sq + 16, sq, EMPTY, EMPTY, MFLAG_PS);

		//addQuietMove(board, moveList, serializeMove(sq + 16, sq, EMPTY, EMPTY, MFLAG_PS), p);
	}
}

/// <summary>
/// Generate all possible east attacks for white pawns
/// </summary>
/// <param name="board">Board</param>
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

		//addEnPassantMove(board, moveList, serializeMove(board->enPas - 7, board->enPas, EMPTY, EMPTY, MFLAG_EP));
	}
	if ((whitePawns << 9 & ~FILE_A_HEX) & setMask[board->enPas]) {
		moveList->moves[moveList->cnt++] = serializeMove(board->enPas - 9, board->enPas, EMPTY, EMPTY, MFLAG_EP);

		//addEnPassantMove(board, moveList, serializeMove(board->enPas - 9, board->enPas, EMPTY, EMPTY, MFLAG_EP));
	}

	while (whitePawns) {
		sq = popBit(&whitePawns);
		atks = pawnAtkMask[WHITE][sq] & board->color[BLACK];
		while (atks) {
			atk_sq = popBit(&atks);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, board->pieceAt(atk_sq), EMPTY, EMPTY);

			//addCaptureMove(board, moveList, serializeMove(sq, atk_sq, board->pieceAt(atk_sq), EMPTY, EMPTY), P);
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

			//addPromotingCapture(board, moveList, serializeMove(sq, atk_sq, board->pieceAt(atk_sq), Q, EMPTY), P);
			//addPromotingCapture(board, moveList, serializeMove(sq, atk_sq, board->pieceAt(atk_sq), R, EMPTY), P);
			//addPromotingCapture(board, moveList, serializeMove(sq, atk_sq, board->pieceAt(atk_sq), B, EMPTY), P);
			//addPromotingCapture(board, moveList, serializeMove(sq, atk_sq, board->pieceAt(atk_sq), N, EMPTY), P);
		}
	}
}

/// <summary>
/// Generate all possible east attacks for white pawns.
/// </summary>
/// <param name="board">Board</param>
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

		//addEnPassantMove(board, moveList, serializeMove(board->enPas + 7, board->enPas, EMPTY, EMPTY, MFLAG_EP));
	}
	if ((blackPawns >> 9 & ~FILE_H_HEX) & setMask[board->enPas]) {
		moveList->moves[moveList->cnt++] = serializeMove(board->enPas + 9, board->enPas, EMPTY, EMPTY, MFLAG_EP);

		//addEnPassantMove(board, moveList, serializeMove(board->enPas + 9, board->enPas, EMPTY, EMPTY, MFLAG_EP));
	}

	while (blackPawns) {
		sq = popBit(&blackPawns);
		atks = pawnAtkMask[BLACK][sq] & board->color[WHITE];
		while (atks) {
			atk_sq = popBit(&atks);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, board->pieceAt(atk_sq), EMPTY, EMPTY);

			//addCaptureMove(board, moveList, serializeMove(sq, atk_sq, board->pieceAt(atk_sq), EMPTY, EMPTY), p);
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

			//addPromotingCapture(board, moveList, serializeMove(sq, atk_sq, board->pieceAt(atk_sq), q, EMPTY), p);
			//addPromotingCapture(board, moveList, serializeMove(sq, atk_sq, board->pieceAt(atk_sq), r, EMPTY), p);
			//addPromotingCapture(board, moveList, serializeMove(sq, atk_sq, board->pieceAt(atk_sq), b, EMPTY), p);
			//addPromotingCapture(board, moveList, serializeMove(sq, atk_sq, board->pieceAt(atk_sq), n, EMPTY), p);
		}
	}
}

void addKnightMoves(Board* b, moveList_t* moveList) {
	int sq, atk_sq;
	int piece = (b->side == WHITE) ? N : n;
	bitboard_t knights = b->getPieces(KNIGHT, b->side);
	bitboard_t atks;

	while (knights) {
		sq = popBit(&knights);
		atks = knightAtkMask[sq] & ~b->occupied;
		while (atks) {
			atk_sq = popBit(&atks);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, EMPTY, EMPTY, EMPTY);

			//addQuietMove(b, moveList, serializeMove(sq, atk_sq, EMPTY, EMPTY, EMPTY), piece);
		}
	}
}

void addKnightCaptures(Board* b, moveList_t* moveList) {
	int sq, atk_sq;
	int piece = (b->side == WHITE) ? N : n;
	bitboard_t knights = b->getPieces(KNIGHT, b->side);
	bitboard_t atks;

	while (knights) {
		sq = popBit(&knights);
		atks = knightAtkMask[sq] & b->color[b->side ^ 1];
		while (atks) {
			atk_sq = popBit(&atks);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, b->pieceAt(atk_sq), EMPTY, EMPTY);

			//addCaptureMove(b, moveList, serializeMove(sq, atk_sq, b->pieceAt(atk_sq), EMPTY, EMPTY), piece);
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

		//addQuietMove(b, moveList, serializeMove(kSq, sq, EMPTY, EMPTY, EMPTY), piece);
	}

	switch (b->side) {
		case WHITE:
			if (b->castleValid(K_CASTLE, &moveList->attackedSquares)) {
				ASSERT(kSq == E1);
				moveList->moves[moveList->cnt++] = serializeMove(kSq, G1, EMPTY, EMPTY, MFLAG_CAS);

				//addCastleMove(b, moveList, serializeMove(kSq, G1, EMPTY, EMPTY, MFLAG_CAS));
			}
			if (b->castleValid(Q_CASTLE, &moveList->attackedSquares)) {
				ASSERT(kSq == E1);
				moveList->moves[moveList->cnt++] = serializeMove(kSq, C1, EMPTY, EMPTY, MFLAG_CAS);

				//addCastleMove(b, moveList, serializeMove(kSq, C1, EMPTY, EMPTY, MFLAG_CAS));
			}
			break;
		case BLACK:
			if (b->castleValid(k_CASTLE, &moveList->attackedSquares)) {
				ASSERT(kSq == E8);
				moveList->moves[moveList->cnt++] = serializeMove(kSq, G8, EMPTY, EMPTY, MFLAG_CAS);

				//addCastleMove(b, moveList, serializeMove(kSq, G8, EMPTY, EMPTY, MFLAG_CAS));
			}

			if (b->castleValid(q_CASTLE, &moveList->attackedSquares)) {
				ASSERT(kSq == E8);
				moveList->moves[moveList->cnt++] = serializeMove(kSq, C8, EMPTY, EMPTY, MFLAG_CAS);

				//addCastleMove(b, moveList, serializeMove(kSq, C8, EMPTY, EMPTY, MFLAG_CAS));
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

		//addCaptureMove(b, moveList, serializeMove(kSq, atk_sq, b->pieceAt(atk_sq), EMPTY, EMPTY), piece);
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
		ASSERT(attackSet == calculateRookMoves(sq, b->occupied));
		attackSet &= ~b->occupied;
		while (attackSet) {
			atk_sq = popBit(&attackSet);
			moveList->moves[moveList->cnt++] = serializeMove(sq, atk_sq, EMPTY, EMPTY, EMPTY);

			//addQuietMove(b, moveList, serializeMove(sq, atk_sq, EMPTY, EMPTY, EMPTY), piece);
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

			//addQuietMove(b, moveList, serializeMove(sq, atk_sq, EMPTY, EMPTY, EMPTY), piece);
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

			//addCaptureMove(b, moveList, serializeMove(sq, atk_sq, b->pieceAt(atk_sq), EMPTY, EMPTY), piece);
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

			//addCaptureMove(board, moveList, serializeMove(sq, atk_sq, board->pieceAt(atk_sq), EMPTY, EMPTY), piece);
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

			//addQuietMove(b, moveList, serializeMove(sq, atk_sq, EMPTY, EMPTY, EMPTY), piece);
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

			//addCaptureMove(b, moveList, serializeMove(sq, atk_sq, b->pieceAt(atk_sq), EMPTY, EMPTY), piece);
		}
	}
}

void printGeneratedMoves(moveList_t* moveList) {

	cout << "\nGenerated " << moveList->cnt << " moves: " << endl;

	for (int i = 0; i < moveList->cnt; i++) {
		cout << i << ". " << getStringMove(moveList->moves[i]) << endl;;
	}
}
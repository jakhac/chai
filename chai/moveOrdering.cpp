#include "moveOrdering.h"

void initMVV_LVA() {
	int attacker;
	int victim;
	for (attacker = P; attacker <= k; attacker++) {
		for (victim = P; victim <= k; victim++) {
			MVV_LVA[victim][attacker] = victimScore[victim] + 6 - (victimScore[attacker] / 100);
		}
	}
}

bitboard_t getLeastValuablePiece(Board* b, bitboard_t atkDef, int side, int attackerPiece) {
	atkDef &= b->color[side];
	if (!atkDef) {
		return 0ULL;
	}

	bitboard_t lva;
	for (int i = PAWN; i <= KING; i++) {
		lva = atkDef & b->pieces[i];
		if (lva) {
			return setMask[bitscanForward(lva)];
		}
	}

	Assert(false);
	return 0ULL;
}

int see(Board* b, const int move) {
	Assert(pieceValid(capPiece(move)));

	int to = toSq(move);
	int attackerPiece = b->pieceAt(fromSq(move));
	int gain[32]{}, d = 0, side = b->side;

	bitboard_t occ = b->occupied;
	bitboard_t mayXray = b->pieces[PAWN] | b->pieces[BISHOP] | b->pieces[ROOK] | b->pieces[QUEEN];
	bitboard_t attadef = b->squareAtkDef(to);
	bitboard_t from = setMask[fromSq(move)];
	bitboard_t used = 0ULL, discovered = 0ULL;

	gain[d] = pieceScores[capPiece(move)];
	do {
		Assert(pieceValid(attackerPiece));
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

int lazySee(Board* b, const int move) {
	Assert(pieceValid(capPiece(move)));

	int to = toSq(move);
	int attackerPiece = b->pieceAt(fromSq(move));
	int gain[32]{}, d = 0, side = b->side;

	bitboard_t occ = b->occupied;
	bitboard_t mayXray = b->pieces[PAWN] | b->pieces[BISHOP] | b->pieces[ROOK] | b->pieces[QUEEN];
	bitboard_t attadef = b->squareAtkDef(to);
	bitboard_t from = setMask[fromSq(move)];
	bitboard_t used = 0ULL, discovered = 0ULL;

	gain[d] = pieceScores[capPiece(move)];

	// If the captured piece is worth more than the attacker, it is always
	// winning. An estimated SEE score is then returned.
	if (gain[d] > pieceScores[attackerPiece]) {
		return gain[d] - (pieceScores[attackerPiece] / 2);
	}

	do {
		Assert(pieceValid(attackerPiece));
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

static void updateBestMove(int* scores, int* bestIdx, int curIdx) {
	if (scores[curIdx] > scores[*bestIdx]) {
		*bestIdx = curIdx;
	}
}

void scoreMovesAlphaBeta(Board* b, moveList_t* moveList, move_t hashMove) {
	move_t currentMove;
	int seeScore = 0;
	int mvvLvaScore = 0;
	int bestIdx = 0;

	// set all scores to 0
	for (int i = 0; i < moveList->cnt; i++) moveList->scores[i] = 0;

	for (int i = 0; i < moveList->cnt; i++) {
		currentMove = moveList->moves[i];
		Assert(currentMove != NO_MOVE);

		// hash move
		if (currentMove == hashMove) {
			moveList->scores[i] = HASH_MOVE;

			updateBestMove(moveList->scores, &bestIdx, i);
			continue;
		}

		// enpas move
		if (currentMove & MCHECK_EP) {
			moveList->scores[i] = GOOD_CAPTURE + 105;

			updateBestMove(moveList->scores, &bestIdx, i);
			continue;
		}

		// capture moves
		if (currentMove & MCHECK_CAP) {

			if (currentMove & MCHECK_PROM) {
				// Promoting captures
				moveList->scores[i] = PROMOTING_CAPTURE + b->pieceAt(fromSq(currentMove));
			} else {
				// Plain Captures
				seeScore = see(b, currentMove);
				mvvLvaScore = MVV_LVA[capPiece(currentMove)][b->pieceAt(fromSq(currentMove))];
				Assert(mvvLvaScore > 0);
				Assert(mvvLvaScore < MVV_LVA_UBOUND);

				if (seeScore > 0) {
					moveList->scores[i] = GOOD_CAPTURE + mvvLvaScore;
				} else if (seeScore == 0) {
					moveList->scores[i] = EQUAL_CAPTURE + mvvLvaScore;
				} else {
					moveList->scores[i] = BAD_CAPTURE + mvvLvaScore;
				}
			}

			updateBestMove(moveList->scores, &bestIdx, i);
			continue;
		}

		// Only quiet moves left:

		// first killer
		if (currentMove == b->killer[0][b->ply]) {
			Assert(!(currentMove & MCHECK_CAP));
			moveList->scores[i] = KILLER_SCORE_1;

			updateBestMove(moveList->scores, &bestIdx, i);
			continue;
		}

		// second killer
		if (currentMove == b->killer[1][b->ply]) {
			Assert(!(currentMove & MCHECK_CAP));
			moveList->scores[i] = KILLER_SCORE_2;

			updateBestMove(moveList->scores, &bestIdx, i);
			continue;
		}

		// mate killer
		if (currentMove == b->mateKiller[b->ply]) {
			moveList->scores[i] = MATE_KILLER;

			updateBestMove(moveList->scores, &bestIdx, i);
			continue;
		}

		// counter move
		if (b->ply > 0) {
			move_t prevMove = b->undoHistory[b->ply - 1].move;
			move_t counterMove = b->counterHeuristic[fromSq(prevMove)][toSq(prevMove)][b->side];

			if (currentMove == counterMove) {
				moveList->scores[i] = COUNTER_SCORE;
			}

			updateBestMove(moveList->scores, &bestIdx, i);
			continue;
		}

		// castle move
		if (currentMove & MCHECK_CAS) {
			moveList->scores[i] = QUIET_SCORE + CASTLE_SCORE;

			updateBestMove(moveList->scores, &bestIdx, i);
			continue;
		}

		// promotion
		if (currentMove & MCHECK_PROM) {
			moveList->scores[i] = PROMOTION + b->pieceAt(fromSq(currentMove));

			updateBestMove(moveList->scores, &bestIdx, i);
			continue;
		}

		// last resort: history heuristic
		int histScore = b->histHeuristic[b->pieceAt(fromSq(currentMove))][toSq(currentMove)];
		moveList->scores[i] = QUIET_SCORE + (histScore / 250);
		updateBestMove(moveList->scores, &bestIdx, i);
	}

	// swap best move to first position !!!!! score has to be moved too
	//if (moveList->cnt > 1) {
	//	move_t temp = moveList->moves[0];
	//	moveList->moves[0] = moveList->moves[bestIdx];
	//	moveList->moves[0] = temp;
	//}

}

void scoreMovesQuiesence(Board* b, moveList_t* moveList) {
	int mvvLvaScore = 0;
	int seeScore = 0;
	move_t currentMove;

	for (int i = 0; i < moveList->cnt; i++) {
		currentMove = moveList->moves[i];

		// All moves in quiesence are either captures or promotions, check evasions
		// are scored like alphaBeta movegen.
		Assert(currentMove & MCHECK_PROM_OR_CAP || currentMove & MCHECK_EP);
		Assert(pieceValid(b->pieceAt(fromSq(currentMove))));

		mvvLvaScore = MVV_LVA[capPiece(currentMove)][b->pieceAt(fromSq(currentMove))];

		// EnPas captures
		if (currentMove & MCHECK_EP) {
			moveList->scores[i] = GOOD_CAPTURE + 105;
			continue;
		}

		// Promoting captures
		if (currentMove & MCHECK_PROM && currentMove & MCHECK_CAP) {
			Assert(pieceValid(promPiece(currentMove)));
			moveList->scores[i] = PROMOTING_CAPTURE + b->pieceAt(fromSq(currentMove));
			continue;
		}

		// Promotions
		if (currentMove & MCHECK_PROM) {
			Assert(pieceValid(promPiece(currentMove)));
			moveList->scores[i] = PROMOTION + b->pieceAt(fromSq(currentMove));
			continue;
		}

		// Only plain captures left
		seeScore = lazySee(b, currentMove);
		if (seeScore > 0) {
			moveList->scores[i] = GOOD_CAPTURE + mvvLvaScore;
		} else if (seeScore == 0) {
			moveList->scores[i] = EQUAL_CAPTURE + mvvLvaScore;
		} else {
			moveList->scores[i] = BAD_CAPTURE + mvvLvaScore;
		}

	}
}

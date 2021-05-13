#include "moveOrdering.h"

void initMVV_LVA() {
	int attacker;
	int victim;
	for (attacker = Piece::P; attacker <= Piece::k; attacker++) {
		for (victim = Piece::P; victim <= Piece::k; victim++) {
			MVV_LVA[victim][attacker] = victimScore[victim] + 6 - (victimScore[attacker] / 100);
		}
	}
}

bitboard_t getLeastValuablePiece(board_t* b, bitboard_t atkDef, int side, int attackerPiece) {
	atkDef &= b->color[side];
	if (!atkDef) {
		return 0ULL;
	}

	bitboard_t lva;
	for (int i = Piece::PAWN; i <= Piece::KING; i++) {
		lva = atkDef & b->pieces[i];
		if (lva) {
			return setMask[bitscanForward(lva)];
		}
	}

	Assert(false);
	return 0ULL;
}

int see(board_t* b, const int move) {
	int to = toSq(move);
	int attackerPiece = pieceAt(b, fromSq(move));
	int movingPiece = attackerPiece;
	int gain[32]{}, d = 0, side = b->side;

	bitboard_t occ = b->occupied;
	bitboard_t mayXray = b->pieces[Piece::PAWN] | b->pieces[Piece::BISHOP] | b->pieces[Piece::ROOK] | b->pieces[Piece::QUEEN];
	bitboard_t attadef = squareAtkDef(b, to);
	bitboard_t from = setMask[fromSq(move)];
	bitboard_t used = 0ULL, discovered = 0ULL;

	gain[d] = pieceScores[capPiece(b, move)];
	do {
		Assert(pieceValid(attackerPiece));
		d++; // next depth and side
		gain[d] = -gain[d - 1] + pieceScores[attackerPiece]; // speculative store, if defended

		if (max(-gain[d - 1], gain[d]) < 0)
			break; // pruning does not influence the result

		attadef ^= from; // reset bit in set to traverse
		occ ^= from; // reset bit in temporary occupancy (for magic move generation)
		used |= from;

		if (from & mayXray) {
			discovered = 0ULL;
			discovered |= lookUpBishopMoves(to, occ) & (getPieces(b, Piece::QUEEN, side) | getPieces(b, Piece::BISHOP, side));
			discovered |= lookUpRookMoves(to, occ) & (getPieces(b, Piece::QUEEN, side) | getPieces(b, Piece::ROOK, side));
			attadef |= discovered & ~used;
		}

		side ^= 1;
		from = getLeastValuablePiece(b, attadef, side, attackerPiece);
		attackerPiece = pieceAt(b, bitscanForward(from));

	} while (from && attackerPiece);
	while (--d) {
		gain[d - 1] = -max(-gain[d - 1], gain[d]);
	}

	if (isEnPassant(move))
		gain[0] += 100;

	return gain[0];
}

int lazySee(board_t* b, const int move) {
	Assert(false); // deprecated
	Assert(pieceValid(capPiece(b, move)));

	int to = toSq(move);
	int attackerPiece = pieceAt(b, fromSq(move));
	int movingPiece = attackerPiece;
	int gain[32]{}, d = 0, side = b->side;

	bitboard_t occ = b->occupied;
	bitboard_t mayXray = b->pieces[Piece::PAWN] | b->pieces[Piece::BISHOP] | b->pieces[Piece::ROOK] | b->pieces[Piece::QUEEN];
	bitboard_t attadef = squareAtkDef(b, to);
	bitboard_t from = setMask[fromSq(move)];
	bitboard_t used = 0ULL, discovered = 0ULL;

	gain[d] = pieceScores[capPiece(b, move)];

	// If the captured piece is worth more than the attacker, it is always
	// winning. An estimated SEE score is then returned.
	if (gain[d] > pieceScores[attackerPiece]) {
		return gain[d] - (pieceScores[attackerPiece] / 2);
	}

	do {
		Assert(pieceValid(attackerPiece));
		d++; // next depth and side
		gain[d] = -gain[d - 1] + pieceScores[attackerPiece]; // speculative store, if defended

		if (max(-gain[d - 1], gain[d]) < 0)
			break; // pruning does not influence the result

		attadef ^= from; // reset bit in set to traverse
		occ ^= from; // reset bit in temporary occupancy (for magic move generation)
		used |= from;

		if (from & mayXray) {
			discovered = 0ULL;
			discovered |= lookUpBishopMoves(to, occ) & (getPieces(b, Piece::QUEEN, side) | getPieces(b, Piece::BISHOP, side));
			discovered |= lookUpRookMoves(to, occ) & (getPieces(b, Piece::QUEEN, side) | getPieces(b, Piece::ROOK, side));
			attadef |= discovered & ~used;
		}

		side ^= 1;
		from = getLeastValuablePiece(b, attadef, side, attackerPiece);
		attackerPiece = pieceAt(b, bitscanForward(from));

	} while (from && attackerPiece);
	while (--d) {
		gain[d - 1] = -max(-gain[d - 1], gain[d]);
	}

	if (isEnPassant(move))
		gain[0] += 100;

	return gain[0];
}

bool see_ge(board_t* b, move_t move, int threshHold) {
	int color;
	int from = fromSq(move);
	int to = toSq(move);
	int nextVictim = pieceAt(b, from);
	int balance = SEEPieceValues[pieceAt(b, to)];

	if (isCastling(move)) {
		return false;
	}

	if (isPromotion(move)) {
		nextVictim = promPiece(b, move);
		balance += (SEEPieceValues[nextVictim] - SEEPieceValues[Piece::PAWN]);
	}

	if (isEnPassant(move)) {
		balance = SEEPieceValues[Piece::PAWN];
	}

	balance -= threshHold;
	balance -= SEEPieceValues[nextVictim];

	// Even after losing the moved piece balance is up
	if (balance >= 0) return true;

	// slider used to detect discovered attack
	bitboard_t bishops = b->pieces[Piece::BISHOP] | b->pieces[Piece::QUEEN];
	bitboard_t rooks = b->pieces[Piece::ROOK] | b->pieces[Piece::QUEEN];

	// copy occupied to simulate captures on bitboard
	bitboard_t occ = b->occupied;
	occ = (occ ^ (1ULL << from)) | (1ULL << to);
	if (isEnPassant(move)) {
		occ ^= (1ULL << b->enPas);
	}

	bitboard_t nowAttacking;
	bitboard_t attackers = squareAtkDefOcc(b, occ, to) & occ;

	color = !b->side;

	while (1) {
		nowAttacking = attackers & b->color[color];

		// No more attackers left
		if (!nowAttacking) {
			break;
		}

		// Find weakest attacker
		for (nextVictim = Piece::PAWN; nextVictim <= Piece::KING; nextVictim++) {
			if (nowAttacking & b->pieces[nextVictim])
				break;
		}

		// Remove least valuable attacker
		occ ^= (1ULL << popBit(nowAttacking & b->pieces[nextVictim]));

		if (nextVictim == Piece::PAWN || nextVictim == Piece::BISHOP || nextVictim == Piece::QUEEN) {
			attackers |= lookUpBishopMoves(to, occ) & bishops;
		}

		if (nextVictim == Piece::ROOK || nextVictim == Piece::QUEEN) {
			attackers |= lookUpRookMoves(to, occ) & rooks;
		}

		attackers &= occ;
		color ^= 1;

		balance = -balance - 1 - SEEPieceValues[nextVictim];

		if (balance >= 0) {

			if (nextVictim == Piece::KING && (attackers & b->color[color])) {
				color ^= 1;
			}

			break;
		}

	}

	return b->side != color;
}

static void updateBestMove(int* scores, int* bestIdx, int curIdx) {
	if (scores[curIdx] > scores[*bestIdx]) {
		*bestIdx = curIdx;
	}
}

void scoreMoves(board_t* b, moveList_t* moveList, move_t hashMove) {
	move_t currentMove;
	int seeScore = 0;
	int mvvLvaScore = 0;
	int bestIdx = 0;
	int capturedPiece = 0;

	//// set all scores to 0
	//for (int i = 0; i < moveList->cnt; i++)
	//	moveList->scores[i] = 0;

	for (int i = 0; i < moveList->cnt; i++) {
		currentMove = moveList->moves[i];
		Assert(currentMove != NO_MOVE);
		Assert(currentMove != NULL_MOVE);

		// hash move
		if (currentMove == hashMove) {
			moveList->scores[i] = HASH_MOVE;

			updateBestMove(moveList->scores, &bestIdx, i);
			continue;
		}

		// enpas move
		if (isEnPassant(currentMove)) {
			moveList->scores[i] = GOOD_CAPTURE + 105;
			updateBestMove(moveList->scores, &bestIdx, i);
			continue;
		}

		// capture moves
		if (capturedPiece = capPiece(b, currentMove)) {
			Assert(pieceValid(capturedPiece));

			if (promPiece(b, currentMove)) {
				// Promoting captures
				moveList->scores[i] = PROMOTING_CAPTURE + pieceAt(b, fromSq(currentMove));
			} else {
				// Plain Captures
				mvvLvaScore = MVV_LVA[capturedPiece][pieceAt(b, fromSq(currentMove))];
				Assert(mvvLvaScore > 0);
				Assert(mvvLvaScore < MVV_LVA_UBOUND);

				if (see_ge(b, currentMove, 0)) {
					moveList->scores[i] = GOOD_CAPTURE + mvvLvaScore;
				} else {
					moveList->scores[i] = BAD_CAPTURE + mvvLvaScore;
				}

			}

			updateBestMove(moveList->scores, &bestIdx, i);
			continue;
		}

		// Only quiet moves left:

		// promotion
		if (promPiece(b, currentMove)) {
			moveList->scores[i] = PROMOTION + pieceAt(b, fromSq(currentMove));

			updateBestMove(moveList->scores, &bestIdx, i);
			continue;
		}

		// mate killer
		if (currentMove == mateKiller[b->ply]) {
			moveList->scores[i] = MATE_KILLER;

			updateBestMove(moveList->scores, &bestIdx, i);
			continue;
		}

		// first killer
		if (currentMove == killer[0][b->ply]) {
			Assert(!capPiece(b, currentMove));
			moveList->scores[i] = KILLER_SCORE_1;

			updateBestMove(moveList->scores, &bestIdx, i);
			continue;
		}

		// second killer
		if (currentMove == killer[1][b->ply]) {
			Assert(!capPiece(b, currentMove));
			moveList->scores[i] = KILLER_SCORE_2;

			updateBestMove(moveList->scores, &bestIdx, i);
			continue;
		}

		// counter move
		if (b->ply > 0) {
			move_t prevMove = b->undoHistory[b->ply - 1].move;
			move_t counterMove = counterHeuristic[fromSq(prevMove)][toSq(prevMove)][b->side];

			if (currentMove == counterMove) {
				moveList->scores[i] = COUNTER_SCORE;
				updateBestMove(moveList->scores, &bestIdx, i);
				continue;
			}

		}

		// castle move
		if (isCastling(currentMove)) {
			moveList->scores[i] = CASTLE_SCORE;

			updateBestMove(moveList->scores, &bestIdx, i);
			continue;
		}

		// last resort: history heuristic
		int histScore = histHeuristic[b->side][fromSq(currentMove)][toSq(currentMove)];
		Assert(histScore >= 0);
		moveList->scores[i] = QUIET_SCORE + (histScore);
		Assert(moveList->scores[i] < COUNTER_SCORE);

		updateBestMove(moveList->scores, &bestIdx, i);
	}

	// swap best move to first position !!!!! score has to be moved too
	//if (moveList->cnt > 1) {
	//	move_t temp = moveList->moves[0];
	//	moveList->moves[0] = moveList->moves[bestIdx];
	//	moveList->moves[0] = temp;
	//}
}

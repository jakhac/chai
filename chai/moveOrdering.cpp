#include "moveOrdering.h"

void initMVV_LVA() {
	int attacker;
	int victim;
	for (attacker = Pieces::P; attacker <= Pieces::k; attacker++) {
		for (victim = Pieces::P; victim <= Pieces::k; victim++) {
			MVV_LVA[victim][attacker] = victimScore[victim] + 6 - (victimScore[attacker] / 100);
		}
	}
}

bitboard_t getLeastValuablePiece(board_t* b, bitboard_t atkDef, int side) {
	atkDef &= b->color[side];
	if (!atkDef) {
		return 0ULL;
	}

	bitboard_t lva;
	for (int i = chai::PAWN; i <= chai::KING; i++) {
		lva = atkDef & b->pieces[i];
		if (lva) {
			return setMask[getLSB(lva)];
		}
	}

	Assert(false);
	return 0ULL;
}

int see(board_t* b, const int move) {
	int to = toSq(move);
	int attackerPiece = pieceAt(b, fromSq(move));
	int gain[32]{}, d = 0, side = b->stm;

	bitboard_t occ = b->occupied;
	bitboard_t mayXray = b->pieces[chai::PAWN] | b->pieces[chai::BISHOP] | b->pieces[chai::ROOK] | b->pieces[chai::QUEEN];
	bitboard_t attadef = squareAtkDef(b, to);
	bitboard_t from = setMask[fromSq(move)];
	bitboard_t used = 0ULL, discovered = 0ULL;

	gain[d] = pieceValues[capPiece(b, move)];
	do {
		Assert(pieceValid(attackerPiece));
		d++; // next depth and side
		gain[d] = -gain[d - 1] + pieceValues[attackerPiece]; // speculative store, if defended

		if (std::max(-gain[d - 1], gain[d]) < 0)
			break; // pruning does not influence the result

		attadef ^= from; // reset bit in set to traverse
		occ ^= from; // reset bit in temporary occupancy (for magic move generation)
		used |= from;

		if (from & mayXray) {
			discovered = 0ULL;
			discovered |= lookUpBishopMoves(to, occ) & (getPieces(b, chai::QUEEN, side) | getPieces(b, chai::BISHOP, side));
			discovered |= lookUpRookMoves(to, occ) & (getPieces(b, chai::QUEEN, side) | getPieces(b, chai::ROOK, side));
			attadef |= discovered & ~used;
		}

		side ^= 1;
		from = getLeastValuablePiece(b, attadef, side);
		attackerPiece = from ? pieceAt(b, getLSB(from)) : 0;

	} while (from && attackerPiece);

	while (--d) {
		gain[d - 1] = -std::max(-gain[d - 1], gain[d]);
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
	int gain[32]{}, d = 0, side = b->stm;

	bitboard_t occ = b->occupied;
	bitboard_t mayXray = b->pieces[chai::PAWN] | b->pieces[chai::BISHOP] | b->pieces[chai::ROOK] | b->pieces[chai::QUEEN];
	bitboard_t attadef = squareAtkDef(b, to);
	bitboard_t from = setMask[fromSq(move)];
	bitboard_t used = 0ULL, discovered = 0ULL;

	gain[d] = pieceValues[capPiece(b, move)];

	// If the captured piece is worth more than the attacker, it is always
	// winning. An estimated SEE score is then returned.
	if (gain[d] > pieceValues[attackerPiece]) {
		return gain[d] - (pieceValues[attackerPiece] / 2);
	}

	do {
		Assert(pieceValid(attackerPiece));
		d++; // next depth and side
		gain[d] = -gain[d - 1] + pieceValues[attackerPiece]; // speculative store, if defended

		if (std::max(-gain[d - 1], gain[d]) < 0)
			break; // pruning does not influence the result

		attadef ^= from; // reset bit in set to traverse
		occ ^= from; // reset bit in temporary occupancy (for magic move generation)
		used |= from;

		if (from & mayXray) {
			discovered = 0ULL;
			discovered |= lookUpBishopMoves(to, occ) & (getPieces(b, chai::QUEEN, side) | getPieces(b, chai::BISHOP, side));
			discovered |= lookUpRookMoves(to, occ) & (getPieces(b, chai::QUEEN, side) | getPieces(b, chai::ROOK, side));
			attadef |= discovered & ~used;
		}

		side ^= 1;
		from = getLeastValuablePiece(b, attadef, side);
		attackerPiece = from ? pieceAt(b, getLSB(from)) : 0;

	} while (from && attackerPiece);
	while (--d) {
		gain[d - 1] = -std::max(-gain[d - 1], gain[d]);
	}

	if (isEnPassant(move))
		gain[0] += 100;

	return gain[0];
}

bool see_ge(board_t* b, move_t move, int threshold) {
	bool color;
	int from = fromSq(move);
	int to = toSq(move);
	int nextVictim = pieceAt(b, from);
	int balance = SEEPieceValues[pieceAt(b, to)];

	if (isCastling(move)) {
		return false;
	}

	if (isPromotion(move)) {
		nextVictim = promPiece(b, move);
		balance += (SEEPieceValues[nextVictim] - SEEPieceValues[chai::PAWN]);
	}

	if (isEnPassant(move)) {
		balance = SEEPieceValues[chai::PAWN];
	}

	balance -= threshold;
	balance -= SEEPieceValues[nextVictim];

	// Even after losing the moved piece balance is up
	if (balance >= 0) return true;

	// slider used to detect discovered attack
	bitboard_t bishops = b->pieces[chai::BISHOP] | b->pieces[chai::QUEEN];
	bitboard_t rooks = b->pieces[chai::ROOK] | b->pieces[chai::QUEEN];

	// copy occupied to simulate captures on bitboard
	bitboard_t occ = b->occupied;
	occ = (occ ^ (1ULL << from)) | (1ULL << to);
	if (isEnPassant(move)) {
		occ ^= (1ULL << b->enPas);
	}

	bitboard_t nowAttacking;
	bitboard_t attackers = squareAtkDefOcc(b, occ, to) & occ;

	color = !b->stm;

	while (1) {
		nowAttacking = attackers & b->color[color];

		// No more attackers left
		if (!nowAttacking) {
			break;
		}

		// Find weakest attacker
		for (nextVictim = chai::PAWN; nextVictim <= chai::KING; nextVictim++) {
			if (nowAttacking & b->pieces[nextVictim])
				break;
		}

		// Remove least valuable attacker
		Assert(nowAttacking & b->pieces[nextVictim]);
		occ ^= (1ULL << getLSB(nowAttacking & b->pieces[nextVictim]));

		if (nextVictim == chai::PAWN || nextVictim == chai::BISHOP || nextVictim == chai::QUEEN) {
			attackers |= lookUpBishopMoves(to, occ) & bishops;
		}

		if (nextVictim == chai::ROOK || nextVictim == chai::QUEEN) {
			attackers |= lookUpRookMoves(to, occ) & rooks;
		}

		attackers &= occ;
		color ^= 1;

		balance = -balance - 1 - SEEPieceValues[nextVictim];

		if (balance >= 0) {

			if (nextVictim == chai::KING && (attackers & b->color[color])) {
				color ^= 1;
			}

			break;
		}

	}

	return b->stm != color;
}

void scoreMoves(board_t* b, moveList_t* moveList, move_t hashMove) {
	move_t currentMove;
	int mvvLvaScore = 0;
	int capturedPiece = 0;

	for (int i = 0; i < moveList->cnt; i++) {
		currentMove = moveList->moves[i];
		Assert(currentMove != MOVE_NONE);
		Assert(currentMove != MOVE_NULL);

		// hash move
		if (currentMove == hashMove) {
			moveList->scores[i] = HASH_MOVE;
			continue;
		}

		// enpas move
		if (isEnPassant(currentMove)) {
			moveList->scores[i] = GOOD_CAPTURE + 105;
			continue;
		}

		// capture moves
		capturedPiece = capPiece(b, currentMove);
		if (capturedPiece) {
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
			continue;
		}

		// Only quiet moves left:

		// promotion
		if (promPiece(b, currentMove)) {
			moveList->scores[i] = PROMOTION + pieceAt(b, fromSq(currentMove));
			continue;
		}

		// mate killer
		if (currentMove == mateKiller[b->ply]) {
			moveList->scores[i] = MATE_KILLER;
			continue;
		}

		// first killer
		if (currentMove == killer[0][b->ply]) {
			Assert(!capPiece(b, currentMove));
			moveList->scores[i] = KILLER_SCORE_1;
			continue;
		}

		// second killer
		if (currentMove == killer[1][b->ply]) {
			Assert(!capPiece(b, currentMove));
			moveList->scores[i] = KILLER_SCORE_2;
			continue;
		}

		// counter move
		if (b->ply > 0) {
			move_t prevMove = b->undoHistory[b->ply - 1].move;
			move_t counterMove = counterHeuristic[fromSq(prevMove)][toSq(prevMove)][b->stm];

			if (currentMove == counterMove) {
				moveList->scores[i] = COUNTER_SCORE;
				continue;
			}

		}

		// castle move
		if (isCastling(currentMove)) {
			moveList->scores[i] = CASTLE_SCORE;
			continue;
		}

		// last resort: history heuristic
		int histScore = histHeuristic[b->stm][fromSq(currentMove)][toSq(currentMove)];
		Assert(0 <= histScore && histScore <= HISTORY_MAX);
		moveList->scores[i] = QUIET_SCORE + (histScore);
		Assert3(moveList->scores[i] < COUNTER_SCORE,
				std::to_string(moveList->scores[i]), std::to_string(COUNTER_SCORE));
	}
}

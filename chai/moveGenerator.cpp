#include "moveGenerator.h"

// cap piece ][ moving piece
int MVV_LVA[13][13];

void generateMoves(Board* b, MOVE_S* move_s) {
	for (int i = 0; i < MAX_POSITION_MOVES; i++) move_s->moveScore[i] = 0;

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

	addKnightMoves(b, move_s);
	addKnightCaptures(b, move_s);

	addBishopMoves(b, move_s);
	addBishopCaptures(b, move_s);

	addKingMoves(b, move_s);
	addKingCaptures(b, move_s);

	addRookMoves(b, move_s);
	addRookCaptures(b, move_s);

	addQueenMoves(b, move_s);
	addQueenCaptures(b, move_s);

}

void generateCaptures(Board* b, MOVE_S* move_s) {
	for (int i = 0; i < MAX_POSITION_MOVES; i++) move_s->moveScore[i] = 0;
	//move_s->attackedSquares = b->attackerSet(b->side ^ 1);

	//b->attackedSquares[WHITE] = b->attackerSet(WHITE);
	//b->attackedSquares[BLACK] = b->attackerSet(BLACK);

	if (b->side == WHITE) {
		whitePawnCaptures(b, move_s);
	} else {
		blackPawnCaptures(b, move_s);
	}

	addKnightCaptures(b, move_s);
	addBishopCaptures(b, move_s);
	addRookCaptures(b, move_s);
	addQueenCaptures(b, move_s);
	addKingCaptures(b, move_s);
}

bool moveLegal(Board* b, const int move) {

	MOVE_S move_s[1];
	generateMoves(b, move_s);

	for (int i = 0; i < move_s->moveCounter; i++) {
		if (move == move_s->moveList[i]) {
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

void addQuietMove(Board* b, MOVE_S* move_s, int move) {
	// score according to killers or zero else
	if (b->killer[0][b->ply] == move) {
		move_s->moveScore[move_s->moveCounter] = 900000;
	} else if (b->killer[1][b->ply] == move) {
		move_s->moveScore[move_s->moveCounter] = 800000;
	} else {
		move_s->moveScore[move_s->moveCounter] = 50000;
	}
	move_s->moveList[move_s->moveCounter] = move;
	move_s->moveCounter++;
}

void addCaptureMove(Board* b, MOVE_S* move_s, int move, int movingPiece) {
	ASSERT(pieceValid(CAPTURED(move)));

	int moveScore;
	int seeScore = see(b, move);
	if (seeScore > 0) {
		moveScore = MVV_LVA[CAPTURED(move)][movingPiece] + 1100000;
	} else if (seeScore == 0) {
		moveScore = MVV_LVA[CAPTURED(move)][movingPiece] + 1000000;
	} else {
		moveScore = MVV_LVA[CAPTURED(move)][movingPiece];
	}

	move_s->moveScore[move_s->moveCounter] = moveScore;
	move_s->moveList[move_s->moveCounter] = move;
	move_s->moveCounter++;
}

void addEnPassantMove(Board* b, MOVE_S* move_s, int move) {
	move_s->moveScore[move_s->moveCounter] = 105 + 1000000;
	move_s->moveList[move_s->moveCounter] = move;
	move_s->moveCounter++;
}

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
U64 getLeastValuablePiece(Board* b, U64 attadef, int side, int attackerPiece) {
	attadef &= b->color[side];
 	if (!attadef) {
		return 0ULL;
	}

	U64 lva;
	for (int i = PAWN; i <= KING; i++) {
		lva = attadef & b->pieces[i];
		if (lva) {
			return setMask[bitscanForward(lva)];
		}
	}

	ASSERT(false);
}

int see(Board* b, const int move) {
	int toSq = TOSQ(move);
	int attackerPiece = b->pieceAt(FROMSQ(move));
	int gain[32], d = 0, side = b->side;

	U64 occ = b->occupied;
	U64 mayXray = b->pieces[PAWN] | b->pieces[BISHOP] | b->pieces[ROOK] | b->pieces[QUEEN];
	U64 attadef = b->squareAttacked(toSq);
	U64 from = setMask[FROMSQ(move)];
	U64 used = 0ULL, discovered = 0ULL;
	
	gain[d] = pieceScores[CAPTURED(move)];
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
			discovered |= lookUpBishopMoves(toSq, occ) & (b->getPieces(QUEEN, side) | b->getPieces(BISHOP, side));
			discovered |= lookUpRookMoves(toSq, occ) & (b->getPieces(QUEEN, side) | b->getPieces(ROOK, side));
			attadef |= discovered & ~used;
		}

		side ^= 1;
		from = getLeastValuablePiece(b, attadef, side, attackerPiece);
		attackerPiece = b->pieceAt(bitscanForward(from));

	} while (from && attackerPiece);
	while (--d) {
		gain[d - 1] = -max(-gain[d - 1], gain[d]);
	}

	if (MFLAGEP & move) gain[0] += 100;

	return gain[0];

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
		addQuietMove(b, move_s, MOVE(sq - 8, sq, EMPTY, EMPTY, EMPTY));
	}

	// prom pawn pushes
	while (promPawns) {
		sq = popBit(&promPawns);
		addQuietMove(b, move_s, MOVE(sq - 8, sq, EMPTY, Q, EMPTY));
		addQuietMove(b, move_s, MOVE(sq - 8, sq, EMPTY, R, EMPTY));
		addQuietMove(b, move_s, MOVE(sq - 8, sq, EMPTY, B, EMPTY));
		addQuietMove(b, move_s, MOVE(sq - 8, sq, EMPTY, N, EMPTY));
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
		addQuietMove(board, move_s, MOVE(sq + 8, sq, EMPTY, EMPTY, EMPTY));
	}

	// prom pawn pushes
	while (promPawns) {
		sq = popBit(&promPawns);
		addQuietMove(board, move_s, MOVE(sq + 8, sq, EMPTY, q, EMPTY));
		addQuietMove(board, move_s, MOVE(sq + 8, sq, EMPTY, r, EMPTY));
		addQuietMove(board, move_s, MOVE(sq + 8, sq, EMPTY, b, EMPTY));
		addQuietMove(board, move_s, MOVE(sq + 8, sq, EMPTY, n, EMPTY));
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
		addQuietMove(board, move_s, MOVE(sq - 16, sq, EMPTY, EMPTY, MFLAGPS));
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
		addQuietMove(board, move_s, MOVE(sq + 16, sq, EMPTY, EMPTY, MFLAGPS));
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
		addEnPassantMove(board, move_s, MOVE(board->enPas - 7, board->enPas, EMPTY, EMPTY, MFLAGEP));
	}
	if ((whitePawns << 9 & ~FILE_A_HEX) & setMask[board->enPas]) {
		addEnPassantMove(board, move_s, MOVE(board->enPas - 9, board->enPas, EMPTY, EMPTY, MFLAGEP));
	}

	while (whitePawns) {
		sq = popBit(&whitePawns);
		atks = pawnAtkMask[WHITE][sq] & board->color[BLACK];
		while (atks) {
			atk_sq = popBit(&atks);
			addCaptureMove(board, move_s, MOVE(sq, atk_sq, board->pieceAt(atk_sq), EMPTY, EMPTY), P);
		}
	}

	// promoting attacks
	while (whitePawnProm) {
		sq = popBit(&whitePawnProm);
		atks = pawnAtkMask[WHITE][sq] & board->color[BLACK];
		while (atks) {
			atk_sq = popBit(&atks);
			addCaptureMove(board, move_s, MOVE(sq, atk_sq, board->pieceAt(atk_sq), Q, EMPTY), P);
			addCaptureMove(board, move_s, MOVE(sq, atk_sq, board->pieceAt(atk_sq), R, EMPTY), P);
			addCaptureMove(board, move_s, MOVE(sq, atk_sq, board->pieceAt(atk_sq), B, EMPTY), P);
			addCaptureMove(board, move_s, MOVE(sq, atk_sq, board->pieceAt(atk_sq), N, EMPTY), P);
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
		addEnPassantMove(board, move_s, MOVE(board->enPas + 7, board->enPas, EMPTY, EMPTY, MFLAGEP));
	}
	if ((blackPawns >> 9 & ~FILE_H_HEX) & setMask[board->enPas]) {
		addEnPassantMove(board, move_s, MOVE(board->enPas + 9, board->enPas, EMPTY, EMPTY, MFLAGEP));
	}

	while (blackPawns) {
		sq = popBit(&blackPawns);
		atks = pawnAtkMask[BLACK][sq] & board->color[WHITE];
		while (atks) {
			atk_sq = popBit(&atks);
			addCaptureMove(board, move_s, MOVE(sq, atk_sq, board->pieceAt(atk_sq), EMPTY, EMPTY), p);
		}
	}

	// promoting attacks
	while (blackPawnProm) {
		sq = popBit(&blackPawnProm);
		atks = pawnAtkMask[BLACK][sq] & board->color[WHITE];
		while (atks) {
			atk_sq = popBit(&atks);
			addCaptureMove(board, move_s, MOVE(sq, atk_sq, board->pieceAt(atk_sq), q, EMPTY), p);
			addCaptureMove(board, move_s, MOVE(sq, atk_sq, board->pieceAt(atk_sq), r, EMPTY), p);
			addCaptureMove(board, move_s, MOVE(sq, atk_sq, board->pieceAt(atk_sq), b, EMPTY), p);
			addCaptureMove(board, move_s, MOVE(sq, atk_sq, board->pieceAt(atk_sq), n, EMPTY), p);
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
			addQuietMove(b, move_s, MOVE(sq, atk_sq, EMPTY, EMPTY, EMPTY));
		}
	}
}

void addKnightCaptures(Board* b, MOVE_S* move_s) {
	int sq, atk_sq;
	int piece = (b->side == WHITE) ? N : n;
	U64 knights = b->getPieces(KNIGHT, b->side);
	U64 atks;

	while (knights) {
		sq = popBit(&knights);
		atks = knightAtkMask[sq] & b->color[b->side ^ 1];
		while (atks) {
			atk_sq = popBit(&atks);
			addCaptureMove(b, move_s, MOVE(sq, atk_sq, b->pieceAt(atk_sq), EMPTY, EMPTY), piece);
		}
	}
}

void addKingMoves(Board* b, MOVE_S* move_s) {
	int sq, kSq = b->getKingSquare(b->side);
	U64 kingMoves = kingAtkMask[kSq] & ~move_s->attackedSquares & ~b->occupied;

	while (kingMoves) {
		sq = popBit(&kingMoves);
		addQuietMove(b, move_s, MOVE(kSq, sq, EMPTY, EMPTY, EMPTY));
	}

	switch (b->side) {
		case WHITE:	
			if (b->castleValid(K_CASTLE, &move_s->attackedSquares)) {
				ASSERT(kSq == E1);
				addQuietMove(b, move_s, MOVE(kSq, G1, EMPTY, EMPTY, MFLAGCA));
			}
			if (b->castleValid(Q_CASTLE, &move_s->attackedSquares)) {
				ASSERT(kSq == E1);
				addQuietMove(b, move_s, MOVE(kSq, C1, EMPTY, EMPTY, MFLAGCA));
			}
			break;
		case BLACK:
			if (b->castleValid(k_CASTLE, &move_s->attackedSquares)) {
				ASSERT(kSq == E8);
				addQuietMove(b, move_s, MOVE(kSq, G8, EMPTY, EMPTY, MFLAGCA));
			}

			if (b->castleValid(q_CASTLE, &move_s->attackedSquares)) {
				ASSERT(kSq == E8);
				addQuietMove(b, move_s, MOVE(kSq, C8, EMPTY, EMPTY, MFLAGCA));
			}
			break;
		default: break;
	}
}

void addKingCaptures(Board* b, MOVE_S* move_s) {
	int piece = (b->side == WHITE) ? K : k;
	int atk_sq, kSq = b->getKingSquare(b->side);
	U64 whiteKingAttacks = kingAtkMask[kSq] & ~move_s->attackedSquares & b->color[b->side^1];

	while (whiteKingAttacks) {
		atk_sq = popBit(&whiteKingAttacks);
		addCaptureMove(b, move_s, MOVE(kSq, atk_sq, b->pieceAt(atk_sq), EMPTY, EMPTY), piece);
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
			addQuietMove(b, move_s, MOVE(sq, atk_sq, EMPTY, EMPTY, EMPTY));
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
			addQuietMove(b, move_s, MOVE(sq, atk_sq, EMPTY, EMPTY, EMPTY));
		}
	}
}

void addRookCaptures(Board* b, MOVE_S* move_s) {
	int piece = (b->side == WHITE) ? R : r;
	int sq, atk_sq;
	U64 captureSet;
	U64 rooks = b->getPieces(ROOK, b->side);
	while (rooks) {
		sq = popBit(&rooks);
		captureSet = lookUpRookMoves(sq, b->occupied);
		captureSet = captureSet & b->color[b->side ^ 1];

		while (captureSet) {
			atk_sq = popBit(&captureSet);
			addCaptureMove(b, move_s, MOVE(sq, atk_sq, b->pieceAt(atk_sq), EMPTY, EMPTY), piece);
		}
	}
}

void addBishopCaptures(Board* board, MOVE_S* move_s) {
	int piece = (board->side == WHITE) ? B : b;
	int sq, atk_sq;
	U64 captureSet;
	U64 bishops = board->getPieces(BISHOP, board->side);
	while (bishops) {
		sq = popBit(&bishops);
		captureSet = lookUpBishopMoves(sq, board->occupied);
		captureSet &= board->color[board->side ^ 1];

		while (captureSet) {
			atk_sq = popBit(&captureSet);
			addCaptureMove(board, move_s, MOVE(sq, atk_sq, board->pieceAt(atk_sq), EMPTY, EMPTY), piece);
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
			addQuietMove(b, move_s, MOVE(sq, atk_sq, EMPTY, EMPTY, EMPTY));
		}
	}
}

void addQueenCaptures(Board* b, MOVE_S* move_s) {
	int piece = (b->side == WHITE) ? Q : q;
	int sq, atk_sq;
	U64 attackSet;
	U64 queen = b->getPieces(QUEEN, b->side);
	while (queen) {
		sq = popBit(&queen);
		attackSet = lookUpRookMoves(sq, b->occupied) ^ lookUpBishopMoves(sq, b->occupied);
		attackSet &= b->color[b->side ^ 1];

		while (attackSet) {
			atk_sq = popBit(&attackSet);
			addCaptureMove(b, move_s, MOVE(sq, atk_sq, b->pieceAt(atk_sq), EMPTY, EMPTY), piece);
		}
	}
}

void printGeneratedMoves(MOVE_S* move_s) {

	cout << "\nGenerated " << move_s->moveCounter << " moves: " << endl;

	for (int i = 0; i < move_s->moveCounter; i++) {
		cout << i << ". " << getStringMove(move_s->moveList[i]) << endl;;
	}
}
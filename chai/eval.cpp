#include "eval.h"

// include extern vars
const int* maps[7];
bitboard_t pawnIsolatedMask[64];
bitboard_t pawnPassedMask[2][64];
bitboard_t upperMask[64];
bitboard_t lowerMask[64];
bitboard_t pawnShield[2][64];
bitboard_t xMask[64];
int manhattenDistance[64][64];

// 0 == a / 1 == b
float interpolate(int a, int b, float t) {
	return (float)a + t * ((float)b - (float)a);
}

value_t evalPST(board_t* b, int side, float* t) {
	bitboard_t pieces;
	int score = 0, kSq = getKingSquare(b, side), sq;

	// PAWNS
	pieces = getPieces(b, Piece::PAWN, side);
	int kingPawnDistance = 0, pawnSum = countBits(pieces);
	while (pieces) {
		sq = popBit(&pieces);

		// king pawn tropism, average distance to own pawns
		kingPawnDistance += manhattenDistance[kSq][sq];

		sq = (side == WHITE) ? sq : mirror64[sq];
		score += (int)interpolate(PAWN_OPEN[sq], PAWN_ENDGAME[sq], *t);
	}
	kingPawnDistance /= max(1, pawnSum);
	score += (int)interpolate(kingPawnDistance, kingPawnDistance * 2, *t);

	// KNIGHTS
	pieces = getPieces(b, Piece::KNIGHT, side);
	while (pieces) {
		sq = popBit(&pieces);
		sq = (side == WHITE) ? sq : mirror64[sq];
		score += (int)interpolate(KNIGHT_OPEN[sq], KNIGHT_ENDGAME[sq], *t);
	}

	pieces = getPieces(b, Piece::BISHOP, side);
	while (pieces) {
		sq = popBit(&pieces);
		sq = (side == WHITE) ? sq : mirror64[sq];
		score += (int)interpolate(BISHOP_OPEN[sq], BISHOP_ENDGAME[sq], *t);
	}

	pieces = getPieces(b, Piece::ROOK, side);
	while (pieces) {
		sq = popBit(&pieces);
		sq = (side == WHITE) ? sq : mirror64[sq];
		score += (int)interpolate(ROOK_OPEN[sq], ROOK_ENDGAME[sq], *t);
	}

	pieces = getPieces(b, Piece::QUEEN, side);
	while (pieces) {
		sq = popBit(&pieces);
		sq = (side == WHITE) ? sq : mirror64[sq];
		score += (int)interpolate(QUEEN_OPEN[sq], QUEEN_ENDGAME[sq], *t);
	}

	sq = (side == WHITE) ? kSq : mirror64[kSq];
	score += (int)interpolate(KING_OPENING[sq], KING_ENDGAME[sq], *t);

	return score;
}

value_t materialScore(board_t* b, int side) {
	int score = 0;
	for (int i = 1; i < 7; i++) {
		score += countBits(getPieces(b, i, side)) * pieceScores[i];
	}

	return score;
}

// number of isolated pawns
value_t isolatedPawns(board_t* b, int side) {
	int isolated = 0, sq;
	bitboard_t pawns = getPieces(b, Piece::PAWN, side);
	bitboard_t refPawns = pawns;

	while (pawns) {
		sq = popBit(&pawns);
		if (!(pawnIsolatedMask[sq] & refPawns)) {
			isolated++;
		}
	}

	return isolated;
}

// number of passed pawns
value_t passedPawns(board_t* b, int side) {
	int passedScore = 0, sq;
	bitboard_t pawns = getPieces(b, Piece::PAWN, side);
	bitboard_t oppPawns = getPieces(b, Piece::PAWN, side ^ 1);
	while (pawns) {
		sq = popBit(&pawns);
		if (!(pawnPassedMask[side][sq] & oppPawns)) {
			passedScore += passedBonus[side][squareToRank[sq]];
		}
	}

	//return passedScore;
	return (int)interpolate(passedScore, scale(4, passedScore), (float)b->halfMoves);
}

value_t stackedPawn(board_t* b, int side) {
	int stackedPenalty = 0;

	bitboard_t pawns = getPieces(b, Piece::PAWN, side);
	for (int i = 0; i < 8; i++) {
		if (countBits(FILE_LIST[i] & pawns) > 1) {
			stackedPenalty -= 4;
		}
	}

	return stackedPenalty;
}

value_t pawnChain(board_t* b, int side) {
	int result = 0;
	bitboard_t pawns = getPieces(b, Piece::PAWN, side);
	bitboard_t tPawns = pawns;

	// use xMask to reward protected pawns
	int sq;
	while (pawns) {
		sq = popBit(&pawns);
		result += (bool)(tPawns & xMask[sq]);
	}

	return result;
}

value_t openFilesRQ(board_t* b, int side) {
	int sq, score = 0;
	bitboard_t pawns = b->pieces[Piece::PAWN];

	// openFileBonus for rooks
	bitboard_t rooks = getPieces(b, Piece::ROOK, side);
	bitboard_t oppKing = getKingSquare(b, side ^ 1);
	while (rooks) {
		sq = popBit(&rooks);
		if (!(setMask[squareToFile[sq]] & pawns)) {
			score += openFileBonusR;
		}

		// small bonus for rooks on same file as queen or king
		if (FILE_LIST[squareToFile[sq]] & oppKing) {
			score += 3;
		}
	}

	// openFileBonus for queens
	bitboard_t queens = getPieces(b, Piece::QUEEN, side);
	while (queens) {
		sq = popBit(&queens);
		if (!(setMask[squareToFile[sq]] & pawns)) {
			score += openFileBonusQ;
		}
	}

	return score;
}

value_t bishopPair(board_t* b, int side) {
	Assert(((bool)(countBits(getPieces(b, Piece::BISHOP, side)) >= 2)) * 30 <= 30);
	return ((bool)(countBits(getPieces(b, Piece::BISHOP, side)) >= 2)) * 30;
}

value_t kingSafety(board_t* b, int side, float* t) {
	int result = 0;
	int kSq = getKingSquare(b, side);
	bitboard_t pawns = getPieces(b, Piece::PAWN, side);

	// count pawn shielder
	result += countBits(pawnShield[side][kSq] & pawns) * 3;

	// punish attacked squares around king 
	result += 8 - countBits(kingAtkMask[kSq] & b->attackedSquares[side ^ 1]);

	int openFilePenalty = 0;
	// if not endgame
	if (!(countBits(b->occupied) <= 7 || countMajorPieces(b, side) <= 6)) {
		int file;
		// punish open king file
		if (!(FILE_LIST[squareToFile[kSq]] & pawns)) {
			result -= 8;
		}

		file = squareToFile[kSq - 1];
		if (fileValid(file) && !(FILE_LIST[file] & pawns)) {
			result -= 5;
		}

		file = squareToFile[kSq + 1];
		if (fileValid(file) && !(FILE_LIST[file] & pawns)) {
			result -= 5;
		}
	}

	// punish pinned pieces (excluding pawns) to kSq
	result -= countBits(getPinned(b, kSq, side) & ~pawns);

	//scale depending on gamestate
	result = (int)interpolate(result * 2, result, *t);

	int attackedKingSquares = countBits(kingAtkMask[kSq] & b->attackedSquares[side ^ 1]);
	Assert(attackedKingSquares <= 8);

	result -= kingZoneTropism[attackedKingSquares];

	Assert(abs(result) < ISMATE);
	return result;
}

value_t mobility(board_t* b, bool side, float* t) {
	int mobility = 0;
	int restoreSide = b->side;
	//int interpolFactor = // TODO ?

	// how many pieces are attacked by side
	mobility += countBits(b->attackedSquares[side] & b->color[side ^ 1]) / 4;

	// weighted sum of possible moves, reward knight, bishop and rook moves
	moveList_t move_s[1];

	// change side for move generation
	if (b->side != side) b->side = side;

	if (b->side == WHITE) {
		whiteSinglePawnPush(b, move_s);
		whiteDoublePawnPush(b, move_s);
		whitePawnCaptures(b, move_s);
	} else {
		blackSinglePawnPush(b, move_s);
		blackDoublePawnPush(b, move_s);
		blackPawnCaptures(b, move_s);
	}
	mobility += (int)interpolate(move_s->cnt / 6, move_s->cnt, *t);
	move_s->cnt = 0;

	int pieceMoves = 0;
	addBishopCaptures(b, move_s);
	addBishopMoves(b, move_s);
	mobility += move_s->cnt;
	move_s->cnt = 0;

	addKnightMoves(b, move_s);
	addKnightCaptures(b, move_s);
	mobility += move_s->cnt;
	move_s->cnt = 0;

	addRookMoves(b, move_s);
	addRookCaptures(b, move_s);
	mobility += move_s->cnt / 3;
	move_s->cnt = 0;
	b->side = restoreSide;

	return mobility;
}

int scale(int scaler, int pressure) {
	int scaledPressure;
	switch (scaler) {
		case 0: scaledPressure = 0; break;
		case 1: scaledPressure = 1; break;
		case 2: scaledPressure = pressure; break;
		case 3: scaledPressure = (pressure * 4) / 3; break;
		case 4: scaledPressure = (pressure * 3) / 2; break;
		default: scaledPressure = pressure * 2; break;
	}

	return scaledPressure;
}

value_t evaluatePawns(board_t* b, float* t) {
	value_t score = 0;

	// lack of pawns penalty
	if (!getPieces(b, Piece::PAWN, WHITE)) score -= 16;
	if (!getPieces(b, Piece::PAWN, WHITE)) score += 16;

	// isolani
	score += -10 * (isolatedPawns(b, WHITE) - isolatedPawns(b, BLACK));

	// passed
	int passed = 5 * passedPawns(b, WHITE) - passedPawns(b, BLACK);
	passed = (int)interpolate(passed, passed + 20, *t);
	// blocked pawns

	// stacked pawns
	score += stackedPawn(b, WHITE) - stackedPawn(b, BLACK);

	// pawn mobility
	score += pawnChain(b, WHITE) - pawnChain(b, BLACK);

	// rams

	// reward defended pawns

	Assert(abs(score) < ISMATE);
	return score;
}


value_t evaluation(board_t* b) {
	value_t eval = 0;
	float interpolFactor = min(1.f, (float)b->halfMoves / (float)(70 + countBits(b->occupied)));

	prefetchPawnEntry(b);

	// Check insufficient material to detect drawn positions
	if (countBits(b->occupied) <= 5 && insufficientMaterial(b)) {
		return 0;
	}

	// calculate reused bitboards once and share between functions
	b->attackedSquares[WHITE] = attackerSet(b, WHITE);
	b->attackedSquares[BLACK] = attackerSet(b, BLACK);

	b->pt->probed++;
	value_t pawnEval = 0;
	bool foundHash = probePawnEntry(b, &pawnEval);
	if (foundHash) {
		b->pt->hit++;
	} else {
		pawnEval = evaluatePawns(b, &interpolFactor);
		storePawnEntry(b, pawnEval);
	}

	// squares controlled
	int centerSquares = (countBits(b->attackedSquares[WHITE] & CENTER_SQUARES) -
						 countBits(b->attackedSquares[BLACK] & CENTER_SQUARES));
	int surroundingSquares = countBits(b->attackedSquares[WHITE] & ~CENTER_SQUARES) -
		countBits(b->attackedSquares[BLACK] & ~CENTER_SQUARES);
	int kingSquares = countBits(b->attackedSquares[WHITE] & kingAtkMask[getKingSquare(b, WHITE)]) -
		countBits(b->attackedSquares[BLACK] & kingAtkMask[getKingSquare(b, BLACK)]);

	eval += surroundingSquares + 2 * centerSquares + 3 * kingSquares;
	eval += pawnEval;
	eval += evalPST(b, WHITE, &interpolFactor) - evalPST(b, BLACK, &interpolFactor);
	eval += materialScore(b, WHITE) - materialScore(b, BLACK);
	eval += openFilesRQ(b, WHITE) - openFilesRQ(b, BLACK);
	eval += bishopPair(b, WHITE) - bishopPair(b, BLACK);
	eval += kingSafety(b, WHITE, &interpolFactor) - kingSafety(b, BLACK, &interpolFactor);
	eval += mobility(b, WHITE, &interpolFactor) - mobility(b, BLACK, &interpolFactor);

	Assert(abs(eval) < ISMATE);

	// white scores positive and black scores negative
	int sign = (b->side == WHITE) ? 1 : -1;
	return eval * sign;
}

value_t lazyEvaluation(board_t* b) {
	value_t eval = 0;
	float interpolFactor = min(1.f, (float)b->halfMoves / (float)(70 + countBits(b->occupied)));

	b->pt->probed++;
	value_t pawnEval = 0;
	bool foundHash = probePawnEntry(b, &pawnEval);
	if (foundHash) {
		b->pt->hit++;
		eval += pawnEval;
	}

	eval += evalPST(b, WHITE, &interpolFactor) - evalPST(b, BLACK, &interpolFactor);
	eval += materialScore(b, WHITE) - materialScore(b, BLACK);

	Assert(abs(eval) < ISMATE);

	// white scores positive and black scores negative
	int sign = (b->side == WHITE) ? 1 : -1;
	return eval * sign;
}

value_t contemptFactor(board_t* b) {

	value_t contempt = lazyEvaluation(b);

	switch (b->side) {
		case WHITE:
			return (contempt > 100) ? -50 : 0;
			break;
		case BLACK:
			return (contempt < -100) ? 50 : 0;
			break;
	}
	return 0;
}

bool insufficientMaterial(board_t* b) {

	// Trivial case, most likely to happen
	if (countBits(b->occupied) > 5) {
		return false;
	}

	// King vs King
	if (countBits(b->occupied) == 2) {
		return true;
	}

	// 3 pieces on board:
	if (countBits(b->occupied) == 3) {
		// King Knight vs King
		if (b->pieces[Piece::KNIGHT]) {
			return true;
		}

		// King Bishop vs King
		if (b->pieces[Piece::BISHOP]) {
			return true;
		}
	}

	if (countBits(b->occupied) == 4) {
		// King Bishop vs King Bishop (all same color)
		if (countBits(b->pieces[Piece::BISHOP] & WHITE_SQUARES) == 2 &&
			countBits(b->pieces[Piece::BISHOP] & BLACK_SQUARES) == 2) {
			return true;
		}

	}

	if (countBits(b->occupied) == 5) {
		// King Bishop Bishop vs King Bishop (all same color)
		if (countBits(b->pieces[Piece::BISHOP] & WHITE_SQUARES) == 3 &&
			countBits(b->pieces[Piece::BISHOP] & BLACK_SQUARES) == 3) {
			return true;
		}

	}

	return false;
}


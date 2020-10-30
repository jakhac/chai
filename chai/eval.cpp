#include "eval.h"

// include extern vars
const int* maps[7];
U64 pawnIsolatedMask[64];
U64 pawnPassedMask[2][64];
U64 upperMask[64];
U64 lowerMask[64];
U64 pawnShield[2][64];
U64 xMask[64];


void initEval() {
	maps[1] = PAWN_OPEN;
	maps[2] = KNIGHT_OPEN;
	maps[3] = BISHOP_OPEN;
	maps[4] = ROOK_OPEN;
	maps[5] = QUEEN_OPEN;
	maps[6] = KING_OPENING;

	int file;
	U64 left, right;
	for (int i = 0; i < 64; i++) {
		file = squareToFile[i] - 1;
		left = (file >= 0 ) ? FILE_LIST[file] : 0ULL;

		file = squareToFile[i] + 1;
		right = (file <= 7) ? FILE_LIST[file] : 0ULL;

		pawnIsolatedMask[i] = left | right;
	}

	for (int i = 0; i < 64; i++) {
		int rank = squareToRank[i] + 1;
		while (rank <= 7) {
			upperMask[i] |= RANK_LIST[rank];
			rank++;
		}

		rank = squareToRank[i] - 1;
		while (rank >= 0) {
			lowerMask[i] |= RANK_LIST[rank];
			rank--;
		}
	}

	for (int i = 0; i < 64; i++) {
		pawnPassedMask[WHITE][i] = upperMask[i] & (FILE_LIST[squareToFile[i]] | pawnIsolatedMask[i]);
		pawnPassedMask[BLACK][i] = lowerMask[i] & (FILE_LIST[squareToFile[i]] | pawnIsolatedMask[i]);
	}

	U64 shield;
	for (int i = 0; i < 64; i++) {
		shield = 0ULL;
		shield = (setMask[i] >> 1 & ~FILE_H_HEX) | (setMask[i] << 1 & ~FILE_A_HEX) | setMask[i];
		pawnShield[WHITE][i] = (shield << 8) | (shield << 16);
		pawnShield[BLACK][i] = (shield >> 8) | (shield >> 16);
	}

	for (int i = 0; i < 64; i++) {
		xMask[i] = pawnAtkMask[WHITE][i] | pawnAtkMask[BLACK][i];
	}


}

float interpolate(int a, int b, float t) {
	return (float)a + t * ((float)b - (float)a);
}

int evalBonusMaps(Board* b, int side) {
	U64 pieces;
	int score = 0, sq;

	// 0 == a / 1 == b
	float interpolFactor = min(1, (float)b->halfMoves / (float)(70 + countBits(b->occupied))); 

	pieces = b->getPieces(PAWN, side);
	while (pieces) {
		sq = popBit(&pieces);
		sq = (side == WHITE) ? sq : mirror64[sq];
		score += (int) interpolate(PAWN_OPEN[sq], PAWN_ENDGAME[sq], interpolFactor);
	}

	pieces = b->getPieces(KNIGHT, side);
	while (pieces) {
		sq = popBit(&pieces);
		sq = (side == WHITE) ? sq : mirror64[sq];
		score += (int) interpolate(KNIGHT_OPEN[sq], KNIGHT_ENDGAME[sq], interpolFactor);
	}

	pieces = b->getPieces(BISHOP, side);
	while (pieces) {
		sq = popBit(&pieces);
		sq = (side == WHITE) ? sq : mirror64[sq];
		score += (int) interpolate(BISHOP_OPEN[sq], BISHOP_ENDGAME[sq], interpolFactor);
	}

	pieces = b->getPieces(ROOK, side);
	while (pieces) {
		sq = popBit(&pieces);
		sq = (side == WHITE) ? sq : mirror64[sq];
		score += (int) interpolate(ROOK_OPEN[sq], ROOK_ENDGAME[sq], interpolFactor);
	}

	pieces = b->getPieces(QUEEN, side);
	while (pieces) {
		sq = popBit(&pieces);
		sq = (side == WHITE) ? sq : mirror64[sq];
		score += (int) interpolate(QUEEN_OPEN[sq], QUEEN_ENDGAME[sq], interpolFactor);
	}

	pieces = b->getPieces(KING, side);
	while (pieces) {
		sq = popBit(&pieces);
		sq = (side == WHITE) ? sq : mirror64[sq];
		score += (int) interpolate(KING_OPENING[sq], KING_ENDGAME[sq], interpolFactor);
	}

	return score;
}

int materialScore(Board* b, int side) {
	int score = 0;
	for (int i = 1; i < 7; i++) {
		score += countBits(b->getPieces(i, side)) * pieceScores[i];
	}

	return score;
}

// number of isolated pawns
int isolatedPawns(Board* b, int side) {
	int isolated = 0, sq;
	U64 pawns = b->getPieces(PAWN, side);
	U64 refPawns = pawns;

	while (pawns) {
		sq = popBit(&pawns);
		if (!(pawnIsolatedMask[sq] & refPawns)) {
			isolated++;
		}
	}

	return isolated;
}

// number of passed pawns
int passedPawns(Board* b, int side) {
	int passedScore = 0, sq;
	U64 pawns = b->getPieces(PAWN, side);
	U64 oppPawns = b->getPieces(PAWN, side ^ 1);
	while (pawns) {
		sq = popBit(&pawns);
		if (!(pawnPassedMask[side][sq] & oppPawns)) {
			passedScore += passedBonus[side][squareToRank[sq]];
		}
	}

	//return passedScore;
	return interpolate(passedScore, scale(4, passedScore), b->halfMoves);
}

int stackedPawn(Board* b, int side) {
	int stackedPenalty = 0;

	U64 pawns = b->getPieces(PAWN, side);
	for (int i = 0; i < 8; i++) {
		if (countBits(FILE_LIST[i] & pawns) > 1) {
			stackedPenalty -= 4;
		}
	}

	return stackedPenalty;
}

int pawnChain(Board* b, int side) {
	int result = 0;
	U64 pawns = b->getPieces(PAWN, side);
	U64 tPawns = pawns;

	// use xMask to reward protected pawns
	int sq;
	while (pawns) {
		sq = popBit(&pawns);
		result += (bool) (tPawns & xMask[sq]);
	}

	return result;
}

int openFilesRQ(Board* b, int side) {
	int sq, score = 0;
	U64 pawns = b->pieces[PAWN];

	// openFileBonus for rooks
	U64 rooks = b->getPieces(ROOK, side);
	U64 oppKing = b->getKingSquare(side^1);
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
	U64 queens = b->getPieces(QUEEN, side);
	while (queens) {
		sq = popBit(&queens);
		if (!(setMask[squareToFile[sq]] & pawns)) {
			score += openFileBonusQ;
		}
	}

	return score;
}

int bishopPair(Board* b, int side) {
	ASSERT(((bool)(countBits(b->getPieces(BISHOP, side)) >= 2)) * 30 <= 30);
	return ((bool) (countBits(b->getPieces(BISHOP, side)) >= 2)) * 30;
}

int kingSafety(Board* b, int side) {
	int result = 0;
	int kSq = b->getKingSquare(side);
	U64 pawns = b->getPieces(PAWN, side);

	// count pawn shielder
	result += countBits(pawnShield[side][kSq] & pawns) * 2;

	// punish attacked squares around king 
	result += 8 - countBits(kingAtkMask[kSq] & b->attackedSquares[side^1]);

	int openFilePenalty = 0;
	// if not endgame
	if (!(countBits(b->occupied) <= 7 || b->countMajorPieces(side) <= 6)) {
		int file;
		// punish open king file
		if (!(FILE_LIST[squareToFile[kSq]] & pawns)) {
			result -= 8;
		}

		file = squareToFile[kSq-1];
		if (fileValid(file) && !(FILE_LIST[file] & pawns)) {
			result -= 4;
		}

		file = squareToFile[kSq + 1];
		if (fileValid(file) && !(FILE_LIST[file] & pawns)) {
			result -= 4;
		}
	}

	// punish pinned pieces (excluding pawns) to kSq
	result -= countBits(b->pinned(kSq, side) & ~pawns);

	 //scale depending on gamestate
	switch (b->gameState) {
		case START:
			result = scale(4, result);
			break;
		case MID:
			result = scale(3, result);
			break;
		case END:
			result = scale(2, result);
			break;
		default: break;
	}

	return result;
}

int mobility(Board* b, int side) {
	int mobility = 0;
	int restoreSide = b->side;

	// how many pieces are attacked by side
	 mobility += countBits(b->attackedSquares[side] & b->color[side^1]) / 4;

	// weighted sum of possible moves, reward knight, bishop and rook moves
	MOVE_S move_s[1];

	// change side for move generation
	if (b->side != side) b->side = side;

	if (b->gameState == END) {
		if (b->side == WHITE) {
			whiteSinglePawnPush(b, move_s);
			whiteDoublePawnPush(b, move_s);
			whitePawnCaptures(b, move_s);
		} else {
			blackSinglePawnPush(b, move_s);
			blackDoublePawnPush(b, move_s);
			blackPawnCaptures(b, move_s);
		}
		mobility += move_s->moveCounter / 6;
		move_s->moveCounter = 0;
	}

	int pieceMoves = 0;
	addBishopCaptures(b, move_s);
	addBishopMoves(b, move_s);
	mobility += move_s->moveCounter;
	move_s->moveCounter = 0;

	addKnightMoves(b, move_s);
	addKnightCaptures(b, move_s);
	mobility += move_s->moveCounter;
	move_s->moveCounter = 0;

	addRookMoves(b, move_s);
	addRookCaptures(b, move_s);
	mobility += move_s->moveCounter / 3;
	move_s->moveCounter = 0;
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

int evaluatePawns(Board* b) {
	int score = 0;

	// isolani
	score += -10 * (isolatedPawns(b, WHITE) - isolatedPawns(b, BLACK));

	// passed
	int passed = 5 * passedPawns(b, WHITE) - passedPawns(b, BLACK);
	//switch (b->gameState) {
	//	case START: passed = scale(2, passed); break;
	//	case MID: passed = scale(3, passed); break;
	//	case END: passed = scale(4, passed); break;
	//	default: break;
	//}

	// blocked pawns

	// stacked pawns
	score += stackedPawn(b, WHITE) - stackedPawn(b, BLACK);

	// pawn mobility
	score += pawnChain(b, WHITE), - pawnChain(b, BLACK);

	// rams

	return score;
}

int eval(Board* b) {
	int eval = 0;

	// calculate reused bitboards once and share between functions
	b->attackedSquares[WHITE] = b->attackerSet(WHITE);
	b->attackedSquares[BLACK] = b->attackerSet(BLACK);

	int pawnEval = 0;
	int probe = probePawnEntry(b);
	b->pawnTable->probed++;
	if (probe != NO_SCORE) {
		b->pawnTable->hit++;
		pawnEval = probe;
	} else {
		pawnEval = evaluatePawns(b);
		storePawnEntry(b, pawnEval);
	}

	eval += pawnEval;
	eval += evalBonusMaps(b, WHITE) - evalBonusMaps(b, BLACK);
	eval += materialScore(b, WHITE) - materialScore(b, BLACK);
	eval += openFilesRQ(b, WHITE) - openFilesRQ(b,  BLACK);
	eval += bishopPair(b, WHITE) - bishopPair(b, BLACK);
	eval += kingSafety(b, WHITE) - kingSafety(b, BLACK);
	eval += mobility(b, WHITE) - mobility(b, BLACK);

	// white scores positive and black scores negative
	int sign = (b->side == WHITE) ? 1 : -1;
	return eval * sign;
}

int contemptFactor(Board* b) {
	int contempt = eval(b);
	//int contempt = materialScore(b, WHITE) - materialScore(b, BLACK);

	switch (b->side) {
		case WHITE:
			return (contempt > 100) ? -50 : 0;
			break;
		case BLACK:
			return (contempt < -100) ? 50 : 0;
			break;
		default: ASSERT(0) break;
	}
}


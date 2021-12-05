#include "eval.h"

using namespace chai;

// include extern vars
const int* maps[7];
bitboard_t pawnIsolatedMask[64];
bitboard_t pawnPassedMask[2][64];
bitboard_t upperMask[64];
bitboard_t lowerMask[64];
bitboard_t pawnShield[2][64];
bitboard_t xMask[64];
int manhattenDistance[64][64];

const value_t* PSQT_ENDGAME[7] = {
	{},
	PAWN_ENDGAME,
	KNIGHT_ENDGAME,
	BISHOP_ENDGAME,
	ROOK_ENDGAME,
	QUEEN_ENDGAME,
	KING_ENDGAME
};

const value_t* PSQT_OPENING[7] = {
	{},
	PAWN_OPENING,
	KNIGHT_OPENING,
	BISHOP_OPENING,
	ROOK_OPENING,
	QUEEN_OPENING,
	KING_OPENING
};

// 0 == a / 1 == b
float interpolate(int a, int b, float t) {
	return (float)a + t * ((float)b - (float)a); // TODO this aint right?
}

/**
 * @brief Interpolates between two evaluations.
 * 
 * @param s1 Opening value
 * @param s2 Endgame value
 * @param w1 Weight of opening; implicitly determines w2 = 1 - w1
 * @return float The weighted sum
 */
static float weightedSum(float s1, float s2, float w1) {
	Assert(0 <= w1 && w1 <= 1);
	return (w1 * s1) + ((1 - w1) * s2);
}

/**
 * @brief This coefficient is used to interpolate between opening-endgame evaluations 
 * based on the amount of material left on the board.
 * 
 * @param b 
 * @return Return coefficient value w1
 */
static float interpolCoeff(board_t* b) {
	// 1) Evaluate first 15 moves with opening PSQT
	if (b->halfMoves <= 30) {
		return 1;
	}

	int mat = totalMaterial(b);

	// 2) Material below 20 is endgame
	if (mat <= 20) {
		return 0;
	}
	
	// 3) The less material is left, the more we head into an endgame.
	// => (remaining-material + puffer) / (maximum-material)
	return std::min(1.f, (mat + 10.f) / (maximumMaterial));
}

value_t evaluation(board_t* b) {
	value_t eval = 0;
	float interpolFactor = std::min(1.f, (float)b->halfMoves / (float)(70 + popCount(b->occupied)));

	prefetchPT(b);

	// Check insufficient material to detect drawn positions
	if (popCount(b->occupied) <= 5 && insufficientMaterial(b)) {
		return 0;
	}

	// calculate reused bitboards once and share between functions
	b->attackedSquares[WHITE] = attackerSet(b, WHITE);
	b->attackedSquares[BLACK] = attackerSet(b, BLACK);

	pt->probed++;
	value_t pawnEval = 0;
	bool foundHash = probePT(b, &pawnEval);
	if (foundHash) {
		pt->hit++;
	} else {
		pawnEval = evaluatePawns(b);
		storePT(b, pawnEval);
	}

	// squares controlled
	int centerSquares = (popCount(b->attackedSquares[WHITE] & CENTER_SQUARES) -
						 popCount(b->attackedSquares[BLACK] & CENTER_SQUARES));
	int surroundingSquares = popCount(b->attackedSquares[WHITE] & ~CENTER_SQUARES) -
		popCount(b->attackedSquares[BLACK] & ~CENTER_SQUARES);
	int kingSquares = popCount(b->attackedSquares[WHITE] & kingAtkMask[getKingSquare(b, WHITE)]) -
		popCount(b->attackedSquares[BLACK] & kingAtkMask[getKingSquare(b, BLACK)]);

	eval += surroundingSquares + 2 * centerSquares + 3 * kingSquares;
	eval += pawnEval;

	float w1 = interpolCoeff(b);
	eval += weightedSum(b->psqtOpening, b->psqtEndgame, w1);

	eval += materialScore(b);
	eval += openFilesRQ(b, WHITE) - openFilesRQ(b, BLACK);
	eval += bishopPair(b, WHITE) - bishopPair(b, BLACK);
	eval += kingSafety(b, WHITE, &interpolFactor) - kingSafety(b, BLACK, &interpolFactor);
	eval += mobility(b, WHITE, &interpolFactor) - mobility(b, BLACK, &interpolFactor);

	Assert(abs(eval) < VALUE_IS_MATE_IN);

	// white scores positive and black scores negative
	int sign = (b->stm == WHITE) ? 1 : -1;
	return eval * sign;
}

value_t lazyEvaluation(board_t* b) {
	value_t eval = 0;

	pt->probed++;
	value_t pawnEval = 0;
	bool foundHash = probePT(b, &pawnEval);
	if (foundHash) {
		pt->hit++;
		eval += pawnEval;
	}

	// eval += evalPST(b, WHITE, &interpolFactor) - evalPST(b, BLACK, &interpolFactor);
	float w1 = interpolCoeff(b);
	eval += weightedSum(b->psqtOpening, b->psqtEndgame, w1);

	eval += materialScore(b);

	Assert(abs(eval) < VALUE_IS_MATE_IN);

	// white scores positive and black scores negative
	int sign = (b->stm == WHITE) ? 1 : -1;
	return eval * sign;
}






value_t materialScore(board_t* b) {
	int score = 0;
	for (int i = 1; i < 7; i++) {
		score += popCount(getPieces(b, i, WHITE)) * pieceValues[i];
		score -= popCount(getPieces(b, i, BLACK)) * pieceValues[i];
	}

	return score;
}

value_t openFilesRQ(board_t* b, color_t color) {
	int sq, score = 0;
	bitboard_t pawns = b->pieces[PAWN];

	// openFileBonus for rooks
	bitboard_t rooks = getPieces(b, ROOK, color);
	bitboard_t oppKing = getKingSquare(b, color ^ 1);
	while (rooks) {
		sq = popLSB(&rooks);
		if (!(setMask[squareToFile[sq]] & pawns)) {
			score += openFileBonusR;
		}

		// small bonus for rooks on same file as queen or king
		if (FILE_LIST[squareToFile[sq]] & oppKing) {
			score += 3;
		}
	}

	// openFileBonus for queens
	bitboard_t queens = getPieces(b, QUEEN, color);
	while (queens) {
		sq = popLSB(&queens);
		if (!(setMask[squareToFile[sq]] & pawns)) {
			score += openFileBonusQ;
		}
	}

	return score;
}

value_t bishopPair(board_t* b, color_t color) {
	Assert(((bool)(popCount(getPieces(b, BISHOP, color)) >= 2)) * 30 <= 30);
	return ((bool)(popCount(getPieces(b, BISHOP, color)) >= 2)) * 30;
}

value_t kingSafety(board_t* b, color_t color, float* t) {
	int result = 0;
	int kSq = getKingSquare(b, color);
	bitboard_t pawns = getPieces(b, PAWN, color);

	// punish attacked squares around king 
	result += 8 - popCount(kingAtkMask[kSq] & b->attackedSquares[color ^ 1]);

	// int openFilePenalty = 0; TODO
	// if not endgame
	if (!(popCount(b->occupied) <= 7 || countMajorPieces(b, color) <= 6)) {
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
	result -= popCount(getPinned(b, kSq, color) & ~pawns);

	//scale depending on gamestate
	result = (int)interpolate(result * 2, result, *t);

	int attackedKingSquares = popCount(kingAtkMask[kSq] & b->attackedSquares[color ^ 1]);
	Assert(attackedKingSquares <= 8);

	result -= kingZoneTropism[attackedKingSquares];

	Assert(abs(result) < VALUE_IS_MATE_IN);
	return result;
}

value_t mobility(board_t* b, color_t color, float* t) {
	int mobility = 0;
	int restoreSide = b->stm;

	// how many pieces are attacked by color
	mobility += popCount(b->attackedSquares[color] & b->color[color ^ 1]) / 4;

	// weighted sum of possible moves, reward knight, bishop and rook moves
	moveList_t move_s[1];

	// change color for move generation
	if (b->stm != color) b->stm = color;

	// if (b->stm == WHITE) {
	// 	whiteSinglePawnPush(b, move_s);
	// 	whiteDoublePawnPush(b, move_s);
	// 	whitePawnCaptures(b, move_s);
	// } else {
	// 	blackSinglePawnPush(b, move_s);
	// 	blackDoublePawnPush(b, move_s);
	// 	blackPawnCaptures(b, move_s);
	// }
	// mobility += (int)interpolate(move_s->cnt / 6, move_s->cnt, *t);
	move_s->cnt = 0;

	// int pieceMoves = 0;
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
	b->stm = restoreSide;

	return mobility;
}

int scale(int scaler, int pressure) {
	int scaledPressure;
	switch (scaler) {
		case 0: scaledPressure  = 0; break;
		case 1: scaledPressure  = 1; break;
		case 2: scaledPressure  = pressure; break;
		case 3: scaledPressure  = (pressure * 4) / 3; break;
		case 4: scaledPressure  = (pressure * 3) / 2; break;
		default: scaledPressure = pressure * 2; break;
	}

	return scaledPressure;
}





value_t contemptFactor(board_t* b) {
	value_t contempt = lazyEvaluation(b);

	switch (b->stm) {
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
	if (popCount(b->occupied) > 5) {
		return false;
	}

	// King vs King
	if (popCount(b->occupied) == 2) {
		return true;
	}

	// 3 pieces on board:
	if (popCount(b->occupied) == 3) {
		// King Knight vs King
		if (b->pieces[KNIGHT]) {
			return true;
		}

		// King Bishop vs King
		if (b->pieces[BISHOP]) {
			return true;
		}
	}

	if (popCount(b->occupied) == 4) {
		// King Bishop vs King Bishop (all same color)
		if (popCount(b->pieces[BISHOP] & WHITESQUARES) == 2 &&
			popCount(b->pieces[BISHOP] & BLACKSQUARES) == 2) {
			return true;
		}

	}

	if (popCount(b->occupied) == 5) {
		// King Bishop Bishop vs King Bishop (all same color)
		if (popCount(b->pieces[BISHOP] & WHITESQUARES) == 3 &&
			popCount(b->pieces[BISHOP] & BLACKSQUARES) == 3) {
			return true;
		}

	}

	return false;
}

value_t calcPSQT(board_t* b, const value_t* psqtTable[64]) {
	bitboard_t pieces;
	int sq, sign, value = 0;

	// PAWNS
	pieces = b->pieces[PAWN];
	while (pieces) {
		sq = popLSB(&pieces);

		sign = 1;
		if (b->color[BLACK] & (1ULL << sq)) {
			sq = mirror64[sq];
			sign = -1;
		}
		value += sign * psqtTable[PAWN][sq];
	}

	// KNIGHTS
	pieces = b->pieces[KNIGHT];
	while (pieces) {
		sq = popLSB(&pieces);

		sign = 1;
		if (b->color[BLACK] & (1ULL << sq)) {
			sq = mirror64[sq];
			sign = -1;
		}
		value += sign * psqtTable[KNIGHT][sq];
	}

	pieces = b->pieces[BISHOP];
	while (pieces) {
		sq = popLSB(&pieces);

		sign = 1;
		if (b->color[BLACK] & (1ULL << sq)) {
			sq = mirror64[sq];
			sign = -1;
		}
		value += sign * psqtTable[BISHOP][sq];
	}

	pieces = b->pieces[ROOK];
	while (pieces) {
		sq = popLSB(&pieces);

		sign = 1;
		if (b->color[BLACK] & (1ULL << sq)) {
			sq = mirror64[sq];
			sign = -1;
		}
		value += sign * psqtTable[ROOK][sq];
	}

	pieces = b->pieces[QUEEN];
	while (pieces) {
		sq = popLSB(&pieces);

		sign = 1;
		if (b->color[BLACK] & (1ULL << sq)) {
			sq = mirror64[sq];
			sign = -1;
		}
		value += sign * psqtTable[QUEEN][sq];
	}

	value += psqtTable[KING][getKingSquare(b, WHITE)];
	value -= psqtTable[KING][mirror64[getKingSquare(b, BLACK)]];

	return value;
}
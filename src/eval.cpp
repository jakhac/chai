#include "eval.h"

using namespace chai;

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


tuple_t t(value_t v1, value_t v2) {
	return (v2 << 16) + (v1);
}

value_t t1(tuple_t tuple) {
	return (int16_t)((uint16_t)((unsigned)(tuple)));
}

value_t t2(tuple_t tuple) {
	return (int16_t)((uint16_t)((unsigned)(tuple + 0x8000) >> 16));
	// return (tuple >> 16) & 0x0000FFFF;
}


// Old functions
float interpolate(int a, int b, float t);
value_t openFilesRQ(board_t* b, color_t color);
value_t bishopPair(board_t* b, color_t color);
value_t _kingSafety(board_t* b, color_t color, float* interpol);



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
 * @return Return coefficient value w1 (weight of opening-value)
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

static float gamePhase(board_t* b) {
	int	phase = maxPhase;

	phase -= popCount(b->pieces[PAWN  ]);
	phase -= popCount(b->pieces[KNIGHT]);
	phase -= popCount(b->pieces[BISHOP]);
	phase -= popCount(b->pieces[ROOK  ]);
	phase -= popCount(b->pieces[QUEEN]);

	return (phase * 256 + (maxPhase / 2)) / maxPhase;
}

static value_t scaleGamePhase(tuple_t tuple, float phase) {
	return ((t1(tuple) * (256 - phase)) + (t2(tuple) * phase)) / 256;
}


value_t mixedEvaluation(board_t* b) {

#ifdef USE_NNUE
	// Use NNUE on balanced positions
	if (   canUseNNUE
		&& abs(b->psqtEndgame) < 520) {
		return evaluateNNUE(b);
	}
#endif // USE_NNUE

	prefetchPT(b);

	// NNUE
	accumulateFeatures(b, chai::WHITE);
	accumulateFeatures(b, chai::BLACK);

	accum_t* accDstTest = &b->accum[b->ply];
	assertActiveFeatures(b, chai::WHITE, accDstTest);
	assertActiveFeatures(b, chai::BLACK, accDstTest);

	value_t eval = 0;
	float phase = gamePhase(b);
	// float interpolFactor = std::min(1.f, (float)b->halfMoves / (float)(70 + popCount(b->occupied)));

	// Check insufficient material to detect drawn positions
	if (popCount(b->occupied) <= 5 && insufficientMaterial(b)) {
		return 0;
	}

	// calculate reused bitboards once and share between functions
	b->attackedSquares[WHITE] = attackerSet(b, WHITE);
	b->attackedSquares[BLACK] = attackerSet(b, BLACK);

	tuple_t tupleEval = 0;
	value_t oldEval = 0;

	// Pawn evaluation
	pt->probed++;
	value_t pawnEval = 0;
	if (!probePT(b, &pawnEval)) {
		pawnEval = evaluatePawns(b);
		storePT(b, pawnEval);
	}

	tupleEval += t(pawnEval, pawnEval);
	tupleEval += materialTupleScore(b);
	tupleEval += t(b->psqtOpening, b->psqtEndgame);

	tupleEval += mobility(b, WHITE) - mobility(b, BLACK);

	/****** NEW EVAL ******/

	tupleEval += squareControl(b, WHITE) - squareControl(b, BLACK);

	// // Knight-Bishop placement and coordination
	// tupleEval += evaluateBishops(b, WHITE) - evaluateBishops(b, BLACK);
	tupleEval += evaluateKnights(b, WHITE) - evaluateKnights(b, BLACK);

	// // Rook-Queen
	// tupleEval += evaluateRooks(b, WHITE) - evaluateRooks(b, BLACK);
	// tupleEval += evaluateQueens(b, WHITE) - evaluateQueens(b, BLACK);

	// // // King
	tupleEval += evaluateKing(b, WHITE) - evaluateKing(b, BLACK);


	/****** OLD EVAL ******/

	// squares controlled
	// int centerSquares = (popCount(b->attackedSquares[WHITE] & CENTER_SQUARES) -
	// 					 popCount(b->attackedSquares[BLACK] & CENTER_SQUARES));
	// int surroundingSquares = popCount(b->attackedSquares[WHITE] & ~CENTER_SQUARES) -
	// 	popCount(b->attackedSquares[BLACK] & ~CENTER_SQUARES);
	// int kingSquares = popCount(b->attackedSquares[WHITE] & kingAtkMask[getKingSquare(b, WHITE)]) -
	// 	popCount(b->attackedSquares[BLACK] & kingAtkMask[getKingSquare(b, BLACK)]);
	// oldEval += surroundingSquares + 2 * centerSquares + 3 * kingSquares;

	// oldEval += _kingSafety(b, WHITE, &interpolFactor) - _kingSafety(b, BLACK, &interpolFactor);

	oldEval += openFilesRQ(b, WHITE) - openFilesRQ(b, BLACK);
	oldEval += bishopPair(b, WHITE) - bishopPair(b, BLACK);

	// Combine evals
	value_t newEval = scaleGamePhase(tupleEval, phase);
	eval = newEval + oldEval;

	Assert(abs(eval) < VALUE_IS_MATE_IN);
	return (b->stm == WHITE) ? eval : -eval;
}


value_t evaluation(board_t* b) {
	return mixedEvaluation(b);

	// Prefetch as early as possible
	prefetchPT(b);

	// Check insufficient material to detect drawn positions
	if (popCount(b->occupied) <= 5 && insufficientMaterial(b)) {
		return 0;
	}

	tuple_t eval = 0;
	// float w1 = interpolCoeff(b);
	float phase = gamePhase(b);
	
	// Calculate reused bitboards once and share between eval functions
	b->attackedSquares[WHITE] = attackerSet(b, WHITE);
	b->attackedSquares[BLACK] = attackerSet(b, BLACK);

	// Pawn evaluation
	pt->probed++;
	value_t pawnEval = 0;
	if (!probePT(b, &pawnEval)) {
		pawnEval = evaluatePawns(b);
		storePT(b, pawnEval);
	}

	// Basic evaluation components: Center control, Mobility
	eval += t(pawnEval, pawnEval);
	eval += materialTupleScore(b);
	eval += t(b->psqtOpening, b->psqtEndgame);

	eval += squareControl(b, WHITE) - squareControl(b, BLACK);
	eval += mobility(b, WHITE) - mobility(b, BLACK);

	// Knight-Bishop placement and coordination
	eval += evaluateBishops(b, WHITE) - evaluateBishops(b, BLACK);
	eval += evaluateKnights(b, WHITE) - evaluateKnights(b, BLACK);

	// Rook-Queen
	eval += evaluateRooks(b, WHITE) - evaluateRooks(b, BLACK);
	eval += evaluateQueens(b, WHITE) - evaluateQueens(b, BLACK);

	// King
	eval += evaluateKing(b, WHITE) - evaluateKing(b, BLACK);

	value_t finalEval = ((t1(eval) * (256 - phase)) + (t2(eval) * phase)) / 256;
	Assert(abs(finalEval) < VALUE_IS_MATE_IN);
	return (b->stm == WHITE) ? finalEval : -finalEval;
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

	float w1 = interpolCoeff(b);
	eval += weightedSum(b->psqtOpening, b->psqtEndgame, w1);
	eval += materialScore(b);

	Assert(abs(eval) < VALUE_IS_MATE_IN);

	// white scores positive and black scores negative
	return (b->stm == WHITE) ? eval : -eval;
}

tuple_t mobility(board_t* b, color_t color) {
	value_t score = 0;
	moveList_t moveList[1];

	// How many of our pieces are attacked - might have to move away?
	int piecesAttacked = popCount(b->attackedSquares[color ^ 1] & b->color[color]) / 4;
	score -= t(piecesAttacked, piecesAttacked);

	// Change color for move generation
	color_t tempColor = b->stm;
	if (b->stm != color) b->stm = color;

	// Uniformly weight bishop moves
	moveList->cnt = 0;
	addBishopCaptures(b, moveList);
	addBishopMoves(b, moveList);
	score += BISHOP_MOBILITY * moveList->cnt;

	// Favor knight moves in opening and midgame
	moveList->cnt = 0;
	addKnightMoves(b, moveList);
	addKnightCaptures(b, moveList);
	score += KNIGHT_MOBILITY * moveList->cnt;

	// Favor rook mobility in towards endgame
	moveList->cnt = 0;
	addRookMoves(b, moveList);
	addRookCaptures(b, moveList);
	score += ROOK_MOBILITY * moveList->cnt;
	
	b->stm = tempColor;
	return score;
}

tuple_t squareControl(board_t* b, color_t color) {
	tuple_t tuple = 0;
	int centerExtAttacked   = popCount(b->attackedSquares[color] &  CENTER_SQUARES_EXT);
	int surroundingAttacked = popCount(b->attackedSquares[color] & ~CENTER_SQUARES_EXT);
	int centerOccupied      = popCount(b->color[color] & CENTER_SQUARES);
	// int kingSquaresDefended = popCount(kingAtkMask[getKingSquare(b, color)] & b->attackedSquares[color]);

	tuple += t(3 * centerExtAttacked, centerExtAttacked);
	tuple += t(surroundingAttacked, surroundingAttacked);
	tuple += t(2 * centerOccupied, centerOccupied);
	// tuple += t(2 * kingSquaresDefended, kingSquaresDefended);
	return tuple;
}

tuple_t evaluateBishops(board_t* b, color_t color) {
	int cnt;
	tuple_t score = 0;
	bitboard_t bishops = getPieces(b, BISHOP, color);

	// TODO who favors opposite colored bishops?

	// 1) Bishop Pair
	if (popCount(bishops) > 1) {
		score += BISHOP_PAIR;
	}

	// 2) Bad Bishop
	// - How many blocked pawns on color of bishop?
	bitboard_t blockedPawns = getBlockedPawns(b, color);
	if (bishops & SQUARES_BLACK) {
		cnt = popCount(blockedPawns & SQUARES_BLACK);
		score += BISHOP_COLOR_BLOCKED * cnt;
	}

	if (bishops & SQUARES_WHITE) {
		cnt = popCount(blockedPawns & SQUARES_WHITE);
		score += BISHOP_COLOR_BLOCKED * cnt;
	}



	// 3) Fianchetto Pattern
	value_t fianchetto = 0;
	bitboard_t pawns = getPieces(b, PAWN, color);
	if (color == WHITE) {
		if (popCount(FIANCHETTO_B2 & pawns) > 2 && bishops & B2)
			fianchetto++;

		if (popCount(FIANCHETTO_G2 & pawns) > 2 && bishops & G2)
			fianchetto++;

	} else {
		if (popCount(FIANCHETTO_B7 & pawns) > 2 && bishops & B7)
			fianchetto++;

		if (popCount(FIANCHETTO_G7 & pawns) > 2 && bishops & G7)
			fianchetto++;

	}
	score += fianchetto * BISHOP_FIANCHETTO;

	// 4) Color Weakness
	// - One Bishop left and majority of pawns on different-colored squares
	if (popCount(bishops) == 1) {
		bool whiteSqMajority = 0 < (popCount(pawns & SQUARES_WHITE) - popCount(pawns & SQUARES_BLACK));

		// Only bishop on color non-dominated by pawns
		if (bishops & SQUARES_BLACK && whiteSqMajority) {
			score += BISHOP_STRONG_COLOR_CPLX;
		}
		if (bishops & SQUARES_WHITE && !whiteSqMajority) {
			score += BISHOP_STRONG_COLOR_CPLX;
		}
	}

	// Bishop Outpost
	int sq;
	bitboard_t oppositePawns = getPieces(b, PAWN, color ^ 1);
	while (bishops) {
		sq = popLSB(&bishops);

		// No opposite-colored pawns can challenge this bishop.
		if (   !(pawnPassedMask[color][sq] & oppositePawns)
			&& !((FILE_A_HEX | FILE_H_HEX) & (1 << sq))) {

			score += BISHOP_OUTPOST;

			// Bishop is defended by pawn
			if (pawnAtkMask[color ^ 1][sq] & pawns) {
				score += BISHOP_OUTPOST_DEFENDED;
			}

		}
	}

	return score;
}

tuple_t evaluateKnights(board_t* b, color_t color) {
	tuple_t score            = 0;
	int kSq                  = getKingSquare(b, color);
	int kSqOpp               = getKingSquare(b, color ^ 1);
	bitboard_t knights       = getPieces(b, KNIGHT, color);
	bitboard_t pawns         = getPieces(b, PAWN, color);
	bitboard_t oppositePawns = getPieces(b, PAWN, color ^ 1);
	bitboard_t currentFile;

	// 1) Knights on border of board
	score += popCount(knights & BORDER_SQUARES) * KNIGHT_BORDER_SQUARE;

	int sq;
	bool isOutpost;
	while (knights) {
		sq = popLSB(&knights);

		// 2) Outposts
		currentFile = FILE_LIST[squareToFile[sq]];
		isOutpost = (1ULL << sq) & outpost_squares[color];
		if (   !((pawnPassedMask[color][sq] & ~currentFile) & oppositePawns)
			&& isOutpost) {
			score += KNIGHT_OUTPOST;

			// Outpost square
			currentFile = FILE_LIST[squareToFile[sq]];
			if (pawnAtkMask[color ^ 1][sq] & pawns) {
				score += KNIGHT_OUTPOST_DEFENDED;
			}
		}

		// 3) Center squares attacked
		Assert(b->attackedSquares[color ^ 1] == attackerSet(b, color ^ 1));
		bitboard_t atks = knightAtkMask[sq] & CENTER_SQUARES_EXT;
		score += popCount(atks) * KNIGHT_CENTER_ATTACKS;

		// 4) Distance to kings
		int dist = (manhattenDistance[kSq][sq] + manhattenDistance[kSqOpp][sq]);
		score -= t(0, dist);

		// 5) Shielded by own pawn
		bitboard_t shieldSquare = (color == WHITE) ? (1ULL << (sq+8)) : (1ULL << (sq-8));
		if (shieldSquare & pawns) {
			score += KNIGHT_SHIELDED_BY_PAWN;
		}
	
	}

	// Scale value by remaining pawns 
	if (popCount(b->pieces[PAWN]) < 8) {
		score = t(t1(score) * 0.8, t2(score) * 0.8);
	}

	return score;
}

bool rooksBelowPasser(bitboard_t rooks, bitboard_t passer) {
	int sq;
	while (passer) {
		sq = popLSB(&passer);
		if (rooks & FILE_LIST[squareToFile[sq]] & lowerMask[sq]) {
			return true;
		}

	}
	return false;
}

bool rooksAbovePasser(bitboard_t rooks, bitboard_t passer) {
	int sq;
	while (passer) {
		sq = popLSB(&passer);
		if (rooks & FILE_LIST[squareToFile[sq]] & upperMask[sq]) {
			return true;
		}

	}
	return false;
}

bool rooksConnected(board_t* b, int sq1, int sq2) {
	return (   squareToFile[sq1] == squareToFile[sq2]
			|| squareToRank[sq1] == squareToRank[sq2])
			&& !(inBetween[sq1][sq2] & b->occupied);
}

tuple_t evaluateRooks(board_t* b, color_t color) {
	tuple_t score = 0;
	bitboard_t rooks = getPieces(b, ROOK, color);
	bitboard_t wPassers = getPassers(b, WHITE);
	bitboard_t bPassers = getPassers(b, BLACK);

	// Get rook behind own passers
	if (color == WHITE) {
		if (rooksBelowPasser(rooks, wPassers))
			score += ROOK_CONTROLS_PASSER;

		if (rooksAbovePasser(rooks, bPassers))
			score += ROOK_CONTROLS_PASSER;

	} else {
		if (rooksAbovePasser(rooks, bPassers))
			score += ROOK_CONTROLS_PASSER;
			
		if (rooksBelowPasser(rooks, wPassers))
			score += ROOK_CONTROLS_PASSER;

	}

	int sq = NO_SQ, lastSq, oppKSq = getKingSquare(b, color ^ 1);
	while(rooks) {
		lastSq = sq;
		sq = popLSB(&rooks);

		if (FILE_LIST[squareToFile[sq]] & oppKSq) {
			score += ROOK_SAME_FILE_AS_KING;
		}

		// Rooks on open files
		if (!(b->occupied & FILE_LIST[squareToFile[sq]])) {
			score += ROOK_OPEN_FILE;
		} else if (!(b->pieces[PAWN] & FILE_LIST[squareToFile[sq]])) {
			score += ROOK_SEMI_OPEN_FILE;
		}

		// Rook on 2nd (7th) rank
		if (   (color == WHITE && squareToRank[sq] == RANK_7)
			|| (color == BLACK && squareToRank[sq] == RANK_2)) {
			
			score += ROOK_ON_SEVENTH;
		}

		// Connected rooks
		if (lastSq != NO_SQ && rooksConnected(b, sq, lastSq)) {
			score += ROOK_CONNECTED;
		}
	}

	return score;
}

tuple_t evaluateQueens(board_t* b, color_t color) {
	tuple_t score = 0;
	bitboard_t queens = getPieces(b, QUEEN, color);

	// Penality for early development
	if (   (color == WHITE && pieceAt(b, D1) != Pieces::Q)
		|| (color == BLACK && pieceAt(b, D8) != Pieces::q)) {

		score += QUEEN_EARLY_DEVELOPMENT;
	}

	int sq;
	bitboard_t threats;
	while (queens) {
		sq = popLSB(&queens);

		// Discovered attacks on queen
		threats = getDiscoveredAttacks(b, sq, color);
		score += popCount(threats) * QUEEN_DANGEROUS_SQUARE;

		// Reward activity (central and mobile) of queen in later game phase
		// score += popCount(lookUpQueenMoves(sq, b->occupied) & CENTER_SQUARES_EXT) * QUEEN_CENTER_SIGHT;
	}

	return score;
}

int getAttackerUnits(board_t* b, bitboard_t dZone, color_t color) {
	int sq;
	int attackUnits = 0;

	bitboard_t piece = getPieces(b, KNIGHT, color ^ 1);
	while (piece) {
		sq = popLSB(&piece);
		attackUnits += KNIGHT_UNIT * popCount(knightAtkMask[sq] & dZone);
		// if (knightAtkMask[sq] & dZone)
			// attackUnits += KNIGHT_UNIT;
	}

	piece = getPieces(b, BISHOP, color ^ 1);
	while (piece) {
		sq = popLSB(&piece);
		attackUnits += BISHOP_UNIT * popCount(lookUpBishopMoves(sq, b->occupied) & dZone);
		// if (lookUpBishopMoves(sq, b->occupied) & dZone)
		// 	attackUnits += BISHOP_UNIT;
	}

	piece = getPieces(b, ROOK, color ^ 1);
	while (piece) {
		sq = popLSB(&piece);
		attackUnits += ROOK_UNIT * popCount(lookUpRookMoves(sq, b->occupied) & dZone);
		// if (lookUpRookMoves(sq, b->occupied) & dZone)
			// attackUnits += ROOK_UNIT;
	}

	piece = getPieces(b, QUEEN, color ^ 1);
	while (piece) {
		sq = popLSB(&piece);
		attackUnits += QUEEN_UNIT * popCount(lookUpQueenMoves(sq, b->occupied) & dZone);
		// if ((lookUpBishopMoves(sq, b->occupied) | lookUpRookMoves(sq, b->occupied)) & dZone)
			// attackUnits += QUEEN_UNIT;
	}

	Assert(attackUnits < 100);
	return attackUnits;
}

int undefendedKnightChecks(board_t* b, int kSq, color_t color) {
	Assert(b->attackedSquares[WHITE] == attackerSet(b, WHITE));
	Assert(b->attackedSquares[BLACK] == attackerSet(b, BLACK));
	
	bitboard_t knights = getPieces(b, KNIGHT, color);
	bitboard_t checkingSquares = knightAtkMask[kSq] & ~b->attackedSquares[color ^ 1];

	int sq, res = 0;
	while (knights) {
		sq = popLSB(&knights);
		res += popCount(knightAtkMask[sq] & checkingSquares);
	}

	return res;
}

int undefendedBishopChecks(board_t* b, int kSq, color_t color) {
	Assert(b->attackedSquares[WHITE] == attackerSet(b, WHITE));
	Assert(b->attackedSquares[BLACK] == attackerSet(b, BLACK));
	
	bitboard_t bishops = getPieces(b, BISHOP, color);
	bitboard_t checkingSquares = lookUpBishopMoves(kSq, b->occupied);
	checkingSquares &= ~b->occupied & ~b->attackedSquares[color ^ 1];

	int sq, res = 0;
	while (bishops) {
		sq = popLSB(&bishops);
		res += popCount(lookUpBishopMoves(sq, b->occupied) & checkingSquares);
	}
	return res;
}

int undefendedRookChecks(board_t* b, int kSq, color_t color) {
	Assert(b->attackedSquares[WHITE] == attackerSet(b, WHITE));
	Assert(b->attackedSquares[BLACK] == attackerSet(b, BLACK));

	bitboard_t rooks = getPieces(b, ROOK, color);
	bitboard_t checkingSquares = lookUpRookMoves(kSq, b->occupied);
	checkingSquares &= ~b->occupied & ~b->attackedSquares[color ^ 1];

	int sq, res = 0;
	while (rooks) {
		sq = popLSB(&rooks);
		res += popCount(lookUpRookMoves(sq, b->occupied) & checkingSquares);
	}
	return res;;
}

int undefendedQueenChecks(board_t* b, int kSq, color_t color) {
	Assert(b->attackedSquares[WHITE] == attackerSet(b, WHITE));
	Assert(b->attackedSquares[BLACK] == attackerSet(b, BLACK));
	
	bitboard_t queens = getPieces(b, QUEEN, color);
	bitboard_t checkingSquares = lookUpQueenMoves(kSq, b->occupied);
	checkingSquares &= ~b->occupied & ~b->attackedSquares[color ^ 1];

	int sq, res = 0;
	while (queens) {
		sq = popLSB(&queens);
		res += popCount(lookUpQueenMoves(sq, b->occupied) & checkingSquares);
	}
	return res;
}

/*tuple_t kingSafety(board_t* b, int side) {
	int result = 0;
	int kSq = getKingSquare(b, side);
	bitboard_t pawns = getPieces(b, PAWN, side);

	// count pawn shielder
	result += popCount(pawnShield[side][kSq] & pawns) * 3;

	// punish attacked squares around king 
	result += 8 - popCount(kingAtkMask[kSq] & b->attackedSquares[side ^ 1]);

	// int openFilePenalty = 0; TODO
	// if not endgame
	if (!(popCount(b->occupied) <= 7 || countMajorPieces(b, side) <= 6)) {
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
	result -= popCount(getPinned(b, kSq, side) & ~pawns);

	//scale depending on gamestate

	int attackedKingSquares = popCount(kingAtkMask[kSq] & b->attackedSquares[side ^ 1]);
	Assert(attackedKingSquares <= 8);

	result -= kingZoneTropism[attackedKingSquares];

	Assert(abs(result) < VALUE_IS_MATE_IN);
	return t(result, result);
}*/
 
tuple_t evaluateKing(board_t* b, color_t color) {
	tuple_t score = 0;
	int kSq = getKingSquare(b, color);

	// 1) Reward king activity in endgames
	bitboard_t pawns = getPieces(b, PAWN, color);
	int sq;
	int longestDistToPawn = 0;
	while (pawns) {
		sq = popLSB(&pawns);
		longestDistToPawn = std::max(longestDistToPawn, manhattenDistance[sq][kSq]);
	}
	score += t(0, -longestDistToPawn);

	// 2) Penalty for discovered attacks
	int discoveredAttacks = popCount(getDiscoveredAttacks(b, kSq, color));
	score += discoveredAttacks * KING_DANGEROUS_SQUARE;

	// 3) If at least 3 pieces attack dangerZone of king, count attacker-units and 
	// determine danger-level by lookup in the safety table
	bitboard_t dZone = dangerZone[color][kSq];

	int attackerUnits    = 0;
	int undefendedChecks = 2 * undefendedKnightChecks(b, kSq, color ^ 1)
						 + 2 * undefendedBishopChecks(b, kSq, color ^ 1)
						 + 3 * undefendedRookChecks(b, kSq, color ^ 1)
						 + 3 * undefendedQueenChecks(b, kSq, color ^ 1);

	if (popCount(dZone & b->attackedSquares[color ^ 1]) > 2) {

		attackerUnits = getAttackerUnits(b, dZone, color);
		// int attackedKingSquares = popCount(dangerZone & b->attackedSquares[color ^ 1]);
		// value_t tropism = kingZoneTropism[attackedKingSquares];
		// score -= t(tropism, tropism);

		// 4) Count pieces that can check king from non-attacked square
		// score += undefendedKnightChecks(b, kSq, color ^ 1) * KING_CHECK_UNDEF_KNIGHT;
		// score += undefendedBishopChecks(b, kSq, color ^ 1) * KING_CHECK_UNDEF_BISHOP;
		// score += undefendedRookChecks(b, kSq, color ^ 1) * KING_CHECK_UNDEF_ROOK;
		// score += undefendedQueenChecks(b, kSq, color ^ 1) * KING_CHECK_UNDEF_QUEEN;

	}

	int safetyIndex = std::min(99, undefendedChecks + attackerUnits);
	value_t safetyValue = safetyTable[safetyIndex];
	Assert(safetyValue < VALUE_IS_MATE_IN);
	score -= t(safetyValue, 0.5 * safetyValue);

	// King activity
	if ((1 << kSq) & CENTER_SQUARES_EXT) {
		score += KING_ACTIVITY;
	}

	// Open File next to king
	int kingFile = squareToFile[kSq];
	if (   kingFile != FILE_A
		&& !(pawns & FILE_LIST[kingFile - 1])) {
		score += T_KING_OPEN_NEIHGBOR_FILE;
	}

	if (   kingFile != FILE_H
		&& !(pawns & FILE_LIST[kingFile + 1])) {
		score += T_KING_OPEN_NEIHGBOR_FILE;
	}

	if (!(pawns & FILE_LIST[kingFile])) {
		score += T_KING_OPEN_FILE;
	}


	Assert(abs(t1(score)) < VALUE_IS_MATE_IN);
	Assert(abs(t2(score)) < VALUE_IS_MATE_IN);
	return score;
}

value_t materialScore(board_t* b) {
	int score = 0;
	for (int i = 1; i < 7; i++) {
		score += popCount(getPieces(b, i, WHITE)) * pieceValues[i];
		score -= popCount(getPieces(b, i, BLACK)) * pieceValues[i];
	}

	// Assert (b->material == score); // TODO
	return score;
}

tuple_t materialTupleScore(board_t* b) {
	tuple_t score = 0;
	for (int i = 1; i < 7; i++) {
		score += popCount(getPieces(b, i, WHITE)) * pieceTupleValues[i];
		score -= popCount(getPieces(b, i, BLACK)) * pieceTupleValues[i];
	}

	// Assert (b->material == score); // TODO
	return score;
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
	int occ = popCount(b->occupied);

	// Trivial case, most likely to happen
	if (occ > 5) {
		return false;
	}

	// King vs King
	if (occ == 2) {
		return true;
	}

	// 3 pieces on board:
	if (occ == 3) {
		// King Knight vs King
		if (b->pieces[KNIGHT]) {
			return true;
		}

		// King Bishop vs King
		if (b->pieces[BISHOP]) {
			return true;
		}
	}

	if (occ == 4) {
		// King Bishop vs King Bishop (all same color)
		if (popCount(b->pieces[BISHOP] & SQUARES_WHITE) == 2 &&
			popCount(b->pieces[BISHOP] & SQUARES_BLACK) == 2) {
			return true;
		}

	}

	if (occ == 5) {
		// King Bishop Bishop vs King Bishop (all same color)
		if (popCount(b->pieces[BISHOP] & SQUARES_WHITE) == 3 &&
			popCount(b->pieces[BISHOP] & SQUARES_BLACK) == 3) {
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



////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////


const value_t openFileBonusR = 10;
const value_t openFileBonusQ = 5;

// 0 == a / 1 == b
float interpolate(int a, int b, float t) {
	return (float)a + t * ((float)b - (float)a);
}

// value_t _evaluation(board_t* b) {
// 	value_t eval = 0;
// 	float interpolFactor = std::min(1.f, (float)b->halfMoves / (float)(70 + popCount(b->occupied)));

// 	prefetchPT(b);

// 	// Check insufficient material to detect drawn positions
// 	if (popCount(b->occupied) <= 5 && insufficientMaterial(b)) {
// 		return 0;
// 	}

// 	// calculate reused bitboards once and share between functions
// 	b->attackedSquares[WHITE] = attackerSet(b, WHITE);
// 	b->attackedSquares[BLACK] = attackerSet(b, BLACK);

// 	pt->probed++;
// 	value_t pawnEval = 0;
// 	bool foundHash = probePT(b, &pawnEval);
// 	if (foundHash) {
// 		pt->hit++;
// 	} else {
// 		pawnEval = evaluatePawns(b);
// 		storePT(b, pawnEval);
// 	}

// 	// squares controlled
// 	int centerSquares = (popCount(b->attackedSquares[WHITE] & CENTER_SQUARES) -
// 						 popCount(b->attackedSquares[BLACK] & CENTER_SQUARES));
// 	int surroundingSquares = popCount(b->attackedSquares[WHITE] & ~CENTER_SQUARES) -
// 		popCount(b->attackedSquares[BLACK] & ~CENTER_SQUARES);
// 	int kingSquares = popCount(b->attackedSquares[WHITE] & kingAtkMask[getKingSquare(b, WHITE)]) -
// 		popCount(b->attackedSquares[BLACK] & kingAtkMask[getKingSquare(b, BLACK)]);

// 	eval += surroundingSquares + 2 * centerSquares + 3 * kingSquares;
// 	eval += pawnEval;

// 	float w1 = interpolCoeff(b);
// 	eval += weightedSum(b->psqtOpening, b->psqtEndgame, w1);

// 	eval += materialScore(b);
// 	eval += openFilesRQ(b, WHITE) - openFilesRQ(b, BLACK);
// 	eval += bishopPair(b, WHITE) - bishopPair(b, BLACK);
// 	eval += _kingSafety(b, WHITE, &interpolFactor) - _kingSafety(b, BLACK, &interpolFactor);
// 	eval += t1(mobility(b, WHITE) - mobility(b, BLACK)); // TODO check

// 	Assert(abs(eval) < VALUE_IS_MATE_IN);

// 	// white scores positive and black scores negative
// 	int sign = (b->stm == WHITE) ? 1 : -1;
// 	return eval * sign;
// }

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
	value_t res = ((bool)(popCount(getPieces(b, BISHOP, color)) >= 2)) * 30;
	return res;
}

value_t _kingSafety(board_t* b, color_t color, float* interpol) {
	int result = 0;
	int kSq = getKingSquare(b, color);
	bitboard_t pawns = getPieces(b, PAWN, color);

	// punish attacked squares around king 
	result += 8 - popCount(kingAtkMask[kSq] & b->attackedSquares[color ^ 1]);

	// int openFilePenalty = 0; TODO
	// if not endgame
	// if (!(popCount(b->occupied) <= 7 || countMajorPieces(b, color) <= 6)) {
	// 	int file;
	// 	// punish open king file
	// 	if (!(FILE_LIST[squareToFile[kSq]] & pawns)) {
	// 		result -= 8;
	// 	}

	// 	file = squareToFile[kSq - 1];
	// 	if (fileValid(file) && !(FILE_LIST[file] & pawns)) {
	// 		result -= 5;
	// 	}

	// 	file = squareToFile[kSq + 1];
	// 	if (fileValid(file) && !(FILE_LIST[file] & pawns)) {
	// 		result -= 5;
	// 	}
	// }
	

	// punish pinned pieces (excluding pawns) to kSq
	result -= popCount(getPinned(b, kSq, color) & ~pawns);

	// scale depending on gamestate
	result = (int)interpolate(result * 2, result, *interpol);

	int attackedKingSquares = popCount(kingAtkMask[kSq] & b->attackedSquares[color ^ 1]);
	Assert(attackedKingSquares <= 8);

	result -= kingZoneTropism[attackedKingSquares];

	Assert(abs(result) < VALUE_IS_MATE_IN);
	return result;
}

// tuple_t _mobility(board_t* b, color_t color) {
// 	int mobility = 0;
// 	int restoreSide = b->stm;

// 	// how many pieces are attacked by color
// 	mobility += popCount(b->attackedSquares[color] & b->color[color ^ 1]) / 4;

// 	// weighted sum of possible moves, reward knight, bishop and rook moves
// 	moveList_t move_s[1];
// 	move_s->cnt = 0;

// 	// change color for move generation
// 	if (b->stm != color) b->stm = color;

// 	// int pieceMoves = 0;
// 	addBishopCaptures(b, move_s);
// 	addBishopMoves(b, move_s);
// 	mobility += move_s->cnt;
// 	move_s->cnt = 0;

// 	addKnightMoves(b, move_s);
// 	addKnightCaptures(b, move_s);
// 	mobility += move_s->cnt;
// 	move_s->cnt = 0;

// 	addRookMoves(b, move_s);
// 	addRookCaptures(b, move_s);
// 	mobility += move_s->cnt / 3;
// 	move_s->cnt = 0;
// 	b->stm = restoreSide;

// 	return t(mobility, mobility);
// }

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
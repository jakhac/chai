#include "eval.h"

const value_t* PSQT_ENDGAME[7] = { {},
	PAWN_ENDGAME, KNIGHT_ENDGAME, BISHOP_ENDGAME, 
	ROOK_ENDGAME, QUEEN_ENDGAME,  KING_ENDGAME
};

const value_t* PSQT_OPENING[7] = { {},
	PAWN_OPENING, KNIGHT_OPENING, BISHOP_OPENING, 
	ROOK_OPENING, QUEEN_OPENING,  KING_OPENING
};

const int pPhase   = 0;
const int nPhase   = 1;
const int bPhase   = 1;
const int rPhase   = 2;
const int qPhase   = 4;
const int maxPhase = pPhase*16 + nPhase*4 + bPhase*4 + rPhase*4 + qPhase*2;


const tuple_t KNIGHT_MOBILITY[9] = {
	t(-41, -51), t(-34, -37), t(-15, -28), 
	t( -4, -15), t(  8,  12), t( 14,  16), 
	t( 20,  24), t( 28,  24), t( 37,  20)
};

const tuple_t BISHOP_MOBILITY[14] = {
	t(-57, -89), t(-24, -67), t(  8,  -8),
	t( 17,   8), t( 31,  13), t( 39,  24),
	t( 41,  28), t( 42,  34), t( 43,  39),
	t( 45,  47), t( 55,  58), t( 68,  70),
	t( 89,  88), t( 94,  90)
};

const tuple_t ROOK_MOBILITY[15] = {
	t(-67, -82), t(-34, -82), t(-14, -23),
	t(-10,   3), t( -8,  20), t( -6,  27),
	t( 10,  39), t( 11,  42), t( 13,  52),
	t( 22,  60), t( 36,  75), t( 45,  90),
	t( 55, 100), t( 58, 105), t( 59, 110)
};


const tuple_t BISHOP_PAIR              = t( 30,  69);
const tuple_t BISHOP_OUTPOST_DEFENDED  = t( 17,  10);
const tuple_t BISHOP_LONG_DIAGONAL     = t( 29,   1);

const tuple_t KNIGHT_OUTPOST           = t( 12,  12);
const tuple_t KNIGHT_OUTPOST_DEFENDED  = t( 31,  16);
const tuple_t KNIGHT_BORDER_SQUARE     = t( -7, -14);
const tuple_t KNIGHT_BLOCKS_PAWN       = t(  6,  16);

const tuple_t ROOK_CONTROLS_PASSER     = t(  0,  12);
const tuple_t ROOK_SEMI_OPEN_FILE      = t (10,  8);
const tuple_t ROOK_OPEN_FILE           = t( 30,  0);
const tuple_t ROOK_ON_SEVENTH          = t( -1,  33);
const tuple_t ROOK_CONNECTED           = t(  7,  13);

const tuple_t QUEEN_DANGEROUS_SQUARE   = t( -7,  -7);

const tuple_t KING_DANGEROUS_SQUARE    = t(-15, -25);
const tuple_t KING_CHECK_UNDEF_KNIGHT  = t( -3,  -3);
const tuple_t KING_CHECK_UNDEF_BISHOP  = t( -4,  -4);
const tuple_t KING_CHECK_UNDEF_ROOK    = t( -7,  -7);
const tuple_t KING_CHECK_UNDEF_QUEEN   = t( -6,  -6);
const tuple_t KING_ACTIVITY			   = t(  0,  15);

const tuple_t KING_OPEN_NEIHGBOR_FILE  = t(-15,   0);
const tuple_t KING_OPEN_FILE           = t(-23,   0);


const int UNIT_TABLE[7] = { 0, 0, 3, 3, 4, 6, 0 };

const value_t safetyTable[100] = {
	  0,   0,   1,   2,   3,   5,   7,   9,  12,  15,
	 18,  22,  26,  30,  35,  39,  44,  50,  56,  62,
	 68,  75,  82,  85,  89,  97, 105, 113, 122, 131,
	140, 150, 169, 180, 191, 202, 213, 225, 237, 248,
	260, 272, 283, 295, 307, 319, 330, 342, 354, 366,
	377, 389, 401, 412, 424, 436, 448, 459, 471, 483,
	494, 500, 500, 500, 500, 500, 500, 500, 500, 500,
	500, 500, 500, 500, 500, 500, 500, 500, 500, 500,
	500, 500, 500, 500, 500, 500, 500, 500, 500, 500,
	500, 500, 500, 500, 500, 500, 500, 500, 500, 500
};



/**
 * @brief Return a parameter indicating the phase regarding opening-endgame.
 */
float gamePhase(board_t* b) {
	int	phase = maxPhase;

	phase -= popCount(b->pieces[cPAWN  ]);
	phase -= popCount(b->pieces[cKNIGHT]);
	phase -= popCount(b->pieces[cBISHOP]);
	phase -= popCount(b->pieces[cROOK  ]);
	phase -= popCount(b->pieces[cQUEEN ]);

	return (phase * 256 + (maxPhase / 2)) / maxPhase;
}

/**
 * @brief Returns the weighted sum between opening- and endgame-evaluation.
 */
value_t scaleGamePhase(tuple_t tuple, float phase) {
	return ((t1(tuple) * (256 - phase)) + (t2(tuple) * phase)) / 256;
}

bool rookControlsPasser(bitboard_t rooks, bitboard_t passer, bool above) {

	bitboard_t* mask = (above) ? upperMask : lowerMask;

	while (passer) {
		int sq = popLSB(&passer);
		if (rooks & toFileBB(sq) & mask[sq])
			return true;

	}

	return false;
}

bool rooksConnected(board_t* b, int rookSq1, int rookSq2) {

	bool connected = !(obstructed(rookSq1, rookSq2) & b->occupied);
	bool aligned   = (toFile(rookSq1) == toFile(rookSq2))
				  || (toRank(rookSq1) == toRank(rookSq2));

	return aligned && connected;
}

template<pType_t pType>
int undefendedChecksFrom(board_t* b, int kSq, color_t us) {

	Assert(b->attackedSquares[WHITE] == attackerSet(b, WHITE));
	Assert(b->attackedSquares[BLACK] == attackerSet(b, BLACK));
	
	bitboard_t pieces = getPieces(b, pType, us);
	bitboard_t checkingSquares = getMoveMask<pType>(kSq, b->occupied, us)
							   & ~b->attackedSquares[!us];

	int sq, res = 0;
	while (pieces) {
		sq = popLSB(&pieces);
		res += popCount(getMoveMask<pType>(sq, b->occupied, us) & checkingSquares);
	}
	return res;
}

template<color_t us, pType_t pType>
int getAttackerUnitsFor(board_t* b, bitboard_t dZone) {

	int sq;
	int attackUnits = 0;
	bitboard_t piece, moves;

	piece = getPieces(b, pType, !us);
	while (piece) {
		sq = popLSB(&piece);
		
		moves = getMoveMask<pType>(sq, b->occupied);
		attackUnits += UNIT_TABLE[pType] * popCount(moves & dZone);
	}

	return attackUnits;
}

template<color_t us>
int getAttackerUnits(board_t* b, bitboard_t dZone) {

	int attackUnits = 0;
	attackUnits += getAttackerUnitsFor<us, cKNIGHT>(b, dZone);
	attackUnits += getAttackerUnitsFor<us, cBISHOP>(b, dZone);
	attackUnits += getAttackerUnitsFor<us, cROOK  >(b, dZone);
	attackUnits += getAttackerUnitsFor<us, cQUEEN >(b, dZone);

	Assert(attackUnits < 100);
	return attackUnits;
}

template<color_t us>
static tuple_t mobility(board_t* b) {

	// How many of our pieces are attacked - and might have to be moved?
	int attacked = popCount(b->attackedSquares[!us] & b->color[us]) / 4;

	return -t(attacked, attacked);
}

template<color_t us>
static tuple_t sqControl(board_t* b) {

	tuple_t tuple = 0;
	int centerExtAttacked   = popCount(b->attackedSquares[us] &  MIDDLE_SQUARES);
	int surroundingAttacked = popCount(b->attackedSquares[us] & ~MIDDLE_SQUARES);
	int centerOccupied      = popCount(b->color[us] 		  &  CENTER_SQUARES);

	tuple += t(3 * centerExtAttacked, centerExtAttacked);
	tuple += t(surroundingAttacked, surroundingAttacked);
	tuple += t(2 * centerOccupied, centerOccupied);
	return tuple;
}

value_t materialScore(board_t* b) {

	value_t score = 0;
	for (int i = cPAWN; i <= cKING; i++) {
		score += popCount(getPieces(b, i, WHITE)) * pieceValues[i];
		score -= popCount(getPieces(b, i, BLACK)) * pieceValues[i];
	}

	return score;
}

tuple_t materialTupleScore(board_t* b) {

	tuple_t score = 0;
	for (int i = cPAWN; i <= cKING; i++) {
		score += popCount(getPieces(b, i, WHITE)) * pieceTupleValues[i];
		score -= popCount(getPieces(b, i, BLACK)) * pieceTupleValues[i];
	}

	return score;
}

template<color_t us>
static tuple_t evaluateKnights(board_t* b) {


	tuple_t score = 0;
	color_t them  = !us;

	int kSq                  = getKingSquare(b, us);
	int kSqOpp               = getKingSquare(b, them);
	bitboard_t knights       = getPieces(b, cKNIGHT, us);
	bitboard_t pawns         = getPieces(b, cPAWN, us);
	bitboard_t oppositePawns = getPieces(b, cPAWN, them);
	bitboard_t currentFile;

	int sq;
	bool isOutpostArea;
	while (knights) {
		sq = popLSB(&knights);

		// 1) Outposts
		currentFile = toFileBB(sq);
		isOutpostArea = (1ULL << sq) & outpostSquares[us];
		if (   !((pawnPassedMask[us][sq] & ~currentFile) & oppositePawns)
			&& isOutpostArea) {
			score += KNIGHT_OUTPOST;

			// Defended by pawn
			if (pawnAtkMask[them][sq] & pawns) {
				score += KNIGHT_OUTPOST_DEFENDED;
			}
		}

		// 2) Mobility and Center squares attacked
		int atks = popCount(knightAtkMask[sq]);
		score += KNIGHT_MOBILITY[atks];

		// 3) Distance to kings
		int dist = (manhatten[kSq][sq] + manhatten[kSqOpp][sq]) / 2;
		Assert(dist < 20);
		score -= t(dist, dist);

		// 4) Block opposite pawn
		bitboard_t blockSquare = (us == WHITE) ? (1ULL << (sq+8)) : (1ULL << (sq-8));
		if (blockSquare & oppositePawns)
			score += KNIGHT_BLOCKS_PAWN;

	}

	return score;
}

template<color_t us>
static tuple_t evaluateBishops(board_t* b) {

	tuple_t score = 0;

	bitboard_t bishops  = getPieces(b, cBISHOP, us);
	bitboard_t pawns    = getPieces(b, cPAWN, us);
	bitboard_t oppPawns = getPieces(b, cPAWN, !us);
	bitboard_t atks;

	// 1) Bishop Pair
	if (popCount(bishops) > 1)
		score += BISHOP_PAIR;


	int sq, numMoves = 0;
	while (bishops) {
		sq = popLSB(&bishops);

		numMoves += popCount(  lookUpBishopMoves(sq, b->occupied) 
							& ~b->color[us]);

		// 2) Bishop Outpost
		bitboard_t currentFile = toFileBB(sq);
		bool isOutpostArea = (1ULL << sq) & outpostSquares[us];
		if (   !((pawnPassedMask[us][sq] & ~currentFile) & oppPawns)
			&& isOutpostArea
			&& pawnAtkMask[!us][sq] & pawns) {

			score += BISHOP_OUTPOST_DEFENDED;
		}

		// 3) Long Diagonal
		atks = lookUpBishopMoves(sq, b->occupied);
		if (popCount(atks & CENTER_SQUARES) >= 2) {
			score += BISHOP_LONG_DIAGONAL;
		}

	}

	// 4) Mobility
	score += BISHOP_MOBILITY[numMoves];

	return score;
}

template<color_t us>
static tuple_t evaluateRooks(board_t* b) {

	tuple_t score = 0;
	
	int numMoves = 0;
	int sq = NO_SQ;
	int lastSq;

	bitboard_t rooks = getPieces(b, cROOK, us);
	bitboard_t wPassers = getPassers(b, WHITE);
	bitboard_t bPassers = getPassers(b, BLACK);

	// 1) Get rook behind own passers
	if (rookControlsPasser(rooks, bPassers, true))
		score += ROOK_CONTROLS_PASSER;

	if (rookControlsPasser(rooks, wPassers, false))
		score += ROOK_CONTROLS_PASSER;


	while(rooks) {
		lastSq = sq;
		sq = popLSB(&rooks);

		numMoves += popCount(  lookUpRookMoves(sq, b->occupied) 
							 & ~b->color[us]);


		// 2) Rooks on open files
		bitboard_t file = FILE_LIST[toFile(sq)];

		if (!(file & b->pieces[cPAWN]))
			score += ROOK_SEMI_OPEN_FILE;

		if (!(file & b->occupied))
			score += ROOK_OPEN_FILE;


		// 3) Rook on seventh rank
		if (toRank(sq) == relSeventh<us>())
			score += ROOK_ON_SEVENTH;

		// 4) Connected rooks
		if (lastSq != NO_SQ && rooksConnected(b, sq, lastSq))
			score += ROOK_CONNECTED;
			
	}

	// 5) Mobility
	score += ROOK_MOBILITY[numMoves];

	return score;
}

template<color_t us>
static tuple_t evaluateQueens(board_t* b) {

	tuple_t score = 0;
	bitboard_t queens = getPieces(b, cQUEEN, us);

	int sq;
	bitboard_t threats;
	while (queens) {
		sq = popLSB(&queens);

		// Discovered attacks on queen
		threats = getDiscoveredAttacks(b, sq, us);
		score  += popCount(threats) * QUEEN_DANGEROUS_SQUARE;
	}

	return score;
}

template<color_t us>
tuple_t evaluateKing(board_t* b) {

	color_t them    = !us;
	tuple_t score = 0;
	int kSq = getKingSquare(b, us);

	// 1) Reward king activity in endgames
	bitboard_t pawns = getPieces(b, cPAWN, us);
	int sq;
	int longestDistToPawn = 0;

	while (pawns) {
		sq = popLSB(&pawns);
		longestDistToPawn = std::max(longestDistToPawn, manhatten[sq][kSq]);
	}

	score += t(0, -longestDistToPawn);

	// 2) Penalty for discovered attacks
	int discoveredAttacks = popCount(getDiscoveredAttacks(b, kSq, us));
	score += discoveredAttacks * KING_DANGEROUS_SQUARE;

	// 3) If at least 3 pieces attack dangerZone of king, count attacker-units and 
	// determine danger-level by lookup in the safety table
	int undefendedChecks = 2 * undefendedChecksFrom<cKNIGHT>(b, kSq, them)
						 + 2 * undefendedChecksFrom<cBISHOP>(b, kSq, them)
						 + 3 * undefendedChecksFrom<cROOK>  (b, kSq, them)
						 + 3 * undefendedChecksFrom<cQUEEN> (b, kSq, them);

	bitboard_t dZone  = dangerZone[us][kSq];
	int attackerUnits = 0;

	if (popCount(dZone & b->attackedSquares[them]) > 2)
		attackerUnits = getAttackerUnits<us>(b, dZone);


	int safetyIndex = std::min(99, undefendedChecks + attackerUnits);
	value_t safetyValue = safetyTable[safetyIndex];
	score -= t(safetyValue, 0.5 * safetyValue);

	// 4) King activity in endgames
	if ((1 << kSq) & MIDDLE_SQUARES)
		score += KING_ACTIVITY;


	// 5) Punish open files around king
	int kingFile = toFile(kSq);

	if (   (kingFile != FILE_A && !(pawns & FILE_LIST[kingFile - 1]))
		|| (kingFile != FILE_H && !(pawns & FILE_LIST[kingFile + 1])))
		score += KING_OPEN_NEIHGBOR_FILE;

	if (!(pawns & FILE_LIST[kingFile]))
		score += KING_OPEN_FILE;

	return score;
}

value_t evaluation(board_t* b) {

	TT::prefetchPT(b);

	// Check for known endgames
	if (popCount(b->occupied) <= 5) {

		if (insufficientMaterial(b))
			return 0;

		if (isKvKNB(b))
 			return evaluate_KvKNB(b);

		if (isKQvKR(b))
 			return evaluate_KQvKR(b);

		if (isKQvKP(b))
			return evaluate_KQvKP(b);

		if (isKQvKQ(b))
			return evaluate_KQvKQ(b);

	}

#ifdef USE_NNUE

	// Use NNUE on balanced positions
	if (   canUseNNUE
		&& abs(b->psqtEndgame) < 520) {
		return NNUE::evaluateNNUE(b);
	}

#endif // USE_NNUE

	float phase  = gamePhase(b);
	tuple_t eval = 0;

	// Calculate these bitboards once and share among eval-functions
	b->attackedSquares[WHITE] = attackerSet(b, WHITE);
	b->attackedSquares[BLACK] = attackerSet(b, BLACK);

	// Pawn evaluation
	pt->probed++;
	value_t pawnEval = 0;
	if (!TT::probePT(b, &pawnEval)) {
		pawnEval = evaluatePawns(b);
		TT::storePT(b, pawnEval);
	}

	eval += t(pawnEval, pawnEval);
	eval += t(b->psqtOpening, b->psqtEndgame);

	eval += sqControl<WHITE>(b) - sqControl<BLACK>(b);
	eval += mobility<WHITE>(b)  - mobility<BLACK>(b);
	eval += materialTupleScore(b);
	
	eval += evaluateBishops<WHITE>(b) - evaluateBishops<BLACK>(b);
	eval += evaluateKnights<WHITE>(b) - evaluateKnights<BLACK>(b);
	eval += evaluateRooks<WHITE>(b)   - evaluateRooks<BLACK>(b);
	eval += evaluateQueens<WHITE>(b)  - evaluateQueens<BLACK>(b);
	eval += evaluateKing<WHITE>(b)    - evaluateKing<BLACK>(b);

	eval = scaleGamePhase(eval, phase);

	Assert(abs(eval) < VALUE_IS_MATE_IN);
	return (b->stm == WHITE) ? eval : -eval;
}

bool insufficientMaterial(board_t* b) {

	int occ = popCount(b->occupied);

	// Rule out most frequent case first
	if (occ > 5) return false;

	// KvK
	if (occ == 2) return true;

	if (occ == 3) {
		// KvKN
		if (b->pieces[cKNIGHT]) return true;

		// KvKB
		if (b->pieces[cBISHOP]) return true;
	}

	if (occ == 4) {
		// KBvKB (all same color)
		if (   popCount(b->pieces[cBISHOP] & SQUARES_WHITE) == 2
		    && popCount(b->pieces[cBISHOP] & SQUARES_BLACK) == 2)
			return true;

	}

	if (occ == 5) {
		// KBBvKB (all same color)
		if (   popCount(b->pieces[cBISHOP] & SQUARES_WHITE) == 3 
			&& popCount(b->pieces[cBISHOP] & SQUARES_BLACK) == 3)
			return true;

	}

	return false;
}

template<pType_t piece>
value_t calcPSQTFor(board_t* b, const value_t* psqtTable[64]) {

	int sq, sign;
	color_t col;
	value_t value = 0;
	bitboard_t pieces = b->pieces[piece];

	while (pieces) {
		sq   = popLSB(&pieces);
		col  = colorAt(b, sq);
		sq   = relSquare(sq, col);
		sign = (col == WHITE) ? 1 : -1;

		value += sign * psqtTable[piece][sq];
	}

	return value;
}

value_t calcPSQT(board_t* b, const value_t* psqtTable[64]) {

	value_t value = 0;
	value += calcPSQTFor<cPAWN>(b, psqtTable);
	value += calcPSQTFor<cKNIGHT>(b, psqtTable);
	value += calcPSQTFor<cBISHOP>(b, psqtTable);
	value += calcPSQTFor<cROOK>(b, psqtTable);
	value += calcPSQTFor<cQUEEN>(b, psqtTable);
	value += calcPSQTFor<cKING>(b, psqtTable);

	return value;
}

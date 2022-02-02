#include "pawn.h"


Value evaluatePawns(Board* b) {

	Value score = 0;
	int wCount, bCount;
	Value wValue, bValue;

	// 1) Negative pawn structure attributes (subtract value)
	// Lack of pawns
	if (!getPieces(b, PAWN, WHITE)) {
		score += +PAWN_LACK_PENALTY;
	}
	if (!getPieces(b, PAWN, BLACK)) {
		score += -PAWN_LACK_PENALTY;
	}

	// Backward pawns
	wCount = popCount(wBackwardPawns(b));
	bCount = popCount(bBackwardPawns(b));
	score += BACKWARD_PAWN_PENALTY * (wCount - bCount);

	// Doubled Pawns
	wCount = doubledPawns<WHITE>(b);
	bCount = doubledPawns<BLACK>(b);
	score += DOUBLED_PAWN_PENALTY * (wCount - bCount);

	// Pawn Islands
	wCount = pawnIslands<WHITE>(b);
	bCount = pawnIslands<BLACK>(b);
	score += PAWN_ISLAND_PENALTY * (wCount - bCount);

	// Isolated Pawns
	wCount = isolatedPawns<WHITE>(b);
	bCount = isolatedPawns<BLACK>(b);
	score += PAWN_ISOLATION_PENALTY * (wCount - bCount);

	// 2) Positive pawn structure attributes (add value)
	// Passed pawns
	wValue = passedPawns<WHITE>(b);
	bValue = passedPawns<BLACK>(b);
	score += (wValue - bValue);

	// Hidden passed pawns (2v1 situation)
	wCount = hiddenPassedPawns<WHITE>(b);
	bCount = hiddenPassedPawns<BLACK>(b);
	score += HIDDE_PASSED_PAWN_REWARD * (wCount - bCount);

	// Pawn Chains
	wCount = pawnChain<WHITE>(b);
	bCount = pawnChain<BLACK>(b);
	score += PAWN_CHAIN_REWARD * (wCount - bCount);

	// 3) Evaluate king-safety with respect to pawn structure
	wValue = kingPawnSafety<WHITE>(b);
	bValue = kingPawnSafety<BLACK>(b);
	score += wValue - bValue;

	Assert(abs(score) < VALUE_IS_MATE_IN);
	return score;
}

template<Color color>
Value kingPawnSafety(Board* b) {

	int kSq = getKingSquare(b, color);

	Value score    = 0;
	Bitboard pawns = getPieces(b, PAWN, color);

	// Pawns shielding king
	score += popCount(pawnShield[color][kSq] & pawns) * PAWN_SHIELD_REWARD;

	return score;
}

Bitboard wBackwardPawns(Board* b) {

	int sq;
	Bitboard file;
	Bitboard wPawns = getPieces(b, PAWN, WHITE);
	Bitboard bPawns = getPieces(b, PAWN, BLACK);
	Bitboard res    = 0;

	Bitboard potentialBackwards = wPawns & (RANK_2_HEX);
	while (potentialBackwards) {
		sq   = popLSB(&potentialBackwards);
		file = toFileBB(sq);

		if (   !(file & bPawns)							// 2) Semi-open file
			&& pawnAtkMask[WHITE][sq+8] & bPawns		// 3) Forward square is controlled
			&& !(horizontalNeighbors[sq] & wPawns)) {	// 4) No neighbors

			// Found a backwards pawn
			res |= 1ULL << sq;
		}
	}

	return res;
}

Bitboard bBackwardPawns(Board* b) {

	int sq;
	Bitboard file;
	Bitboard bPawns = getPieces(b, PAWN, BLACK);
	Bitboard wPawns = getPieces(b, PAWN, WHITE);
	Bitboard res    = 0;

	Bitboard potentialBackwards = bPawns & (RANK_7_HEX);
	while (potentialBackwards) {
		sq   = popLSB(&potentialBackwards);
		file = toFileBB(sq);

		if (   !(file & wPawns)							// 2) Semi-open file
			&& pawnAtkMask[BLACK][sq+8] & wPawns		// 3) Forward square is controlled
			&& !(horizontalNeighbors[sq] & bPawns)) {	// 4) No neighbors

			// Found a backwards pawn
			res |= 1ULL << sq;
		}
	}

	return res;
}

template<Color color>
int doubledPawns(Board* b) {

	int doubled = 0;
	Bitboard pawns = getPieces(b, PAWN, color);

	for (int i = 0; i < 8; i++) {
		int pawnsOneFile = popCount(FILE_LIST[i] & pawns);
		if (pawnsOneFile > 1) {
			doubled += pawnsOneFile - 1; // Count "each" doubled pawn
		}
	}

	return doubled;
}

template<Color color>
int pawnIslands(Board* b) {

	int islands = 0;
	Bitboard pawns = getPieces(b, PAWN, color);

	int i = 0;
	while (i < 8) {

		// Eat up empty files
		while (i < 8 && !(FILE_LIST[i] & pawns)) i++;

		if (FILE_LIST[i] & pawns) {
			islands++;

			// Eat up pawns on current island
			while (i < 8 && FILE_LIST[i] & pawns) i++;
		}
	}

	return islands;
}

template<Color color>
int isolatedPawns(Board* b) {

	int isolated = 0, sq;
	Bitboard pawns = getPieces(b, PAWN, color);
	Bitboard refPawns = pawns;

	while (pawns) {
		sq = popLSB(&pawns);

		// Count isolated pawns. Exclude IQP.
		if (   !(pawnIsolatedMask[sq] & refPawns)
			&& toFile(sq) != FILE_D) {

			isolated++;
		}
	}

	return isolated;
}

template<Color color>
Value passedPawns(Board* b) {

	int passerValue = 0, sq;
	Bitboard pawns = getPieces(b, PAWN, color);
	Bitboard defenders = getPieces(b, PAWN, !color);

	while (pawns) {
		sq = popLSB(&pawns);
		if (!(pawnPassedMask[color][sq] & defenders)) {
			passerValue += PASSED_PAWN_REWARD[color][toRank(sq)];
		}
	}

	return passerValue;
}

template<Color color>
int hiddenPassedPawns(Board* b) {

	int sq, hiddenPassed = 0;
	Bitboard pawns     = getPieces(b, PAWN, color);
	Bitboard refPawns  = pawns;
	Bitboard defenders = getPieces(b, PAWN, !color);

	while (pawns) {
		sq = popLSB(&pawns);

		// only one defender
		if (popCount(pawnPassedMask[color][sq] & defenders) == 1) {

			// Is the current pawn supported 
			Bitboard support = pawnAtkMask[color^1][sq] & refPawns;
			if (support) {

				int supportSq;
				while (support) {
					supportSq = popLSB(&support);

					// Is the supporter still controlled?
					Bitboard mask = pawnPassedMask[color][supportSq];
					mask &= ~pawnPassedMask[color][sq];
					if (!(mask & defenders)) {
						hiddenPassed++;
						break;
					}
				}
			}
		}
	}

	return hiddenPassed;
}

template<Color color>
Value pawnChain(Board* b) {

	int sq;
	int chains = 0;

	Bitboard pawns  = getPieces(b, PAWN, color);
	Bitboard tPawns = pawns;

	while (pawns) {
		sq = popLSB(&pawns);

		if (tPawns & xMask[sq]) {
			chains++;
		}
	}

	return chains;
}

int nonPawnPieces(Board* b, Color color) {

	Bitboard pieces = getPieces(b, BISHOP, color)
					| getPieces(b, KNIGHT, color)
					| getPieces(b, ROOK, color)
					| getPieces(b, QUEEN, color);

	return popCount(pieces);
}

bool nonPawnPieces(Board* b) {
	return b->occupied & ~b->pieces[PAWN] & ~b->pieces[KING];
}


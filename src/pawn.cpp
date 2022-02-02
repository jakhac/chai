#include "pawn.h"


value_t evaluatePawns(board_t* b) {

	value_t score = 0;
	int wCount, bCount;
	value_t wValue, bValue;

	// 1) Negative pawn structure attributes (subtract value)
	// Lack of pawns
	if (!getPieces(b, cPAWN, WHITE)) {
		score += +PAWN_LACK_PENALTY;
	}
	if (!getPieces(b, cPAWN, BLACK)) {
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

template<color_t color>
value_t kingPawnSafety(board_t* b) {

	int kSq = getKingSquare(b, color);

	value_t score    = 0;
	bitboard_t pawns = getPieces(b, cPAWN, color);

	// Pawns shielding king
	score += popCount(pawnShield[color][kSq] & pawns) * PAWN_SHIELD_REWARD;

	return score;
}

bitboard_t wBackwardPawns(board_t* b) {

	int sq;
	bitboard_t file;
	bitboard_t wPawns = getPieces(b, cPAWN, WHITE);
	bitboard_t bPawns = getPieces(b, cPAWN, BLACK);
	bitboard_t res    = 0;

	bitboard_t potentialBackwards = wPawns & (RANK_2_HEX);
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

bitboard_t bBackwardPawns(board_t* b) {

	int sq;
	bitboard_t file;
	bitboard_t bPawns = getPieces(b, cPAWN, BLACK);
	bitboard_t wPawns = getPieces(b, cPAWN, WHITE);
	bitboard_t res    = 0;

	bitboard_t potentialBackwards = bPawns & (RANK_7_HEX);
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

template<color_t color>
int doubledPawns(board_t* b) {

	int doubled = 0;
	bitboard_t pawns = getPieces(b, cPAWN, color);

	for (int i = 0; i < 8; i++) {
		int pawnsOneFile = popCount(FILE_LIST[i] & pawns);
		if (pawnsOneFile > 1) {
			doubled += pawnsOneFile - 1; // Count "each" doubled pawn
		}
	}

	return doubled;
}

template<color_t color>
int pawnIslands(board_t* b) {

	int islands = 0;
	bitboard_t pawns = getPieces(b, cPAWN, color);

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

template<color_t color>
int isolatedPawns(board_t* b) {

	int isolated = 0, sq;
	bitboard_t pawns = getPieces(b, cPAWN, color);
	bitboard_t refPawns = pawns;

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

template<color_t color>
value_t passedPawns(board_t* b) {

	int passerValue = 0, sq;
	bitboard_t pawns = getPieces(b, cPAWN, color);
	bitboard_t defenders = getPieces(b, cPAWN, !color);

	while (pawns) {
		sq = popLSB(&pawns);
		if (!(pawnPassedMask[color][sq] & defenders)) {
			passerValue += PASSED_PAWN_REWARD[color][toRank(sq)];
		}
	}

	return passerValue;
}

template<color_t color>
int hiddenPassedPawns(board_t* b) {

	int sq, hiddenPassed = 0;
	bitboard_t pawns     = getPieces(b, cPAWN, color);
	bitboard_t refPawns  = pawns;
	bitboard_t defenders = getPieces(b, cPAWN, !color);

	while (pawns) {
		sq = popLSB(&pawns);

		// only one defender
		if (popCount(pawnPassedMask[color][sq] & defenders) == 1) {

			// Is the current pawn supported 
			bitboard_t support = pawnAtkMask[color^1][sq] & refPawns;
			if (support) {

				int supportSq;
				while (support) {
					supportSq = popLSB(&support);

					// Is the supporter still controlled?
					bitboard_t mask = pawnPassedMask[color][supportSq];
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

template<color_t color>
value_t pawnChain(board_t* b) {

	int sq;
	int chains = 0;

	bitboard_t pawns  = getPieces(b, cPAWN, color);
	bitboard_t tPawns = pawns;

	while (pawns) {
		sq = popLSB(&pawns);

		if (tPawns & xMask[sq]) {
			chains++;
		}
	}

	return chains;
}

int nonPawnPieces(board_t* b, color_t color) {

	bitboard_t pieces = getPieces(b, cBISHOP, color)
		| getPieces(b, cKNIGHT, color)
		| getPieces(b, cROOK, color)
		| getPieces(b, cQUEEN, color);

	return popCount(pieces);
}

bool nonPawnPieces(board_t* b) {
	return b->occupied & ~b->pieces[cPAWN] & ~b->pieces[cKING];
}


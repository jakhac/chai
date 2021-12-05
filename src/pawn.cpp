#include "pawn.h"

namespace chai {

value_t evaluatePawns(board_t* b) {
	value_t score = 0;
	int wCount, bCount;
	value_t wValue, bValue;

	// 1) Negative pawn structure attributes (subtract value)
	// Lack of pawns
	if (!getPieces(b, PAWN, WHITE)) score += PAWN_LACK_PENALTY;
	if (!getPieces(b, PAWN, BLACK)) score += PAWN_LACK_PENALTY;

	// Backward pawns
	wCount = popCount(wBackwardPawns(b));
	bCount = popCount(bBackwardPawns(b));
	score += BACKWARD_PAWN_PENALTY * (wCount - bCount);

	// Doubled Pawns
	wCount = doubledPawns(b, WHITE);
	bCount = doubledPawns(b, BLACK);
	score += DOUBLED_PAWN_PENALTY * (wCount - bCount);

	// Pawn Islands
	wCount = pawnIslands(b, WHITE);
	bCount = pawnIslands(b, BLACK);
	score += PAWN_ISLAND_PENALTY * (wCount - bCount);

	// Isolated Pawns
	wCount = isolatedPawns(b, WHITE);
	bCount = isolatedPawns(b, BLACK);
	score += PAWN_ISOLATION_PENALTY * (wCount - bCount);


	// 2) Positive pawn structure attributes (add value)
	// Passed pawns
	wValue = passedPawns(b, WHITE);
	bValue = passedPawns(b, BLACK);
	score += (wValue - bValue);

	// Hidden passed pawns (2v1 situation)
	wCount = hiddenPassedPawns(b, WHITE);
	bCount = hiddenPassedPawns(b, BLACK);
	score += HIDDE_PASSED_PAWN_REWARD * (wCount - bCount);

	// Pawn Chains
	wCount = pawnChain(b, WHITE);
	bCount = pawnChain(b, BLACK);
	score += PAWN_CHAIN_REWARD * (wCount - bCount);

	// 3) Evaluate king-safety with respect to pawn structure
	wValue = kingPawnSafety(b, WHITE);
	bValue = kingPawnSafety(b, BLACK);
	score += wValue - bValue;

	Assert(abs(score) < VALUE_IS_MATE_IN);
	return score;
}

value_t kingPawnSafety(board_t* b, color_t color) {
	value_t score = 0;
	int kSq = getKingSquare(b, color);
	bitboard_t pawns = getPieces(b, PAWN, color);
	
	// Open File next to king
	score += openFilesNearKing(b, kSq, color);

	// Pawns shielding king
	score += popCount(pawnShield[color][kSq] & pawns) * PAWN_SHIELD_REWARD;

	return score;
}

int openFilesNearKing(board_t* b, int kSq, color_t color) {
	int openCount = 0;
	int kingFile = squareToFile[kSq];
	bitboard_t pawns = getPieces(b, PAWN, color);

	if (   kingFile != FILE_A
		&& !(pawns & FILE_LIST[kingFile - 1])) {
		openCount++;
	}

	if (   kingFile != FILE_H
		&& !(pawns & FILE_LIST[kingFile + 1])) {
		openCount++;
	}

	if (!(pawns & FILE_LIST[kingFile])) {
		openCount++;
	}

	return openCount;
}


bitboard_t wBackwardPawns(board_t* b) {
	int sq;
	bitboard_t file;
	bitboard_t wPawns = getPieces(b, PAWN, WHITE);
	bitboard_t bPawns = getPieces(b, PAWN, BLACK);
	bitboard_t potentialBackwards = wPawns & (RANK_2_HEX);
	bitboard_t res = 0ULL;

	while (potentialBackwards) {
		sq = popLSB(&potentialBackwards);
		file = FILE_LIST[squareToFile[sq]];

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
	bitboard_t bPawns = getPieces(b, PAWN, BLACK);
	bitboard_t wPawns = getPieces(b, PAWN, WHITE);
	bitboard_t potentialBackwards = bPawns & (RANK_7_HEX);
	bitboard_t res = 0ULL;

	while (potentialBackwards) {
		sq = popLSB(&potentialBackwards);
		file = FILE_LIST[squareToFile[sq]];

		if (   !(file & wPawns)							// 2) Semi-open file
			&& pawnAtkMask[BLACK][sq+8] & wPawns		// 3) Forward square is controlled
			&& !(horizontalNeighbors[sq] & bPawns)) {	// 4) No neighbors

			// Found a backwards pawn
			res |= 1ULL << sq;
		}
	}

	return res;
}

int doubledPawns(board_t* b, color_t color) {
	int doubled = 0;

	bitboard_t pawns = getPieces(b, PAWN, color);
	for (int i = 0; i < 8; i++) {
		int pawnsOneFile = popCount(FILE_LIST[i] & pawns);
		if (pawnsOneFile > 1) {
			doubled += pawnsOneFile - 1; // Count "each" doubled pawn
		}
	}

	return doubled;
}

int pawnIslands(board_t* b, color_t color) {
	int islands = 0;

	bitboard_t pawns = getPieces(b, PAWN, color);

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

int isolatedPawns(board_t* b, color_t color) {
	int isolated = 0, sq;
	bitboard_t pawns = getPieces(b, PAWN, color);
	bitboard_t refPawns = pawns;

	while (pawns) {
		sq = popLSB(&pawns);
		if (   !(pawnIsolatedMask[sq] & refPawns)
			&& squareToFile[sq] != FILE_D) {

			// Count isolated pawns. Exclude IQP.
			isolated++;
		}
	}

	return isolated;
}

value_t passedPawns(board_t* b, color_t color) {
	int passerValue = 0, sq;
	bitboard_t pawns = getPieces(b, PAWN, color);
	bitboard_t defenders = getPieces(b, PAWN, color ^ 1);

	while (pawns) {
		sq = popLSB(&pawns);
		if (!(pawnPassedMask[color][sq] & defenders)) {
			passerValue += PASSED_PAWN_REWARD[color][squareToRank[sq]];
		}
	}

	return passerValue;
}

int hiddenPassedPawns(board_t* b, color_t color) {
	int hiddenPassed = 0, sq;
	bitboard_t pawns = getPieces(b, PAWN, color);
	bitboard_t refPawns = pawns;
	bitboard_t defenders = getPieces(b, PAWN, color ^ 1);

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

value_t pawnChain(board_t* b, color_t color) {
	int chains = 0;
	bitboard_t pawns = getPieces(b, PAWN, color);
	bitboard_t tPawns = pawns;

	int sq;
	while (pawns) {
		sq = popLSB(&pawns);

		if (tPawns & xMask[sq]) {
			chains++;
		}
	}

	return chains;
}

int nonPawnPieces(board_t* b, color_t color) {
	bitboard_t pieces = getPieces(b, chai::BISHOP, color)
		| getPieces(b, chai::KNIGHT, color)
		| getPieces(b, chai::ROOK, color)
		| getPieces(b, chai::QUEEN, color);

	return popCount(pieces);
}

bool nonPawnPieces(board_t* b) {
	return b->occupied & ~b->pieces[chai::PAWN] & ~b->pieces[chai::KING];
}



}

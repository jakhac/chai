#include "eval.h"

// include extern vars
const int* maps[7];

void initEval() {
	maps[1] = PAWN_BONUS;
	maps[2] = KNIGHT_BONUS;
	maps[3] = BISHOP_BONUS;
	maps[4] = ROOK_BONUS;
	maps[5] = QUEEN_BONUS;
	maps[6] = KING_BONUS;
}

int evalBonusMaps(Board* b, int side) {
	U64 pieces;
	int score = 0, sq;

	for (int i = 1; i < 7; i++) {
		pieces = b->getPieces(i, side);
		while (pieces) {
			sq = popBit(&pieces);
			score += maps[i][sq];
		}
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

int eval(Board* b) {
	int eval = 0;

	eval += evalBonusMaps(b, WHITE) - evalBonusMaps(b, BLACK);

	eval += materialScore(b, WHITE) -  materialScore(b, BLACK);

	// white scores positive and black scores negative
	int sign = (b->side == WHITE) ? 1 : -1;
	return eval * sign;
}
#include "moveOrdering.h"

static void updateBestMove(int* scores, int* bestIdx, int curIdx) {
	if (scores[curIdx] > scores[*bestIdx]) {
		*bestIdx = curIdx;
	}
}

void scoreMoves(Board* b, moveList_t* moveList, move_t* hashMove) {
	move_t currentMove = NO_MOVE;
	int seeScore, mvvLvaScore;
	int bestIdx = 0;

	for (int i = 0; i < moveList->cnt; i++) {
		currentMove = moveList->moves[i];

		// hash move
		if (currentMove == *hashMove) {
			moveList->scores[i] = HASH_MOVE;

			updateBestMove(moveList->scores, &bestIdx, i);
			continue;
		}

		// enpas move
		if (currentMove & MCHECK_EP) {
			moveList->scores[moveList->cnt] = GOOD_CAPTURE + 105;

			updateBestMove(moveList->scores, &bestIdx, i);
			continue;
		}

		// capture moves
		if (currentMove & MCHECK_CAP) {

			if (currentMove & MCHECK_PROM) {
				// Promoting Pawns
				moveList->scores[i] = PROMOTING_CAPTURE + b->pieceAt(fromSq(currentMove));
			} else {
				// Plain Captures
				seeScore = see(b, currentMove);
				mvvLvaScore = MVV_LVA[capPiece(currentMove)][b->pieceAt(fromSq(currentMove))];
				ASSERT(mvvLvaScore > 0);

				if (seeScore > 0) {
					moveList->scores[i] = GOOD_CAPTURE + mvvLvaScore;
				} else if (seeScore == 0) {
					moveList->scores[i] = EQUAL_CAPTURE + mvvLvaScore;
				} else {
					moveList->scores[i] = mvvLvaScore;
				}
			}

			updateBestMove(moveList->scores, &bestIdx, i);
			continue;
		}

		// Only quiet moves left:

		// first killer
		if (currentMove == b->killer[0][b->ply]) {
			moveList->scores[i] = KILLER_SCORE_1;

			updateBestMove(moveList->scores, &bestIdx, i);
			continue;
		}

		// second killer
		if (currentMove == b->killer[1][b->ply]) {
			moveList->scores[i] = KILLER_SCORE_2;

			updateBestMove(moveList->scores, &bestIdx, i);
			continue;
		}

		// mate killer
		if (currentMove == b->mateKiller[b->ply]) {
			moveList->scores[i] = MATE_KILLER;

			updateBestMove(moveList->scores, &bestIdx, i);
			continue;
		}

		// counter move
		if (b->ply > 0) {
			move_t prevMove = b->undoHistory[b->ply - 1].move;
			move_t counterMove = b->counterHeuristic[fromSq(prevMove)][toSq(prevMove)][b->side];
			if (currentMove == counterMove) {
				moveList->scores[moveList->cnt] = COUNTER_SCORE;
			}

			updateBestMove(moveList->scores, &bestIdx, i);
			continue;
		}

		// castle move
		if (currentMove & MCHECK_CAS) {
			moveList->scores[moveList->cnt] = QUIET_SCORE + CASTLE_SCORE;

			updateBestMove(moveList->scores, &bestIdx, i);
			continue;
		}

		// last resort: history heuristic
		int histScore = b->histHeuristic[b->pieceAt(fromSq(currentMove))][toSq(currentMove)];
		moveList->scores[i] = QUIET_SCORE + (histScore / 250);

		updateBestMove(moveList->scores, &bestIdx, i);
		continue;
	}

	if (bestIdx != -1) {
		move_t temp = moveList->moves[0];
		moveList->moves[0] = moveList->moves[bestIdx];
		moveList->moves[0] = temp;
	}

}
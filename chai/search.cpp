#include "search.h"

void checkSearchInfo(SEARCH_INFO_S* s) {
	if (s->timeSet && getTimeMs() > s->stopTime) {
		cout << "Forced search quit (set time is up)" << endl;
		s->stopped = true;
	}
	readInput(s);
}

bool isRepetition(Board* b) { 
	return false; 
}

void moveSwapper(Board* b, MOVE_S* move_s, int curIdx) {
	int bestScore = 0;
	int bestIdx = curIdx;
	
	for (int i = curIdx; i < move_s->moveCounter; i++) {
		if (move_s->moveScore[i] > bestScore) {
			bestScore = move_s->moveScore[i];
			bestIdx = i;
		}
	}
	swapMove(move_s, curIdx, bestIdx);
}

void swapMove(MOVE_S* move_s, int id1, int id2) {
	if (id1 == id2) return; 

	// swap score and move int both lists
	int tempMove = move_s->moveList[id1];
	int tempScore = move_s->moveScore[id1];

	move_s->moveList[id1] = move_s->moveList[id2];
	move_s->moveScore[id1] = move_s->moveScore[id2];
	move_s->moveList[id2] = tempMove;
	move_s->moveScore[id2] = tempScore;
}

int alphaBeta(int alpha, int beta, int depth, Board* b, SEARCH_INFO_S* s, bool nullOk) {
	ASSERT(b->checkBoard());
	s->nodes++;
	
	if (depth <= 0) {
		return quiesence(alpha, beta, b, s);
		//return eval(b);
	}

	// check for time and depth
	if ((s->nodes & 2047) == 0) {
		checkSearchInfo(s);
	}

	// no evaluation if side is in check, no performance loss due to lower branching factor
	bool inCheck = b->isCheck(b->side);
	if (inCheck) {
		depth++;
	}

	if (isRepetition(b) || b->fiftyMove >= 100) {
		cout << "Abort due 50 move rule" << endl;
		return 0;
	}

	// keep, might be relevant for pawn endgames and hash table
	if (b->ply > MAX_DEPTH - 1) {
		return eval(b);
	}

	int score = -INF;
	if (nullOk && b->gameState != END && nullValid(b, inCheck)) {
		b->pushNull();
		score = -alphaBeta(-beta, -beta + 1, depth - 1 - R_NULL, b, s, !nullOk);
		b->pop();

		if (s->stopped) {
			return 0;
		}

		if (score >= beta) {
			return beta;
		}
	}

	MOVE_S move_s[1];
	generateMoves(b, move_s);

	int moveNum = 0;
	int legalMoves = 0;
	int oldAlpha = alpha;
	int bestMove = 0;
	score = -INF;

	for (int i = 0; i < move_s->moveCounter; i++) {
		moveSwapper(b, move_s, i);

		if (!b->push(move_s->moveList[i])) continue;

		legalMoves++;
		score = -alphaBeta(-beta, -alpha, depth-1, b, s, !nullOk);

		b->pop();

		if (s->stopped) {
			return 0;
		}

		// alpha beta pruning
		if (score > alpha) {
			// beta cutoff
			if (score >= beta) {
				if (legalMoves == 1) {
					s->fhf++;
				}

				// killers cannot be captures
				if (!(move_s->moveList[i] & MCHECKCAP)) {
					b->killer[1][b->ply] = b->killer[0][b->ply];
					b->killer[0][b->ply] = move_s->moveList[i];
				}

				s->fh++;
				return beta;
			}
			alpha = score;
			bestMove = move_s->moveList[i];
		}
	}

	if (legalMoves == 0) {
		// checkmate
		if (inCheck) {
			return -MATE + b->ply;
		} else {
			// stalemate
			return 0;
		}
 	}

	// if alpha has changed, a better move has been found -> adjust pv table
	if (alpha != oldAlpha) {
		storePV(b, bestMove);
	}

	return alpha;
}

int quiesence(int alpha, int beta, Board* b, SEARCH_INFO_S* search_info_s) {
	ASSERT(b->checkBoard());

	// check for time and depth
	if ((search_info_s->nodes & 2047) == 0) {
		checkSearchInfo(search_info_s);
	}

	search_info_s->nodes++;

	if (isRepetition(b) || b->fiftyMove >= 100) return 0;

	int standPat = eval(b);

	if (b->ply > MAX_DEPTH - 1) return standPat;

	if (standPat >= beta) {
		return beta;
	}
	if (standPat > alpha) {
		alpha = standPat;
	}

	int legalMoves = 0;
	int oldAlpha = alpha;
	int bestMove = 0;
	int score = -INF;

	MOVE_S move_s[1];
	generateCaptures(b, move_s);
	for (int i = 0; i < move_s->moveCounter; i++) {
		moveSwapper(b, move_s, i);

		if (!b->push(move_s->moveList[i])) continue;

		legalMoves++;
		score = -quiesence(-beta, -alpha, b, search_info_s);
		b->pop();

		if (search_info_s->stopped) {
			return 0;
		}

		if (score > alpha) {
			if (score >= beta) {
				if (legalMoves == 1) search_info_s->fhf++;
				search_info_s->fh++;
				return beta;
			}
			alpha = score;
			bestMove = move_s->moveList[i];
		}
	}

	if (alpha != oldAlpha) {
		storePV(b, bestMove);
	}

	return alpha;
}

bool nullValid(Board* b, bool inCheck) {
	return !inCheck && b->ply > 0 && countBits(b->getPieces(ROOK, b->side) | b->getPieces(QUEEN, b->side)) > 0;
}

void clearForSearch(Board* b, SEARCH_INFO_S* s) {
	// reset history of killer heuristic
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < MAX_DEPTH; j++) {
			b->killer[i][j] = 0;
		}
	}

	clearPVTable(b->pvTable_s);
	b->ply = 0;

	long value_ms = getTimeMs();
	s->startTime = value_ms;
	s->stopped = 0;
	s->nodes = 0;
	s->fh = 0;
	s->fhf = 0;
}

void search(Board* b, SEARCH_INFO_S* s) {
	int bestMove = 0;
	int bestScore = -INF;
	int pvMoves = 0;
	int pvNum = 0;

	clearForSearch(b, s);

	for (int currentDepth = 1; currentDepth <= s->depth; currentDepth++) {
		cout << "# DEPTH " << currentDepth << " #" << endl;
		bestScore = alphaBeta(-INF, INF, currentDepth, b, s, true);

		// forced stop, break and use pv line of previous iteration
		if (s->stopped) {
			break;
		}

		pvMoves = getPVLine(b, currentDepth);
		bestMove = b->pvArray[0];

		cout << "info depth " << currentDepth << " score cp "
			<< bestScore << " nodes " << s->nodes << " time " << (getTimeMs() - s->startTime);

		cout << " pv ";
		for (int i = 0; i < pvMoves; i++) {
			cout << getStringMove(b->pvArray[i]);
		}

		float perct = (s->fh == 0) ? 0 : (s->fhf / s->fh);
		printf("\n");
		cout << "Ordering percentage " << setprecision(3) << fixed << perct << endl << endl;
	}

	cout << "bestmove " << getStringMove(bestMove) << "\n";
}



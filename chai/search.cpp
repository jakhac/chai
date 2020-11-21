#include "search.h"

int selDepth = 0;
int deltaPruning = 0;

void checkSearchInfo(SEARCH_S* s) {
	if (s->timeSet && getTimeMs() > s->stopTime) {
		cout << "Forced search quit (set time is up)" << endl;
		s->stopped = true;
	}
	readInput(s);
}

bool isRepetition(Board* b) {
	for (int index = b->undoPly - b->fiftyMove; index < b->undoPly - 1; ++index) {
		ASSERT(index >= 0 && index < MAX_GAME_MOVES);
		if (b->zobristKey == b->undoHistory[index].zobKey) {
			return true;
		}
	}

	return false;
}

bool isThreeFoldRepetition(Board* b) {
	int reps = 0;

	for (int i = 0; i < b->halfMoves; i++) {
		if (b->zobristKey == b->undoHistory[i].zobKey) {
			reps++;
			if (reps >= 3) {
				return true;
			}
		}
	}
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

	// swap score and move in BOTH lists
	int tempMove = move_s->moveList[id1];
	int tempScore = move_s->moveScore[id1];

	move_s->moveList[id1] = move_s->moveList[id2];
	move_s->moveScore[id1] = move_s->moveScore[id2];
	move_s->moveList[id2] = tempMove;
	move_s->moveScore[id2] = tempScore;
}

int alphaBeta(int alpha, int beta, int depth, Board* b, SEARCH_S* s, bool nullOk, bool pvNode) {
	ASSERT(b->checkBoard());
	ASSERT(beta > alpha);

	// drop in quiesence if max depth is reached
	if (depth <= 0 || b->ply > MAX_DEPTH) {
		return quiesence(alpha, beta, b, s);
	}

	// check for time and depth
	if ((s->nodes & 4095) == 0) {
		checkSearchInfo(s);
	}

	s->nodes++;
	// In case of draw (3-fold-rep or 50-move rule) decide wether current position is winnable and
	// either score draw as 0 or penailze draw
	if ((isRepetition(b) || b->fiftyMove >= 100) && b->ply > 0) {
		return contemptFactor(b);
	}

	// variables relevant for mulitple pruning techniques
	bool inCheck = b->isCheck(b->side);
	int static_eval = eval(b);

	// no evaluation if side is in check, no performance loss due to lower branching factor
	if (inCheck) {
		depth = min(depth + 1, MAX_DEPTH);
	}

	// probe hash table for pv move and early cutoff
	int score = -INF;
	int pvMove = NO_MOVE;
	b->tt->probed++;
	if (probeTT(b, &pvMove, &score, alpha, beta, depth)) {
		b->tt->hit++;
		// in pvNodes return exact scores only
		if (!pvNode || (score > alpha && score < beta)) {
			return score;
		}
	}

	// mate distance pruning
	/*if (b->ply != 0) {
		if (alpha < -ISMATE) alpha = -ISMATE;
		if (beta > ISMATE - 1) beta = ISMATE - 1;
		if (alpha >= beta) return alpha;
	}*/

	// static null move pruning (reverse futility pruning)
	/*if (!pvNode && depth <= 3 && !inCheck && abs(beta - 1) > -ISMATE) {
		switch (depth) {
			case 1: 
				if (static_eval - pieceScores[BISHOP] > beta) return beta;
				break;
			case 2:
				if (static_eval - pieceScores[ROOK] > beta) return beta;
				break;
			case 3:
				if (static_eval - pieceScores[QUEEN] > beta) depth--;
				break;
			default:
				ASSERT(false); 
				break;
		}
	}*/

	// adaptive null move pruning
	/*bool endGame = countBits(b->occupied) <= 7 || b->countMajorPieces(b->side) <= 6;
	if (depth > 2 && nullOk && !endGame && !inCheck && !pvNode && static_eval >= beta) {
		b->pushNull();

		int r = (depth > 6) ? 3 : 2;

		score = -alphaBeta(-beta, -beta + 1, depth - 1 - r, b, s, NO_NULL, NO_PV);
		b->pop();

		if (s->stopped) return 0;
		if (score >= beta) return beta;
	}*/

	MOVE_S move_s[1];
	generateMoves(b, move_s);

	int currentMove;
	int moveNum = 0;
	int legalMoves = 0;
	int oldAlpha = alpha;
	int bestMove = 0;
	int bestScore = -INF;
	score = -INF;

	if (pvMove != NO_MOVE) {
		for (int i = 0; i < move_s->moveCounter; i++) {
			if (pvMove == move_s->moveList[i]) {
				s->pvHits++;
				move_s->moveScore[i] = 2000000;
				break;
			}
		}
	}

	// set futility pruning flag
	bool fPrune = false;
	/*int fmargin[4] = { 0, 200, 325, 550 };
	if (depth <= 3 && !inCheck && !pvNode && abs(alpha) < 9000) {
		if (static_eval + fmargin[depth] <= alpha) {
			fPrune = true;
		}
	}*/

	// main move loop
	for (int i = 0; i < move_s->moveCounter; i++) {
		moveSwapper(b, move_s, i);
		currentMove = move_s->moveList[i];

		if (!b->push(currentMove)) continue;

		// Futility pruning: skip moves that are futile and have no chance of raising alpha
		// if at least one legal move was made before
		if (legalMoves && fPrune && !(CAPTURED(currentMove) && !(MCHECKPROM & currentMove) &&
			!b->squareAttackedBy(b->getKingSquare(b->side^1), b->side))) {
			b->pop();
			continue;
		}

		legalMoves++;
		int reduction = 0;

		score = -alphaBeta(-beta, -alpha, depth - 1, b, s, DO_NULL, NO_PV);

		/*if (legalMoves == 1) {
			// always do full search on first move
			score = -alphaBeta(-beta, -alpha, depth - 1, b, s, DO_NULL, IS_PV);
		} else {
			// late move reduction
			if (i > 3 && !CAPTURED(currentMove) && !inCheck && depth >= 3 && !pvNode) {
				reduction = (i > 6) ? 2 : 3;
			}

			// pvs
			score = -alphaBeta(-alpha - 1, -alpha, depth - 1 - reduction, b, s, DO_NULL, NO_PV);
			
			// check if pvs scores higher than alpha, if so do re-search
			if (score > alpha && score < beta) {
				score = -alphaBeta(-beta, -alpha, depth - 1, b, s, DO_NULL, IS_PV);
				if (score > alpha) {
					alpha = score;
				}
			}
		}*/

		b->pop();

		if (s->stopped) {
			return 0;
		}

		if (score > bestScore) {
			bestScore = score;
			bestMove = currentMove;

			// alpha beta pruning
			if (score > alpha) {
				// beta cutoff
				if (score >= beta) {
					if (legalMoves == 1) {
						s->fhf++;
					}
					s->fh++;

					// killers cannot be captures
					if (!(currentMove & MCHECKCAP)) {
						b->killer[1][b->ply] = b->killer[0][b->ply];
						b->killer[0][b->ply] = currentMove;
					}

					// store tt
					storeTT(b, bestMove, beta, TT_BETA, depth);

					return beta;
				}
				alpha = score;
			}
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

	ASSERT(alpha >= oldAlpha); // alpha cant get worse

	// if alpha has changed, a better move has been found -> adjust pv table
	if (alpha != oldAlpha) {
		// exact score since alpha has improved
		storeTT(b, bestMove, bestScore, TT_SCORE, depth);
	} else {
		// no improvemnt, keep lower bound
		storeTT(b, bestMove, alpha, TT_ALPHA, depth);
	}

	return alpha;
}

int quiesence(int alpha, int beta, Board* b, SEARCH_S* s) {
	ASSERT(b->checkBoard());
	selDepth = max(selDepth, b->ply);

	// check for time and depth
	if ((s->nodes & 2047) == 0) {
		checkSearchInfo(s);
	}

	s->nodes++;

	if ((isRepetition(b) || b->fiftyMove >= 100) && b->ply > 0) {
		return contemptFactor(b);
	}

	int standPat = eval(b);

	if (b->ply > MAX_DEPTH - 1) return standPat;

	// stand pat pruning
	if (standPat >= beta) {
		return beta;
	}

	int pvMove = NO_MOVE;
	int score = -INF;
	//b->tt->probed++;
	// try pvMove found from hash table
	probeTT(b, &pvMove, &score, alpha, beta, MAX_DEPTH + 1);

   // rise alpha to stand pat
	if (standPat > alpha) {
		alpha = standPat;
	}

	int legalMoves = 0;
	int oldAlpha = alpha;
	int bestMove = 0;
	score = -INF;

	MOVE_S move_s[1];
	generateCaptures(b, move_s);

	if (pvMove != NO_MOVE && CAPTURED(pvMove)) {
		for (int i = 0; i < move_s->moveCounter; i++) {
			if (pvMove == move_s->moveList[i]) {
				s->pvHits++;
				move_s->moveScore[i] = 2000000;
				break;
			}
		}
	}

	for (int i = 0; i < move_s->moveCounter; i++) {
		moveSwapper(b, move_s, i);
		int currentMove = move_s->moveList[i];
		ASSERT(currentMove & MCHECKCAP || currentMove & MFLAGEP);

		// Delta cutoff: prune moves that cannot improve over alpha
		bool endGame = countBits(b->occupied) <= 7 || b->countMajorPieces(b->side) <= 6;
		if (standPat + pieceScores[CAPTURED(currentMove)] + 200 < alpha && 
			!endGame && !(MCHECKPROM & currentMove)) {
			continue;
		}

		// SEE pruning, skip nodes with negative see score
		if (move_s->moveScore[i] < 0 && !(MCHECKPROM & currentMove)) continue;

		if (!b->push(currentMove)) continue;

		legalMoves++;
		score = -quiesence(-beta, -alpha, b, s);
		b->pop();

		if (s->stopped) {
			return 0;
		}

		if (score > alpha) {
			if (score >= beta) {
				if (legalMoves == 1) s->fhf++;
				s->fh++;
				return beta;
			}
			alpha = score;
			bestMove = move_s->moveList[i];
		}
	}

	ASSERT(alpha >= oldAlpha);

	return alpha;
}

void clearForSearch(Board* b, SEARCH_S* s) {
	// reset history of killer heuristic
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < MAX_DEPTH; j++) {
			b->killer[i][j] = 0;
		}
	}

	b->ply = 0;

	// reset stats
	b->tt->hit = 0;
	b->tt->probed = 0;
	b->tt->valueHit = 0;

	b->pawnTable->collided = 0;
	b->pawnTable->probed = 0;
	b->pawnTable->hit = 0;

	s->startTime = getTimeMs();
	s->stopped = 0;
	s->nodes = 0;
	s->fhf = 0;
	s->fh = 0;
}

int search_aspiration(Board* b, SEARCH_S* s, int depth, int bestScore) {
	int temp = bestScore;
	int alpha = bestScore - 75;
	int beta = bestScore + 75;

	selDepth = 0;
	temp = alphaBeta(alpha, beta, depth, b, s, DO_NULL, IS_PV);
	if (temp <= alpha || temp >= beta) {
		selDepth = 0;
		temp = alphaBeta(-INF, INF, depth, b, s, DO_NULL, IS_PV);
	}

	return temp;
}

void search(Board* b, SEARCH_S* s) {
	int bestMove = 0;
	int bestScore = -INF;
	int pvMoves = 0;
	int pvNum = 0;

	log("Entered search");

	clearForSearch(b, s);

	selDepth = 0;
	b->ply = 0;
	bestScore = alphaBeta(-INF, INF, 1, b, s, DO_NULL, IS_PV);
	pvMoves = getPVLine(b, 1);
	bestMove = b->pvArray[0];

	fflush(stdout);
	cout << "info depth 1 seldepth " << selDepth << " score cp " << bestScore 
		 << " nodes " << s->nodes << " time " << (getTimeMs() - s->startTime) << " pv "
		 << getStringMove(bestMove) << "\n";
	fflush(stdout);

	for (int currentDepth = 2; currentDepth <= s->depth; currentDepth++) {
		selDepth = 0;
		b->ply = 0;

		int bestScore = alphaBeta(-INF, INF, currentDepth, b, s, DO_NULL, IS_PV);
		//bestScore = search_aspiration(b, s, currentDepth, bestScore);

		// forced stop, break and use pv line of previous iteration
		if (s->stopped) break;

		pvMoves = getPVLine(b, currentDepth);
		bestMove = b->pvArray[0];

		fflush(stdout);
		cout << "info depth " << currentDepth << " seldepth " << selDepth << " score cp " << bestScore << " nodes " << s->nodes << " time " << (getTimeMs() - s->startTime);

		cout << " pv ";
		for (int i = 0; i < pvMoves; i++) {
			cout << getStringMove(b->pvArray[i]);
		}

		/*cout << "\n";
		cout << "Ordering percentage: \t\t" << setprecision(3) << fixed << (float)(s->fhf / s->fh) << endl;
		cout << "T table hit percentage: \t" << setprecision(3) << fixed << (float)(b->tt->hit) / (b->tt->probed) << endl;
		cout << "T table hit used: \t\t" << (float)(b->tt->valueHit) / (b->tt->probed) << endl;
		cout << "T table memory used: \t\t" << setprecision(5) << fixed << (float)(b->tt->stored) / (b->tt->entries) << endl;
		cout << "Pawn table hit percentage: \t" << setprecision(3) << fixed << (float)(b->pawnTable->hit) / (b->pawnTable->probed) << endl;
		cout << "Pawn table memory used: \t" << setprecision(5) << fixed << (float)(b->pawnTable->stored) / (b->pawnTable->entries) << endl;
		cout << "Pawn table collisions: \t\t" << setprecision(3) << fixed << b->pawnTable->collided << endl;
		cout << endl;*/

		// quit when checkmate was found
		/*if (currentDepth > 1 && bestScore > ISMATE) {
			cout << "\nEarly exit: found forced checkmate" << endl;
			break;
		}*/

		cout << "\n";
	}

	fflush(stdout);
	log("Left search and cout bestmove found");
	cout << "\n";
	cout << "bestmove " << getStringMove(bestMove) << "\n";
	fflush(stdout);
}



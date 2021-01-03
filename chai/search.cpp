#include "search.h"

int selDepth = 0;
int deltaPruning = 0;

void checkSearchInfo(search_t* s) {
	if (s->timeSet && getTimeMs() > s->stopTime) {
		cout << "Forced search quit (set time is up)" << endl;
		s->stopped = true;
	}

	// read input causes tests to wait for cin and does not terminate
#ifndef TESTING
	//readInput(s);
#endif // TESTING

}

bool isRepetition(Board* b) {
	for (int index = b->undoPly - b->fiftyMove; index < b->undoPly - 1; ++index) {
		Assert(index >= 0 && index < MAX_GAME_MOVES);
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

void getNextMove(Board* b, moveList_t* move_s, int curIdx) {
	int bestScore = 0;
	int bestIdx = curIdx;

	for (int i = curIdx; i < move_s->cnt; i++) {
		if (move_s->scores[i] > bestScore) {
			bestScore = move_s->scores[i];
			bestIdx = i;
		}
	}
	swapMove(move_s, curIdx, bestIdx);
}

void swapMove(moveList_t* move_s, int id1, int id2) {
	if (id1 == id2) return;

	// swap score and move in BOTH lists
	int tempMove = move_s->moves[id1];
	int tempScore = move_s->scores[id1];

	move_s->moves[id1] = move_s->moves[id2];
	move_s->scores[id1] = move_s->scores[id2];
	move_s->moves[id2] = tempMove;
	move_s->scores[id2] = tempScore;
}

int alphaBeta(int alpha, int beta, int depth, Board* b, search_t* s, bool nullOk, bool pvNode, pv_line_t* pvLine) {
	Assert(b->checkBoard());
	Assert(beta > alpha);

	pv_line_t localPV[1]{};
	localPV->len = 0;

	// drop in quiescence if max depth is reached
	if (depth <= 0 || b->ply > MAX_DEPTH) {
		pvLine->len = 0;
		return quiescence(alpha, beta, b, s);
	}

	// check for time and depth
	if ((s->nodes & 2047) == 0) {
		checkSearchInfo(s);
	}

	s->nodes++;
	// In case of draw (3-fold-rep or 50-move rule) decide wether current position is winnable and
	// either score draw as 0 or penalize draw
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

	// prefetch ttable entry into cache
	prefetchTTEntry(b);

	// probe hash table for pv move and early cutoff
	int score = -INF;
	move_t pvMove = NO_MOVE;
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

		score = -alphaBeta(-beta, -beta + 1, depth - 1 - r, b, s, NO_NULL, NO_PV, localPV);
		b->pop();

		if (s->stopped) return 0;
		if (score >= beta) return beta;
	}*/

	moveList_t moveList[1];
	generateMoves(b, moveList, inCheck);

	// score hash move -> check already done in scoreMoves
	/*// check if hash move is possible
	if (pvMove != NO_MOVE) {
		for (int i = 0; i < moveList->cnt; i++) {
			if (pvMove == moveList->moves[i]) {
				s->pvHits++;
				moveList->scores[i] = HASH_MOVE;
				break;
			}
		}
	}*/

	scoreMovesAlphaBeta(b, moveList, pvMove);

	move_t currentMove;
	move_t bestMove = NO_MOVE;
	int moveNum = 0;
	int legalMoves = 0;
	int oldAlpha = alpha;
	int bestScore = -INF;
	score = -INF;

	// compare hash move and pv move, maybe use move from pv line if no hash move or iid
	/*if (moveLegal(b, pvLine->line[b->ply - 1]) && pvMove == pvLine->line[b->ply - 1]) {
		cout << "pv line equals hash move " << getStringMove(pvLine->line[b->ply - 1]) << endl;
		pvMove = pvLine->line[b->ply - 1];
	}*/

	// set futility pruning flag
	bool fPrune = false;
	int fmargin[4] = { 0, 200, 325, 550 };
	if (depth <= 3 && !inCheck && !pvNode && abs(alpha) < 9000) {
		if (static_eval + fmargin[depth] <= alpha) {
			fPrune = true;
		}
	}

	// main move loop
	for (int i = 0; i < moveList->cnt; i++) {
		getNextMove(b, moveList, i);
		currentMove = moveList->moves[i];
		Assert(currentMove != NO_MOVE);

		if (!b->push(currentMove)) continue;
		// if (!inCheck || leavesKingInCheck()) continue;

		// Futility pruning: skip moves that are futile and have no chance of raising alpha
		// if at least one legal move was made before
		/*if (legalMoves && fPrune && !(CAPTURED(currentMove) && !(MCHECKPROM & currentMove) &&
			!b->squareAttackedBy(b->getKingSquare(b->side ^ 1), b->side))) {
			b->pop();
			continue;
		}*/

		legalMoves++;
		int reduction = 0;

		score = -alphaBeta(-beta, -alpha, depth - 1, b, s, DO_NULL, NO_PV, localPV);

		/*if (legalMoves == 1) {
			// always do full search on first move
			score = -alphaBeta(-beta, -alpha, depth - 1, b, s, DO_NULL, IS_PV, localPV);
		} else {
			// late move reduction
			if (i > 3 && !CAPTURED(currentMove) && !inCheck && depth >= 3 && !pvNode) {
				reduction = (i > 6) ? 2 : 3;
			}

			// pvs
			score = -alphaBeta(-alpha - 1, -alpha, depth - 1 - reduction, b, s, DO_NULL, NO_PV, localPV);

			// check if pvs scores higher than alpha, if so do re-search
			if (score > alpha && score < beta) {
				score = -alphaBeta(-beta, -alpha, depth - 1, b, s, DO_NULL, IS_PV, localPV);

				// not necessary? already done ab pruning part
				//if (score > alpha) {
				//	alpha = score;
				//}
			}
		}*/

		b->pop();

		if (s->stopped) {
			return 0;
		}

		/*
		* Alpha-Beta Pruning:
		* If score is greater than beta, the move is too good. The opponent already has a line that prevents
		* this move and therefore it can be pruned.
		*/
		if (score >= beta) {
			// stats: count fail-high percentage on first move in node
			if (legalMoves == 1) {
				s->fhf++;
			}
			s->fh++;

			/*
			* Standard Killer Heuristic:
			* Moves that caused a beta cutoff are likely to be good in other positions in the same ply.
			* These moves are stored and considered in move ordering. Killers within same ply cannot be
			* - equal
			* - captures (already ordered high)
			* - promotions (already ordered high)
			* - when in check
			* since they are either not stable or already considered in move ordering.
			*/
			if (!(currentMove & MCHECK_CAP)
				&& !inCheck
				&& !(currentMove & MCHECK_PROM)
				&& b->killer[0][b->ply] != currentMove) {

				b->killer[1][b->ply] = b->killer[0][b->ply];
				b->killer[0][b->ply] = currentMove;

				Assert(b->killer[1][b->ply] != b->killer[0][b->ply]);
			}

			/*
			* Mate Killers:
			* If a move scores near checkmate, order it above standard killers.
			*/
			if (bestScore >= (MATE - 1000)
				&& currentMove != NULL_MOVE) {
				b->mateKiller[b->ply] = currentMove;
			}

			/*
			* History Heuristic:
			* Piece-To information is stored for each side. Do not apply, if capture, promoting or
			* in check (already considered in move ordering). Remember maximum history score and
			* rescale if needed to prevent overflow.
			*/
			if (!inCheck && !(currentMove & MCHECK_PROM_OR_CAP)) {
				int piece = b->pieceAt(fromSq(currentMove));
				int to = toSq(currentMove);

				b->histHeuristic[piece][to] += depth * depth;
				b->histMax = max(b->histHeuristic[piece][to], b->histMax);

				if (b->histMax > HISTORY_MAX) {
					for (piece = P; piece <= k; piece++) {
						for (int sq = 0; sq < NUM_SQUARES; sq++) {
							b->histHeuristic[piece][sq] /= 2;
						}
					}
					b->histMax /= 2;
				}
			}

			/*
			* Counter-Move Heuristic:
			* If currentMove caused a beta cutoff, store previous move FROM and TO
			* and assign currentMove as an intuitive counter move for consideration in
			* move ordering. Counter moves are not stored if in check or currentMove is
			* a capture or promotion (already considered in move ordering). Only possible if ply > 0;
			*/
			if (!inCheck
				&& b->ply > 0
				&& !(currentMove & MCHECK_PROM_OR_CAP)
				&& currentMove != NULL_MOVE) {
				move_t prevMove = b->undoHistory[b->ply - 1].move;
				if (prevMove != NULL_MOVE) {
					b->counterHeuristic[fromSq(prevMove)][toSq(prevMove)][b->side] = currentMove;
				}
			}

			// Store position with bestMove and beta flag
			storeTT(b, bestMove, beta, TT_BETA, depth);
			return beta;
		}

		/*
		* If the currentMove scores higher than bestMove, update score and move.
		* This might be useful when this position is stored into the ttable.
		*/
		if (score > bestScore) {
			bestScore = score;
			bestMove = currentMove;

			/*
			* If the currentMove scores higher than alpha, the principal variation
			* is updated because a better move for this position has been found.
			* This information can be stored in (counter) history heuristic to improve
			* further move ordering.
			*/
			if (score > alpha) {
				alpha = score;

				pvLine->line[0] = currentMove;
				memcpy(pvLine->line + 1, localPV->line, localPV->len * sizeof(currentMove));
				pvLine->len = localPV->len + 1;
				Assert(pvLine->len <= MAX_DEPTH);
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

	Assert(alpha >= oldAlpha); // alpha cant get worse

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

int quiescence(int alpha, int beta, Board* b, search_t* s) {
	Assert(b->checkBoard());
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
	bool inCheck = b->isCheck(b->side);

	if (b->ply > MAX_DEPTH - 1) return standPat;


	/*
	* If a position scores higher than alpha, update the value to keep a lower bound of the
	* position. If the evaluation is already above beta, there is no point in searching all captures,
	* because there is no need to recapture in a good position. Do not prune when in check: these positions
	* may contain a non-tactical move that raises alpha or possibly no move due to serious mate threat.
	*/
	if (!inCheck) {
		if (standPat > alpha) {
			alpha = standPat;
		}
		if (standPat >= beta) {
			return beta;
		}
	}

	int legalMoves = 0;
	int oldAlpha = alpha;
	int bestMove = 0;
	int score = -INF;

	moveList_t moveList[1];
	generateQuiescence(b, moveList, inCheck);

	// If in check, all evading moves are generated and have to be scored differently.
	// Quiescence move ordering only handles captures and promotions.
	if (inCheck) {
		scoreMovesAlphaBeta(b, moveList, NO_MOVE);
	} else {
		scoreMovesQuiescence(b, moveList);
	}

	/*
	* There are no cutoffs in this node yet. Generate all captures, promotions or check evasions
	* and expand this node.
	*/
	for (int i = 0; i < moveList->cnt; i++) {
		getNextMove(b, moveList, i);
		int currentMove = moveList->moves[i];

		// either position is a check or we capture / promote / enpas
		Assert(inCheck || (currentMove & MCHECK_PROM_OR_CAP || currentMove & MFLAG_EP));

		// Delta cutoff: prune moves that cannot improve over alpha
		//bool endGame = countBits(b->occupied) <= 7 || b->countMajorPieces(b->side) <= 6;
		/*if (standPat + pieceScores[capPiece(currentMove)] + 200 < alpha &&
			!endGame && !(MCHECK_PROM & currentMove)) {
			continue;
		}*/

		/*
		* If SEE score is negative, this is a losing capture. Since there is little to no chance that
		* this move raises alpha, it can be pruned.
		*/
		if (moveList->scores[i] < (BAD_CAPTURE + MVV_LVA_UBOUND)) {
			continue;
		}

		if (!b->push(currentMove)) continue;

		legalMoves++;
		score = -quiescence(-beta, -alpha, b, s);
		b->pop();

		if (s->stopped) {
			return 0;
		}

		if (score > alpha) {
			alpha = score;
			bestMove = moveList->moves[i];
		}

		/**
		 * Beta cutoff: Move is too good and the opponent already a way to defend this attack.
		 */
		if (score >= beta) {
			// stats
			if (legalMoves == 1) {
				s->fhf++;
			}
			s->fh++;

			return beta;
		}

	}

	Assert(alpha >= oldAlpha);

	return alpha;
}

void clearForSearch(Board* b, search_t* s) {
	// reset history of killer heuristic
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < MAX_DEPTH; j++) {
			b->killer[i][j] = 0;
		}
	}

	// reset history heuristic
	for (int i = P; i < k; i++) {
		for (int j = 0; j < NUM_SQUARES; j++) {
			b->histHeuristic[i][j] = 0;
		}
	}
	b->histMax = 0;

	// counter move history
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < NUM_SQUARES; j++) {
			for (int k = 0; k < NUM_SQUARES; k++) {
				b->counterHeuristic[j][k][i] = 1;
			}
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

int search_aspiration(Board* b, search_t* s, int depth, int bestScore) {
	int temp = bestScore;
	int alpha = bestScore - 75;
	int beta = bestScore + 75;

	//selDepth = 0;
	//temp = alphaBeta(alpha, beta, depth, b, s, DO_NULL, IS_PV);
	//if (temp <= alpha || temp >= beta) {
		//selDepth = 0;
		//temp = alphaBeta(-INF, INF, depth, b, s, DO_NULL, IS_PV);
	//}

	return temp;
}

int search(Board* b, search_t* s) {
	int bestMove = 0;
	int bestScore = -INF;
	int pvMoves = 0;
	int pvNum = 0;

	log("Entered search");

	clearForSearch(b, s);

	selDepth = 0;
	b->ply = 0;
	bestScore = alphaBeta(-INF, INF, 1, b, s, DO_NULL, IS_PV, pvLine);
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

		bestScore = alphaBeta(-INF, INF, currentDepth, b, s, DO_NULL, IS_PV, pvLine);
		//bestScore = search_aspiration(b, s, currentDepth, bestScore);

		// forced stop, break and use pv line of previous iteration
		if (s->stopped) break;

		pvMoves = getPVLine(b, currentDepth);
		bestMove = b->pvArray[0];

		fflush(stdout);
		cout << "info depth " << currentDepth << " seldepth " << selDepth << " score cp " << bestScore << " nodes " << s->nodes << " time " << (getTimeMs() - s->startTime);

		cout << " pv ";

#ifdef TT_PV
		for (int i = 0; i < pvMoves; i++) {
			cout << getStringMove(b->pvArray[i]);
		}
#else
		for (int i = 0; i < currentDepth; i++) {
			cout << getStringMove(pvLine->line[i]);
		}
#endif // TT_PV_LINE

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

	return bestScore;
}



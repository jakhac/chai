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
	readInput(s);
#endif // TESTING

}

bool isRepetition(board_t* b) {

	// TODO verify max() condition
	for (int index = max(0, b->undoPly - b->fiftyMove); index < b->undoPly - 1; ++index) {
		Assert(index >= 0 && index < MAX_GAME_MOVES);

		if (b->zobristKey == b->undoHistory[index].zobKey) {
			return true;
		}
	}

	return false;
}

bool isThreeFoldRepetition(board_t* b) {
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

void getNextMove(moveList_t* move_s, int curIdx) {
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

bool zugzwang(board_t* b) {
	bitboard_t bb;

	// Less than 5 pieces on the board is obvious endgame
	if (countBits(b->occupied) <= 5) {
		return true;
	}

	// Pawn endgame, no pieces left
	bb = b->pieces[Piece::KNIGHT] | b->pieces[Piece::BISHOP] | b->pieces[Piece::ROOK] | b->pieces[Piece::QUEEN];
	if (!bb) {
		return true;
	}

	// No pawns left on the board
	bb = b->pieces[Piece::PAWN];
	if (!bb) {
		return true;
	}

	// Knight endgame (only pieces are knight, cannot lose tempo)
	bb = b->pieces[Piece::BISHOP] | b->pieces[Piece::ROOK] | b->pieces[Piece::QUEEN];
	if (!bb) {
		return true;
	}

	// All pawns of one side are blocked (cannot safely advance)
	if (!hasSafePawnPush(b, b->side)) {
		return true;
	}

	return false;
}

int alphaBeta(int alpha, int beta, int depth, board_t* b, search_t* s, bool nullOk, bool pvNode, pv_line_t* pvLine) {
	Assert(checkBoard(b));
	Assert(beta > alpha);

	/*
	* In case of draw (3-fold-rep or 50-move rule) decide wether current position is winnable and
	* either score draw as 0 or penalize draw.
	*/
	if ((isRepetition(b) || b->fiftyMove >= 100) && b->ply > 0) {
		return contemptFactor(b);
	}

	/**
	 * Mate Distance Pruning:
	 * If alpha or beta are already mate scores, bounds can be adjusted to prune irrelevant subtrees.
	 * Mates are delivered faster, but does not speed are search.
	 */
	if (beta > (MATE_VALUE - depth - 1)) {
		beta = MATE_VALUE - depth - 1;
	}
	if (alpha < (-MATE_VALUE + depth)) {
		alpha = -MATE_VALUE + depth;
	}
	if (alpha >= beta) {
		return alpha;
	}

	/* mate-distance pruning */
	//alpha = max(alpha, -MATE_VALUE + b->ply);
	//beta = min(beta, MATE_VALUE - b->ply - 1);
	//if (alpha >= beta)
	//	return alpha;

	prefetchTTEntry(b);

	/*
	* Transposition Table Probing:
	* Probe the TTable and look for useful information from previous transpositions. Return hashScore if hash table
	* stored a better score at same or greater depth. Do not return if close to 50-move draw.
	*/
	int hashScore = -INF;
	int hashDepth = -1;
	uint8_t hashFlag = TT_NONE;
	move_t hashMove = NO_MOVE;

	bool hashStored = probeTT(b, &hashMove, &hashScore, &hashFlag, &hashDepth);
	if (hashStored) {
		b->tt->hit++;

		// Look for valueHit: This position has already been searched before with greater depth.
		if (hashDepth >= depth && b->fiftyMove < 85) {
			Assert(hashDepth >= 1 && hashDepth <= MAX_DEPTH);
			Assert(hashFlag >= TT_ALPHA && hashFlag <= TT_SCORE);
			Assert(hashScore >= -INF && hashScore <= INF);

			// Convert mate scores
			hashToSearch(b, &hashScore);

			if (hashFlag == TT_SCORE) {
				b->tt->valueHit++;
				return hashScore;
			}

			if (hashFlag == TT_BETA && beta <= hashScore) {
				b->tt->valueHit++;
				return hashScore;
			}

			if (hashFlag == TT_ALPHA && alpha >= hashScore) {
				b->tt->valueHit++;
				return hashScore;
			}
		}
	}

	//pv_line_t localPV[1]{};
	//localPV->len = 0;

	// Drop into quiescence if maximum depth is reached.
	if (depth <= 0 || b->ply > MAX_DEPTH) {
		//pvLine->len = 0;

		int quietScore = quiescence(alpha, beta, 0, b, s, nullptr);
		Assert(abs(quietScore) < INF);

		return quietScore;
	}

	// check for time and depth
	if ((s->nodes & 2047) == 0) {
		checkSearchInfo(s);
	}

	s->nodes++;

	// variables relevant for mulitple pruning techniques
	bool inCheck = isCheck(b, b->side);
	bool mateThreat = false;
	int lazyEval = lazyEvalulation(b);
	//int staticEval = eval(b);
	int searchExt = 0;

	int alphaIID = alpha;
	int betaIID = beta;

	// no evaluation if side is in check, no performance loss due to lower branching factor
	if (inCheck) {
		searchExt++;
	}

	/*
	* Null Move Pruning:
	* Give opposite side a free move and use nullScore as lower bound (alpha).
	* If this position is still winning, e.g. fails high, it will never be reached.
	* Restrict NMP to reasonable positions (zugzwang, depth, checks).
	*/
	bool doNull = nullOk
		&& !inCheck
		&& !pvNode
		&& depth > 2
		&& lazyEval > beta
		&& abs(beta) < abs(ISMATE)
		&& countBits(b->occupied) > 7
		&& !zugzwang(b);

	if (doNull) {
		int reduction = (depth > 6) ? R_3 : R_2;
		int nullScore = 0;

		pushNull(b);
		nullScore = -alphaBeta(-beta, -beta + 1, depth - 1 - reduction, b, s, NO_NULL, NO_PV, nullptr);
		pop(b);

		if (s->stopped) {
			return 0;
		}

		// Even after giving opponent free move, position is winning..
		if (nullScore >= beta && abs(nullScore) < ISMATE) {
			//Beta cutoff->store in ttable
			if (depth - 1 - reduction > 0) {
				storeTT(b, NULL_MOVE, nullScore, TT_ALPHA, depth - 1 - reduction); // TODO might cause forfeit
			}
			return nullScore;
		}

		// Mate threat detection
		if (abs(beta) > ISMATE && depth <= 5) {
			mateThreat = true;
		}
	}

	// Razoring http://talkchess.com/forum3/viewtopic.php?t=43165
	bool doRazoring = !inCheck
		&& !mateThreat
		&& abs(beta) < ISMATE
		&& depth <= RAZOR_DEPTH;

	//if (doRazoring) {
	//	// TODO: use dynamic pawn eval
	//	int pawnValue = pieceScores[Piece::PAWN];
	//	int razorMargin = 2 * pieceScores[Piece::PAWN] + (depth - 1 * pawnValue) / 4;

	//	if (lazyEval + razorMargin <= alpha) {
	//		int qScore = quiescence(alpha, beta, 0, b, s, localPV);

	//		if (qScore + razorMargin <= alpha) {
	//			return alpha;
	//		}
	//	}
	//}

	/*
	* Internal Iterative Deepening:
	* If ttable probing does not find a hash move, there is no good move to start searching this
	* position. IID searches with reduced depth and fills ttable entries to ensure hash move.
	* TODO: only use IID in PV nodes? -> http://talkchess.com/forum3/viewtopic.php?t=40484
	*/
	if (hashMove == NO_MOVE && depth > 4) {
		int depthIID = (depth / 2) + 1;
		alphaBeta(alphaIID, betaIID, depthIID, b, s, NO_NULL, NO_PV, nullptr);
		hashMove = probePV(b);
	}

	//if (hashMove == NO_MOVE && depth > 3) {
	//	if (pvNode) {
	//		alphaBeta(alpha, beta, depth - 3, b, s, NO_NULL, NO_PV, localPV);
	//	} else {
	//		alphaBeta(alpha, beta, 1, b, s, NO_NULL, NO_PV, localPV);
	//	}
	//	hashMove = probePV(b);
	//}

	moveList_t moveList[1];
	generateMoves(b, moveList, inCheck);
	scoreMoves(b, moveList, hashMove);

	move_t currentMove;
	move_t bestMove = NO_MOVE;
	int moveNum = 0;
	int legalMoves = 0;
	int oldAlpha = alpha;
	int bestScore = -INF;
	int score = -INF;
	bool skipSearch = false;


	// Futility Pruning flag determines if f-pruning can be applied to this position.
	bool doFutility = !inCheck
		&& !pvNode
		&& depth <= 2
		&& searchExt == 0
		&& !mateThreat
		&& abs(alpha) <= 10000
		&& abs(beta) <= 10000;

	// LMR flag determines, if this position is a candidate for late move reductions.
	bool lmrCandidate = !mateThreat
		&& !inCheck
		&& !searchExt
		&& depth >= 3;

	/**
	 * This position could not be refuted yet. Therefore, moves are generated
	 * and searching continues.
	 */
	for (int i = 0; i < moveList->cnt; i++) {
		getNextMove(moveList, i);
		currentMove = moveList->moves[i];
		Assert(currentMove != NO_MOVE);

		if (!push(b, currentMove)) continue;

		bool moveGivesCheck = isCheck(b, b->side);

		/**
		 * Futility Pruning:
		 * Try to prove that moves in a bad position cannot improve alpha and are futile.
		 * If the move + (margin or capPiece) fails low, do not search any further.
		 *
		 * Restrictions:
		 * - No promotions or ep moves
		 * - At least one legal move found
		 * - Move does not give check (!)
		 */
		skipSearch = false;
		if (doFutility
			&& legalMoves
			&& !(MCHECK_PROM & currentMove)
			&& !(MCHECK_EP & currentMove)
			&& !moveGivesCheck
			) {

			int capPieceValue = (MCHECK_CAP & currentMove) ? pieceScores[capPiece(currentMove)] : 200;

			if (depth == 2 && lazyEval + capPieceValue + F2_MARGIN < alpha) {
				s->futileCnt++;
				pop(b);
				continue;
			}

			if (depth == 1 && lazyEval + capPieceValue + F1_MARGIN < alpha) {
				s->futileCnt++;
				pop(b);
				continue;
			}

		}

		legalMoves++;

		// TODO doc
		// Always assume, that the first move is part of the principal variation.
		if (legalMoves == 1) {
			score = -alphaBeta(-beta, -alpha, depth - 1 + searchExt, b, s, DO_NULL, pvNode, nullptr);
		} else {
			// Late Move Reductions, do reduced zero window search
			if (legalMoves > 3
				//&& !moveGivesCheck
				&& lmrCandidate
				&& !(MCHECK_PROM_OR_CAP & currentMove)) {

				int lmrDepth = (legalMoves > 6) ? 3 : 2;
				score = -alphaBeta(-alpha - 1, -alpha, depth - lmrDepth, b, s, DO_NULL, NO_PV, nullptr);
			} else {
				// Ensure, that PVS is always exexcuted if LMR is not applicable
				score = alpha + 1;
			}

			// Principal Variation Search
			if (score > alpha) {
				score = -alphaBeta(-alpha - 1, -alpha, depth - 1 + searchExt, b, s, DO_NULL, NO_PV, nullptr);

				if (score > alpha && score < beta) {
					score = -alphaBeta(-beta, -alpha, depth - 1 + searchExt, b, s, DO_NULL, NO_PV, nullptr);
				}
			}
		}

		pop(b);

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
				&& killer[0][b->ply] != currentMove) {

				killer[1][b->ply] = killer[0][b->ply];
				killer[0][b->ply] = currentMove;
				Assert(killer[1][b->ply] != killer[0][b->ply]);
			}

			/*
			* Mate Killers:
			* If a move scores near checkmate, order it above standard killers.
			*/
			if (bestScore >= (MATE_VALUE - 1000) && currentMove > 1) {
				mateKiller[b->ply] = currentMove;
			}

			/*
			* History Heuristic:
			* Piece-To information is stored for each side. Do not apply, if capture, promoting or
			* in check (already considered in move ordering). Remember maximum history score and
			* rescale if needed to prevent overflow.
			*/
			if (!inCheck && !(currentMove & MCHECK_PROM_OR_CAP)) {
				int piece = pieceAt(b, fromSq(currentMove));
				int to = toSq(currentMove);

				histHeuristic[piece][to] += depth * depth;
				histMax = max(histHeuristic[piece][to], histMax);

				if (histMax > HISTORY_MAX) {
					for (piece = Piece::P; piece <= Piece::k; piece++) {
						for (int sq = 0; sq < NUM_SQUARES; sq++) {
							histHeuristic[piece][sq] /= 2;
						}
					}
					histMax /= 2;
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
				&& pseudoValidBitMove(currentMove)) {

				move_t prevMove = b->undoHistory[b->ply - 1].move;
				if (pseudoValidBitMove(prevMove)) {
					counterHeuristic[fromSq(prevMove)][toSq(prevMove)][b->side] = currentMove;
				}
			}

			Assert(currentMove != NO_MOVE);
			Assert(abs(beta) <= INF);

			// Store position with bestMove and beta flag
			storeTT(b, currentMove, beta, TT_BETA, depth);
			return beta;
		}

		/*
		* If the currentMove scores higher than bestMove, update score and move.
		* This might be useful when this position is stored into the ttable.
		*/
		if (score > bestScore) {
			bestScore = score;
			bestMove = currentMove;
			Assert(bestScore < INF);

			/*
			* If the currentMove scores higher than alpha, the principal variation
			* is updated because a better move for this position has been found.
			*/
			if (score > alpha) {
				alpha = score;

				//pvLine->line[0] = currentMove;
				//memcpy(pvLine->line + 1, localPV->line, localPV->len * sizeof(currentMove));
				//pvLine->len = localPV->len + 1;
				//Assert(pvLine->len <= MAX_DEPTH);
			}
		}
	}

	Assert(alpha >= oldAlpha);

	// If no moves were possible in this node, position is either a checkmate or stalemate.
	if (legalMoves == 0) {
		if (inCheck) {
			// Checkmate
			return -MATE_VALUE + b->ply;
		} else {
			// Stalemate
			return 0;
		}
	}

	/**
	 * If alpha has been changed, a better and exact score can be stored to transposition table.
	 * Alpha scores are stored, if no move could improve over alpha.
	 */
	if (alpha != oldAlpha) {
		Assert(bestMove != NO_MOVE);
		Assert(abs(bestScore) <= INF);

		storeTT(b, bestMove, bestScore, TT_SCORE, depth);
	} else if (bestMove) {
		Assert(bestMove != NO_MOVE);
		Assert(abs(alpha) <= INF);

		storeTT(b, bestMove, alpha, TT_ALPHA, depth);
	}

	return alpha;
}

int quiescence(int alpha, int beta, int depth, board_t* b, search_t* s, pv_line_t* pvLine) {
	Assert(checkBoard(b));
	selDepth = max(selDepth, b->ply);

	// check for time and depth
	if ((s->nodes & 2047) == 0 || (s->qnodes & 2047) == 0) {
		checkSearchInfo(s);
	}

	s->qnodes++;

	if ((isRepetition(b) || b->fiftyMove >= 100) && b->ply > 0) {
		return contemptFactor(b);
	}

	if (b->ply > MAX_DEPTH - 1) {
		cout << "Warning: MAX_DEPTH reached in quiescence." << endl;
		return eval(b);
	}

	int score = eval(b);
	int standPat = score;
	Assert(abs(standPat) < INF);

	bool inCheck = isCheck(b, b->side);
	quiescenceChecks[abs(depth)] = inCheck;

	/*
	* Stand Pat Pruning:
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
	int bestMove = NO_MOVE;

	moveList_t moveList[1];
	generateQuiescence(b, moveList, inCheck);

	// First ply of quiescence generates checkers, if there is no check at current position.
	if (!inCheck && depth == 0) {
		generateQuietCheckers(b, moveList);
	}

	scoreMoves(b, moveList, NO_MOVE);

	//// Local PV line
	//pv_line_t localPV[1]{};
	//localPV->len = 0;

	/*
	* There are no cutoffs in this node yet. Generate all captures, promotions or check evasions
	* and expand this node.
	*/
	for (int i = 0; i < moveList->cnt; i++) {
		getNextMove(moveList, i);
		int currentMove = moveList->moves[i];

		Assert(moveList->scores[i] >= 0);
		Assert(depth == 0 || inCheck || (currentMove & MCHECK_PROM_OR_CAP || currentMove & MFLAG_EP));
		Assert(currentMove != NO_MOVE);

		/*
		* SEE-Pruning:
		* If SEE score is negative, this is a losing capture. Since there is little to no chance that
		* this move raises alpha, it can be pruned.
		*/
		if (legalMoves
			&& !(MCHECK_PROM & currentMove)
			&& moveList->scores[i] < (BAD_CAPTURE + MVV_LVA_UBOUND)) {
			continue;
		}

		if (!push(b, currentMove)) continue;

		legalMoves++;
		score = -quiescence(-beta, -alpha, depth - 1, b, s, nullptr);
		pop(b);

		if (s->stopped) {
			return 0;
		}

		if (score > alpha) {
			alpha = score;
			bestMove = currentMove;

			//pvLine->line[0] = currentMove;
			//memcpy(pvLine->line + 1, localPV->line, localPV->len * sizeof(currentMove));
			//pvLine->len = localPV->len + 1;
			//Assert(pvLine->len <= MAX_DEPTH);
		}

		/**
		 * Beta cutoff: Move is too good and the opponent already knows a way to defend this attack.
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

	/*
	 * Checkmate Detection:
	 * If this position is checkmate and all positions leading up to this position are check evasions,
	 * this mate is forced. Return standPat, if a non-checking position was found because quiescence
	 * does not generate all moves, when not player is not checked.
	*/
	if (legalMoves == 0 && inCheck) {
		for (int i = depth; i <= 0; i += 2) {
			if (!quiescenceChecks[i]) {
				return score;
			}
		}

		return -MATE_VALUE + b->ply;
	}

	Assert(alpha >= oldAlpha);
	return alpha;
}

void clearForSearch(board_t* b, search_t* s) {
	b->ply = 0;

	// reset stats
	b->tt->probed = 0;
	b->tt->hit = 0;
	b->tt->valueHit = 0;

	// long term stats
	b->tt->collided = 0;
	b->tt->stored = 0;

	// reset mate killer
	for (int i = 0; i < MAX_GAME_MOVES; i++) {
		mateKiller[i] = NO_MOVE;
	}
	// reset killers
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < MAX_DEPTH; j++) {
			killer[i][j] = 0;
		}
	}

	// reset history heuristic
	for (int i = Piece::P; i < Piece::k; i++) {
		for (int j = 0; j < NUM_SQUARES; j++) {
			histHeuristic[i][j] = 0;
		}
	}
	histMax = 0;

	// reset counter move history
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < NUM_SQUARES; j++) {
			for (int k = 0; k < NUM_SQUARES; k++) {
				counterHeuristic[j][Piece::k][i] = NO_MOVE;
			}
		}
	}

	b->pawnTable->collided = 0;
	b->pawnTable->probed = 0;
	b->pawnTable->hit = 0;

	s->startTime = getTimeMs();
	s->stopped = 0;
	s->nodes = 0;
	s->qnodes = 0;
	s->fhf = 0;
	s->fh = 0;
	s->futileFH = 0;
	s->futileCnt = 0;
}

int search_aspiration(board_t* b, search_t* s, int depth, int bestScore) {
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

int search(board_t* b, search_t* s) {
	int bestMove = 0;
	int pvMoves = 0;
	int pvNum = 0;
	int currentDepth = 1;

	clearForSearch(b, s);

	selDepth = 0;
	b->ply = 0;

	int score = alphaBeta(-INF, INF, currentDepth, b, s, DO_NULL, NO_PV, nullptr);
	Assert(abs(score) < INF);

	pvMoves = getPVLine(b, 1);
	bestMove = b->pvArray[0];

	printUCI(s, 1, selDepth, score);
	//printPV(pvLine->line, pvLine->len);
	printTTablePV(b, currentDepth);
	fflush(stdout);
	cout << endl;

	for (currentDepth = 2; currentDepth <= s->depth; currentDepth++) {
		selDepth = 0;
		b->ply = 0;

		score = alphaBeta(-INF, INF, currentDepth, b, s, DO_NULL, IS_PV, nullptr);
		//score = search_aspiration(b, s, currentDepth, score);

		Assert(abs(score) < INF);

		// forced stop, break and use pv line of previous iteration
		if (s->stopped) break;

		pvMoves = getPVLine(b, currentDepth);
		bestMove = b->pvArray[0];

		printUCI(s, currentDepth, selDepth, score);
		//printPV(pvLine->line, pvLine->len);
		printTTablePV(b, currentDepth);
		fflush(stdout);

		//printSearchInfo(b, s);

		if (abs(score) > ISMATE) {
			cout << "\n";
			cout << "bestmove " << getStringMove(bestMove) << "\n";
			return score;
		}

		cout << endl;
		//cout << "Futile moves pruned: \t\t" << s->futileCnt << endl;
	}

	cout << "\n";
	cout << "bestmove " << getStringMove(bestMove) << "\n";
	return score;
}

void printSearchInfo(board_t* b, search_t* s) {
	cout << "\n";
	//cout << "AlphaBeta-Nodes: " << s->nodes << " Piece::Q-Nodes: " << s->qnodes << endl;
	cout << "Ordering percentage: \t\t" << setprecision(4) << fixed << (float)(s->fhf / s->fh) << endl;
	cout << "TTable hits/probed: \t\t" << setprecision(4) << fixed << (float)(b->tt->hit) / (b->tt->probed) << endl;
	cout << "TTable valueHits/hits: \t\t" << setprecision(4) << fixed << (float)(b->tt->valueHit) / (b->tt->hit) << endl;
	cout << "T table memory used: \t\t" << setprecision(4) << fixed << (float)(b->tt->stored) / (b->tt->buckets) << endl;
	cout << "PTable hit percentage: \t\t" << setprecision(4) << fixed << (float)(b->pawnTable->hit) / (b->pawnTable->probed) << endl;
	cout << "PTable memory used: \t\t" << setprecision(4) << fixed << (float)(b->pawnTable->stored) / (b->pawnTable->entries) << endl;
	cout << "PTcollisions: \t\t\t" << setprecision(4) << fixed << b->pawnTable->collided << endl;
	//cout << "Futile failed/tried: \t\t\t" << s->futileFH << "/" << s->futileCnt << endl;
	cout << "Futile moves pruned: \t\t" << s->futileCnt << endl;
	cout << endl;

	//printTTStatus(b);
	cout << endl;
}

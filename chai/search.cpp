#include "search.h"

using namespace chai;


void initSearch() {
	// Calculate MCP depths
	for (int depth = 1; depth < moveCountPruningDepth; depth++) {
		moveCountPruning[depth] = (int)(8.0 + 3.0 * depth * depth / 3.5);
	}
}

void checkSearchInfo(search_t* s) {
	if (s->timeSet && getTimeMs() > s->stopTime) {
		s->stopped = true;
	}

	readInput(s);
}

bool isRepetition(board_t* b) {
	for (int index = max(0, b->undoPly - b->fiftyMove); index < b->undoPly - 1; ++index) {
		Assert(index >= 0 && index < MAX_GAME_MOVES);

		if (b->zobristKey == b->undoHistory[index].zobKey) {
			return true;
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
	if (id1 == id2)
		return;

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
	if (popCount(b->occupied) <= 5) {
		return true;
	}

	// Pawn endgame, no pieces left
	bb = b->pieces[chai::KNIGHT] | b->pieces[chai::BISHOP] | b->pieces[chai::ROOK] | b->pieces[chai::QUEEN];
	if (!bb) {
		return true;
	}

	// No pawns left on the board
	bb = b->pieces[chai::PAWN];
	if (!bb) {
		return true;
	}

	// Knight endgame (only pieces are knight, cannot lose tempo)
	bb = b->pieces[chai::BISHOP] | b->pieces[chai::ROOK] | b->pieces[chai::QUEEN];
	if (!bb) {
		return true;
	}

	// All pawns of one side are blocked (cannot safely advance)
	if (!hasSafePawnPush(b, b->stm)) {
		return true;
	}

	return false;
}

// check if non-pawn material exists on board for given side
static int nonPawnPieces(board_t* b, int side) {
	bitboard_t pieces = getPieces(b, chai::BISHOP, side)
		| getPieces(b, chai::KNIGHT, side)
		| getPieces(b, chai::ROOK, side)
		| getPieces(b, chai::QUEEN, side);

	return popCount(pieces);
}

// check if there are non-king non-pawn pieces on the board
static bool nonPawnPieces(board_t* b) {
	return b->occupied & ~b->pieces[chai::PAWN] & ~b->pieces[chai::KING];
}

// following moves are tactical: caps, proms, eps
static bool moveIsTactical(board_t* b, move_t move) {
	return isCapture(b, move)
		|| isPromotion(move)
		|| isEnPassant(move);
}

// return true if any pawns are on 2nd, resp. 7th, rank
static bool promotablePawns(board_t* b, bool side) {
	if (side == chai::WHITE) {
		return getPieces(b, chai::PAWN, chai::WHITE) & RANK_7_HEX;
	} else {
		return getPieces(b, chai::PAWN, chai::BLACK) & RANK_2_HEX;
	}
}

// check if move pushes pawn on 2nd/7th rank (call AFTER! pushing the move)
static bool dangerousPawnPush(board_t* b, move_t m) {
	if (piecePawn[pieceAt(b, toSq(m))]) {
		if (b->stm == chai::WHITE) {
			return squareToRank[toSq(m)] == RANK_7;
		} else {
			return squareToRank[toSq(m)] == RANK_2;
		}
	}
	return false;
}

// Calculate biggest material swing BEFORE for all possible moves
static value_t biggestMaterialSwing(board_t* b) {

	// Start with roughly 2 pawns to compensate activity
	value_t swing = pieceValues[chai::PAWN] * 2;
	int opponent = b->stm ^ 1;
	int MVPiece = chai::QUEEN;

	// Add value most valuable piece
	for (; MVPiece > chai::NO_TYPE; MVPiece--) {
		if (b->color[opponent] & b->pieces[MVPiece])
			break;
	}
	swing += pieceValues[MVPiece];

	// Add queen value if promotions are possible
	if (promotablePawns(b, b->stm))
		swing += pieceValues[chai::QUEEN] - pieceValues[chai::PAWN];


	return swing;
}

static void updatePvLine(move_t* pv, move_t move, move_t* childPv) {
	Assert(pv && childPv);
	for (*pv++ = move; childPv && *childPv != MOVE_NONE; ) {
		*pv++ = *childPv++;
	}
	*pv = MOVE_NONE;
}


static void resetSearchParameters(board_t* b, search_t* s) {
	b->ply = 0;

	selDepth = 0;

	// Reset stats
	b->tt->probed = 0;
	b->tt->hit = 0;
	b->tt->valueHit = 0;

	// Reset term stats
	b->tt->collided = 0;
	b->tt->stored = 0;

	// Reset mate killer
	for (int i = 0; i < MAX_GAME_MOVES; i++) {
		mateKiller[i] = MOVE_NONE;
	}

	// Reset killers
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < MAX_DEPTH; j++) {
			killer[i][j] = 0;
		}
	}

	// Reset history heuristic
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < NUM_SQUARES; j++) {
			for (int l = 0; l < NUM_SQUARES; l++) {
				histHeuristic[i][j][l] = 0;
			}
		}
	}
	histMax = 0;

	// Reset counter move history
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < NUM_SQUARES; j++) {
			for (int l = 0; l < NUM_SQUARES; l++) {
				counterHeuristic[j][l][i] = MOVE_NONE;
			}
		}
	}

	b->pt->collided = 0;
	b->pt->probed = 0;
	b->pt->hit = 0;

	s->tbHit = 0;

	s->startTime = getTimeMs();
	s->stopped = 0;
	s->nodes = 0;
	s->qnodes = 0;
	s->fhf = 0;
	s->fh = 0;
	s->futileFH = 0;
	s->futileCnt = 0;
}

value_t search(board_t* b, search_t* s) {
	value_t bestScore = VALUE_NONE;

	move_t bestMove = MOVE_NONE;
	move_t pvLine[MAX_DEPTH + 1];

	// Search setup
	resetSearchParameters(b, s);

	// Set up variables used before first recursive call
	searchStack_t* ss = &sStack[b->ply];
	ss->currentMove = MOVE_NONE;
	ss->isCheck = isCheck(b, b->stm);
	ss->staticEval = evaluation(b);
	ss->pvLine = pvLine;

	// Iterative deepening
	int d = 1;
	Assert(s->depth <= MAX_DEPTH);
	while (d <= s->depth && !s->stopped) {

		// Open window for small depths, then aspiration window
		if (d > 3) {
			bestScore = aspirationSearch(b, s, d, bestScore);
		} else {
			bestScore = alphaBeta<PV>(-VALUE_INFTY, VALUE_INFTY, d, b, s);
		}

		Assert(abs(bestScore) < VALUE_INFTY);

		// Use previous bestMove if forced stop mid-search
		if (s->stopped)
			break;

		printUCI(s, d, selDepth, bestScore);
		printPvLine(b, pvLine, d, bestScore);
		cout << endl;

		// Update best move after every complete search iteration
		bestMove = probePV(b);
		Assert(bestMove != MOVE_NONE);

		// Leave IID when mate is found
		if (abs(bestScore) > VALUE_IS_MATE_IN)
			break;

		d++;
	}


	printUCIBestMove(b, bestMove);
	return bestScore;
}

value_t aspirationSearch(board_t* b, search_t* s, int d, value_t bestScore) {
	value_t score;
	int64_t newAlpha, newBeta;
	int researchCnt = 0;
	Assert(abs(bestScore) < VALUE_INFTY);

	// Determine initial alpha/beta values
	int64_t alpha = max(-VALUE_INFTY, bestScore - aspiration);
	int64_t beta = min(VALUE_INFTY, bestScore + aspiration);

	// Research, until score is within bounds
	while (!s->stopped) {

		//cout << "d=" << d << " " << alpha << " " << beta << endl;
		score = alphaBeta<PV>(alpha, beta, d, b, s);

		// Score within bound is returned immediatly
		if (alpha < score && score < beta) {
			return score;
		}

		// Alpha window was too high, decrease
		if (score <= alpha) {
			newAlpha = alpha - aspirationWindows[researchCnt++];

			alpha = max(-VALUE_INFTY, newAlpha);
		}

		// Beta window was too low, increase
		if (score >= beta) {
			newBeta = beta + aspirationWindows[researchCnt++];

			beta = min(VALUE_INFTY, newBeta);
		}
	}

	Assert(abs(score) < VALUE_INFTY);
	return score;
}

template <nodeType_t nodeType>
value_t alphaBeta(value_t alpha, value_t beta, int depth, board_t* b, search_t* s) {
	Assert(checkBoard(b));
	Assert(alpha < beta);

	// Initialize node
	bool rootNode = b->ply == 0;
	bool pvNode = nodeType == PV;
	bool mateThreat = false;
	bool improving = false;
	int searchExt = 0;
	int newDepth = depth - 1;

	value_t value = -VALUE_INFTY;
	value_t bestValue = -VALUE_INFTY;
	value_t posValue;

	move_t pvLine[MAX_DEPTH];
	searchStack_t* ss = &sStack[b->ply];

	Assert(pvNode || (alpha == beta - 1));

	if (insufficientMaterial(b)) {
		return 0;
	}

	if (b->ply >= MAX_DEPTH) {
		return evaluation(b);
	}

	// Check if position is draw at a non-root node
	if (!rootNode) {
		if (isRepetition(b)) {
			return contemptFactor(b);
		}

		// 50-move rule might checkmate / stalemate
		if (b->fiftyMove >= 100) {
			return (b->fiftyMove == 100 && isCheck(b, b->stm) && !hasEvadingMove(b)) ? -VALUE_MATE + b->ply
				: contemptFactor(b);
		}
	}

	// Drop into quiescence if maximum depth is reached
	if (depth <= 0) {
		value_t quietScore = quiescence<nodeType>(alpha, beta, 0, b, s);
		Assert(abs(quietScore) < VALUE_INFTY);

		return quietScore;
	}

	// Early TT prefetch
	prefetchTT(b);

	// check for time and depth
	if ((s->nodes & 2047) == 0) {
		checkSearchInfo(s);
	}
	s->nodes++;

	// Mate Distance Pruning:
	// If alpha or beta are already mate scores, bounds can be adjusted to prune irrelevant subtrees.
	// Mates are delivered faster, but does not speed up search in general.
	if (!rootNode) {
		alpha = max(alpha, (value_t)(-VALUE_MATE + b->ply));
		beta = min(beta, (value_t)(VALUE_MATE - b->ply - 1));
		if (alpha >= beta)
			return alpha;
	}

	// Transposition Table Probing:
	// Probe the TTable and look for useful information from previous transpositions. Return hashScore if hash table
	// stored a better score at same or greater depth. Do not return if close to 50-move draw.
	value_t hashValue = -VALUE_INFTY;
	value_t hashEval = -VALUE_INFTY;
	int8_t hashDepth = -1;
	uint8_t hashFlag = TT_NONE;
	move_t hashMove = MOVE_NONE;

	bool hashStored = probeTT(b, &hashMove, &hashValue, &hashEval, &hashFlag, &hashDepth);
	if (hashStored
		&& !pvNode
		&& !(hashFlag & TT_EVAL)) {
		b->tt->hit++;

		// Look for valueHit: This position has already been searched before with greater depth.
		if (hashDepth >= depth && b->fiftyMove < 90) {
			Assert(hashDepth >= QS_DEPTH && hashDepth <= MAX_DEPTH);
			Assert(hashFlag >= TT_ALPHA && hashFlag <= TT_VALUE);
			Assert(hashValue > -VALUE_INFTY && hashValue < VALUE_INFTY);

			if (hashFlag & TT_VALUE) {
				b->tt->valueHit++;
				return hashValue;
			}

			if (hashFlag & TT_BETA && hashValue >= beta) {
				//if (hashFlag & TT_BETA && hashValue <= alpha) {
				b->tt->valueHit++;
				return hashValue;
			}

			if (hashFlag & TT_ALPHA && hashValue <= alpha) {
				//if (hashFlag & TT_ALPHA && hashValue >= beta) {
				b->tt->valueHit++;
				return hashValue;
			}

		}
	}

	// EGTB:
	// Endgame-Tablebase probing.
	int tbFlag;
	int tbResult = probeTB(b);
	if (tbResult != TB_RESULT_FAILED) {
		s->tbHit++;

		value_t tbValue = tbResult == TB_LOSS ? -VALUE_TB_WIN + b->ply
			: tbResult == TB_WIN ? VALUE_TB_WIN - b->ply : 0;

		tbFlag = tbResult == TB_LOSS ? TT_BETA
			: tbResult == TB_WIN ? TT_ALPHA : TT_VALUE;

		if (tbFlag == TT_VALUE
			|| (tbFlag == TT_ALPHA && tbValue >= beta)
			|| (tbFlag == TT_BETA && tbValue <= alpha)) {

			Assert(abs(searchToHash(b, tbValue)) != VALUE_INFTY);
			storeTT(b, MOVE_NULL, searchToHash(b, tbValue), VALUE_NONE, tbFlag, depth);
			return tbValue;
		}
	}

	// Static evaluation of position:
	// Either use previos evaluations from TT hit / null-move or do calculate evaluation score.
	if (hashStored && abs(hashEval) < VALUE_IS_MATE_IN) {
		ss->staticEval = posValue = hashEval;

		// Try to improve static evaluation with hashValue from TT hit
		if (hashFlag & TT_ALPHA && hashValue > ss->staticEval)
			posValue = hashValue;

		if (hashFlag & TT_BETA && hashValue <= ss->staticEval)
			posValue = hashValue;

	} else if (ss->currentMove == MOVE_NULL) {
		ss->staticEval = posValue = -(ss - 1)->staticEval;

	} else {
		ss->staticEval = posValue = evaluation(b);
		//storeTT(b, NULL_MOVE, NO_VALUE, ss->staticEval, TT_EVAL, MAX_DEPTH);
	}
	Assert(ss->staticEval != VALUE_NONE && abs(ss->staticEval) < VALUE_IS_MATE_IN);
	//Assert(ss->currentMove == getCurrentMove(b));

	// Basic check extension
	bool inCheck = ss->isCheck;
	Assert3(inCheck == isCheck(b, b->stm), std::to_string(b->ply), getFEN(b));
	if (inCheck) {
		newDepth++;
	}

	if (!inCheck && b->ply > 2) {
		improving = !(ss - 2)->isCheck ?
			ss->staticEval > (ss - 2)->staticEval
			: ((b->ply > 4) ?
			   ss->staticEval > (ss - 4)->staticEval
			   : false);
	}

	// Reverse Futility Pruning:
	// Similar idea like NMP, but eval-margin is acting as the lower bound.
	if (!pvNode
		&& !inCheck
		&& depth < 5
		&& nonPawnPieces(b)
		//&& !promotablePawns(b, b->side ^ 1)
		&& posValue <= VALUE_WIN) {

		int margin = (depth - improving) * (pieceValues[chai::BISHOP] - 38);
		if (posValue - margin >= beta) {
			return beta;
		}
	}

	// Null Move Pruning:
	// Give opposite side a free move and use nullScore as lower bound.
	// If this position is still winning, e.g. fails high, it will never be reached.
	// Restrict NMP to reasonable positions (zugzwang, depth, checks).
	if (ss->currentMove != MOVE_NULL
		&& !inCheck
		&& !pvNode
		&& depth > 2
		//&& ss->staticEval >= beta
		&& posValue >= beta
		&& abs(beta) < VALUE_IS_MATE_IN
		&& nonPawnPieces(b, b->stm)
		&& popCount(b->occupied) > 7) {

		int reduction = (depth > 6) ? 3 : 2;

		(ss + 1)->isCheck = false;
		(ss + 1)->currentMove = MOVE_NULL;
		pushNull(b);
		value_t nullValue = -alphaBeta<NoPV>(-beta, -beta + 1, depth - 1 - reduction, b, s);
		popNull(b);

		if (s->stopped) {
			return 0;
		}

		// Even after giving opponent free move, position is winning..
		if (nullValue >= beta && nullValue < VALUE_IS_MATE_IN) {

			// nullScore acts as lower bound to this position
			if (depth - 1 - reduction > 0) {
				storeTT(b, MOVE_NULL, searchToHash(b, nullValue),
						ss->staticEval, TT_ALPHA, depth - 1 - reduction);
			}
			return nullValue;
		}

		// Mate threat detection
		if (nullValue < -VALUE_IS_MATE_IN) {
			mateThreat = true;
		}
	}

	// "Internal Iterative Deepening":
	// If ttable probing does not find a hash move, there is no good move to start searching this
	// position. Reduces these searches as replacement for IID.
	if (pvNode
		&& depth >= 6
		&& hashMove == MOVE_NONE) {
		newDepth--;
	}

	moveList_t moveList;
	generateMoves(b, &moveList, inCheck);
	scoreMoves(b, &moveList, hashMove);

	move_t currentMove = MOVE_NONE;
	move_t bestMove = MOVE_NONE;
	int moveNum = 0;
	int legalMoves = 0;
	int oldAlpha = alpha;

	// This position could not be refuted yet. All moves are generated
	// and searching continues.
	for (int i = 0; i < moveList.cnt; i++) {

		Assert(currentMove != moveList.moves[i]);

		getNextMove(&moveList, i);
		currentMove = moveList.moves[i];

		Assert(currentMove != MOVE_NONE);
		Assert(moveList.scores[i] >= 0);

		int lmrDepth = MAX_DEPTH + 1;
		bool lmrSameDepth = false;

		bool moveGivesCheck = checkingMove(b, currentMove);
		int moveCaptured = isCapture(b, currentMove);

		if (pvNode)
			(ss + 1)->pvLine = nullptr;

		// Futiilty Pruning
		if (!rootNode
			&& legalMoves
			&& depth < 5
			&& abs(alpha) < VALUE_IS_MATE_IN
			&& abs(beta) < VALUE_IS_MATE_IN
			&& bestValue > -VALUE_IS_MATE_IN
			&& nonPawnPieces(b, b->stm)
			&& !isPromotion(currentMove)) {

			// Move count based pruning
			if (!pvNode
				&& !inCheck
				&& !moveGivesCheck
				&& depth < moveCountPruningDepth
				&& legalMoves > moveCountPruning[depth]
				&& !moveCaptured) {
				continue;
			}

			// Futile captures / checks: bad SEE score
			if (moveCaptured || moveGivesCheck) {

				if (!see_ge(b, currentMove, -depth * (SEEPieceValues[chai::BISHOP]))) {
					continue;
				}
			}

			// Futile quiet moves: cannot raise alpha
			if (!inCheck
				&& !mateThreat
				&& !moveGivesCheck
				&& !moveIsTactical(b, currentMove)) {

				if (ss->staticEval + (depth * pieceValues[chai::BISHOP] + 34) < alpha) {
					continue;
				}

				// Higher histScore by a move <=> Higher chance the move can raise alpha
				int histScore = histHeuristic[b->stm][fromSq(currentMove)][toSq(currentMove)];

				// The lower the histScore, the higher the scaled score
				// => bad moves score near histmax and good moves score near 0
				int scaledHistScore = histMax / (histScore ? histScore : 1);
				bool specialQuiet = moveList.scores[i] > COUNTER_SCORE;

				if (ss->staticEval + (depth * 234) + (specialQuiet * 50) < alpha
					&& scaledHistScore > 2 * histMax / 3) {
					continue;
				}
			}
		}

		// Push the move on the board
		(ss + 1)->isCheck = moveGivesCheck;
		(ss + 1)->currentMove = currentMove;
		if (!push(b, currentMove)) {
			Assert(!inCheck); // Position cannot be check: all check evasions are legal
			continue;
		}

		if (rootNode
			&& getTimeMs() > s->startTime + 750
			) {
			cout << "info"
				<< " depth " << depth
				<< " currmove " << getStringMove(b, currentMove)
				<< " currmovenumber " << legalMoves << endl;
		}

		legalMoves++;

		// PVS / LMR:
		// Always assume that the first move is part of the principal variation and scores in
		// alpha - beta bounds. Late moves wont raise alpha in most cases, try to prove this
		// with reduced search around alpha. If the move improves alpha, a research
		// is required.
		if (legalMoves == 1) {

			(ss + 1)->pvLine = pvLine;
			(ss + 1)->pvLine[0] = MOVE_NONE;

			value = -alphaBeta<PV>(-beta, -alpha, newDepth, b, s);

		} else {
			// Late Move Reductions, do reduced zero window search
			if (depth > 3
				&& legalMoves > 2 + rootNode
				&& !isPromotion(currentMove)
				) {

				lmrDepth = newDepth - 2;

				// Increase reduction for late moves
				if (legalMoves > 6)
					lmrDepth--;

				if (!moveGivesCheck
					&& ((ss->staticEval + pieceValues[moveCaptured] + depth * 243 < alpha)
						|| (!pvNode && !improving)))
					lmrDepth--;

				if (mateThreat)
					lmrDepth++;

				if (moveGivesCheck)
					lmrDepth++;

				if (inCheck)
					lmrDepth++;

				if (inCheck && pieceKing[pieceAt(b, toSq(currentMove))])
					lmrDepth++;

				if (!pvNode && moveList.scores[i] < KILLER_SCORE_2)
					lmrDepth--;

				if (pvNode && dangerousPawnPush(b, currentMove))
					lmrDepth++;

				// Minimum depth is 1 and maximum depth is newDepth
				lmrDepth = min(newDepth, max(1, lmrDepth));
				lmrSameDepth = newDepth == lmrDepth;

				value = -alphaBeta<NoPV>(-(alpha + 1), -alpha, lmrDepth, b, s);
			} else {
				// Ensure, that PVS is always exexcuted if LMR is not applicable
				value = alpha + 1;
			}

			// Principal Variation Search
			if (value > alpha) {

				// Do not research with same depth as LMR
				if (!lmrSameDepth)
					value = -alphaBeta<NoPV>(-(alpha + 1), -alpha, newDepth, b, s);

				if (value > alpha && (rootNode || value < beta)) {
					(ss + 1)->pvLine = pvLine;
					(ss + 1)->pvLine[0] = MOVE_NONE;

					value = -alphaBeta<PV>(-beta, -alpha, newDepth, b, s);
				}
			}
		}

		// Pop move from board
		pop(b);

		if (s->stopped) {
			return 0;
		}

		// Alpha-Beta Pruning:
		// If score is greater than beta, the move is too good. The opponent already has a line that prevents
		// this move and therefore it can be pruned.
		if (value >= beta) {
			// stats: count fail-high percentage on first move in node
			if (legalMoves == 1) {
				s->fhf++;
			}
			s->fh++;

			bool moveIsCapOrPromo = moveCaptured || isPromotion(currentMove);

			// Standard Killer Heuristic:
			// Moves that caused a beta cutoff are likely to be good in other positions in the same ply.
			// These moves are stored and considered in move ordering. 
			if (!inCheck
				&& !moveIsCapOrPromo
				&& killer[0][b->ply] != currentMove) {

				killer[1][b->ply] = killer[0][b->ply];
				killer[0][b->ply] = currentMove;
				Assert(killer[1][b->ply] != killer[0][b->ply]);
			}

			// Mate Killers:
			// If a move scores near checkmate, order it above standard killers.
			if (value >= VALUE_IS_MATE_IN) {
				mateKiller[b->ply] = currentMove;
			}

			// History Heuristic:
			// Color-From-To information is stored for each side. Do not apply, if capture, promoting or
			// in check (already considered in move ordering). Remember maximum history score and
			// rescale if needed to prevent overflow.
			if (!inCheck
				&& !moveIsCapOrPromo) {
				int from = fromSq(currentMove);
				int to = toSq(currentMove);

				histHeuristic[b->stm][from][to] += depth * depth;
				histMax = max(histHeuristic[b->stm][from][to], histMax);

				if (histMax >= HISTORY_MAX) {
					for (int c = 0; c < 2; c++) {
						for (int i = 0; i < NUM_SQUARES; i++) {
							for (int j = 0; j < NUM_SQUARES; j++) {
								histHeuristic[c][i][j] >>= 3;
							}
						}
					}
					histMax >>= 3;
					Assert(histMax < HISTORY_MAX);
				}
			}

			// Counter-Move Heuristic:
			// If currentMove caused a beta cutoff, store previous move FROM and TO
			// and assign currentMove as an intuitive counter move for consideration in
			// move ordering.
			if (!inCheck
				&& b->ply > 0
				&& !moveIsCapOrPromo
				&& pseudoValidBitMove(currentMove)) {

				move_t prevMove = b->undoHistory[b->ply - 1].move;
				if (pseudoValidBitMove(prevMove)) {
					counterHeuristic[fromSq(prevMove)][toSq(prevMove)][b->stm] = currentMove;
				}
			}

			Assert(currentMove != MOVE_NONE);
			Assert(abs(beta) <= VALUE_INFTY);

			// Store position with bestMove and beta flag
			storeTT(b, currentMove, searchToHash(b, beta), ss->staticEval, TT_BETA, depth);
			return beta;
		}

		// If the currentMove scores higher than bestMove, update score and move.
		// This might be useful when this position is stored into the ttable.
		if (value > bestValue) {
			bestValue = value;
			bestMove = currentMove;
			Assert(bestValue < VALUE_INFTY);

			// If the currentMove scores higher than alpha, the principal variation
			// is updated because a better move for this position has been found.
			if (value > alpha) {
				alpha = value;

				if (pvNode)
					updatePvLine(ss->pvLine, bestMove, (ss + 1)->pvLine);

			}
		}
	}

	Assert(alpha >= oldAlpha);

	// If no moves were possible in this node, position is either a checkmate or stalemate.
	if (legalMoves == 0) {
		if (inCheck) {
			// Checkmate
			return -VALUE_MATE + b->ply;
		} else {
			// Stalemate
			return 0;
		}
	}

	// If alpha has been changed, a better and exact score can be stored to transposition table.
	// Alpha scores are stored, if no move could improve over alpha.
	if (alpha != oldAlpha) {
		Assert(bestMove != MOVE_NONE);
		Assert(abs(bestValue) <= VALUE_INFTY);

		storeTT(b, bestMove, searchToHash(b, bestValue), ss->staticEval, TT_VALUE, depth);
	} else if (bestMove) {
		Assert(bestMove != MOVE_NONE);
		Assert(abs(alpha) <= VALUE_INFTY);

		storeTT(b, bestMove, searchToHash(b, alpha), ss->staticEval, TT_ALPHA, depth);
	}

	return alpha;
}

template <nodeType_t nodeType>
value_t quiescence(value_t alpha, value_t beta, int depth, board_t* b, search_t* s) {
	Assert(checkBoard(b));

	// Early TT prefetch 
	prefetchTT(b);

	// check for time and depth
	if ((s->nodes & 2047) == 0 || (s->qnodes & 2047) == 0) {
		checkSearchInfo(s);
	}

	s->qnodes++;
	selDepth = max(selDepth, b->ply);

	bool pvNode = nodeType == PV;
	move_t currPvLine[MAX_DEPTH + 1];
	searchStack_t* ss = &sStack[b->ply];

	if (pvNode) {
		(ss + 1)->pvLine = currPvLine;
		ss->pvLine[0] = MOVE_NONE;
	}

	// Draw detection
	if (isRepetition(b) || insufficientMaterial(b)) {
		return contemptFactor(b);
	} else if (b->fiftyMove >= 100) {
		// 50-move rule might checkmate / stalemate
		return (b->fiftyMove == 100 && isCheck(b, b->stm) && !hasEvadingMove(b)) ? -VALUE_MATE + b->ply
			: contemptFactor(b);
	}

	if (b->ply > MAX_DEPTH - 1) {
		return evaluation(b);
	}

	//bool inCheck = ss->isCheck;
	//bool inCheck = (depth == 0) ? ss->isCheck : isCheck(b, b->stm);
	bool inCheck = isCheck(b, b->stm);
	int ttDepth = (inCheck) ? QS_DEPTH_CHECK : QS_DEPTH;
	Assert(inCheck == isCheck(b, b->stm));

	// Transposition Table:
	// Probe transposition table and try to cutoff, if this node is non-pv.
	move_t hashMove = MOVE_NONE;
	value_t hashValue = -VALUE_INFTY;
	value_t hashEval = -VALUE_INFTY;
	uint8_t hashFlag = TT_NONE;
	int8_t hashDepth;

	bool hashStored = probeTT(b, &hashMove, &hashValue, &hashEval, &hashFlag, &hashDepth);
	if (hashStored && !pvNode) {
		b->tt->hit++;

		Assert(hashDepth <= MAX_DEPTH);
		Assert(hashFlag >= TT_ALPHA && hashFlag <= TT_VALUE);
		Assert(hashValue > -VALUE_INFTY && hashValue < VALUE_INFTY);

		if (hashDepth >= ttDepth) {

			if (hashFlag & TT_VALUE) {
				b->tt->valueHit++;
				return hashValue;
			}

			if (hashFlag & TT_BETA && beta <= hashValue) {
				b->tt->valueHit++;
				return hashValue;
			}

			if (hashFlag & TT_ALPHA && alpha >= hashValue) {
				b->tt->valueHit++;
				return hashValue;
			}
		}
	}

	// Static evaluation of the position
	if (hashStored && abs(hashEval) < VALUE_IS_MATE_IN) {
		ss->staticEval = hashEval;

	} else if (ss->currentMove == MOVE_NULL) {
		ss->staticEval = -(ss - 1)->staticEval;

	} else {
		ss->staticEval = evaluation(b);
	}
	Assert(abs(ss->staticEval) < VALUE_INFTY);

	// Stand Pat Pruning:
	// Side to move can reject a recapture, if static evaluation is already good enough.
	// Stand pat is not valid when in check, because this could result in misleading
	// evaluation in checkmate or stalemate position.
	value_t value;
	value_t bestValue = -VALUE_INFTY;

	if (!inCheck) {
		bestValue = ss->staticEval;
		if (bestValue >= beta) {
			return bestValue;
		}

		if (bestValue > alpha) {
			alpha = bestValue;
		}
	}

	// Delta Pruning:
	// If biggest possible positional material swing cannot improve alpha, 
	// this position is hopeless. 
	if (ss->staticEval + biggestMaterialSwing(b) <= alpha) {
		return alpha;
	}

	int legalMoves = 0;
	value_t oldAlpha = alpha;
	move_t bestMove = MOVE_NONE;

	moveList_t moveList;
	generateQuiescence(b, &moveList, inCheck);

	// Checkmate Detection:
	// Quiescence generates check evasions. If in check and no evading moves possible, 
	// return: this is mate.
	if (moveList.cnt == 0 && inCheck) {
		// Checkmate
		return -VALUE_MATE + b->ply;
	}

	// First ply of quiescence generates checkers, if there is no check at current position.
	if (!inCheck && depth == 0) {
		generateQuietCheckers(b, &moveList);
	}

	scoreMoves(b, &moveList, hashMove);

	// There are no cutoffs in this node yet. Loop through all captures, promotions or check evasions
	// and expand search.
	for (int i = 0; i < moveList.cnt; i++) {

		getNextMove(&moveList, i);
		move_t currentMove = moveList.moves[i];
		Assert(currentMove != MOVE_NONE);
		Assert(moveList.scores[i] >= 0);

		// SEE-Pruning:
		// If SEE score is negative, this is a losing capture. Since there is little to no chance that
		// this move raises alpha, it can be pruned.
		if (bestValue > -VALUE_IS_MATE_IN
			&& moveList.scores[i] < BAD_CAPTURE + MVV_LVA_UBOUND) {
			continue;
		}

		//(ss + 1)->isCheck = moveGivesCheck;
		(ss + 1)->currentMove = currentMove;
		if (!push(b, currentMove)) {
			Assert(!inCheck); // Position cannot be check because all check evasions are legal
			continue;
		}

		legalMoves++;
		value = -quiescence<nodeType>(-beta, -alpha, depth - 1, b, s);
		pop(b);

		if (s->stopped) {
			return 0;
		}

		if (value > bestValue) {
			bestValue = value;
			bestMove = currentMove;

			if (value > alpha) {
				alpha = value;

				if (pvNode)
					updatePvLine(ss->pvLine, bestMove, (ss + 1)->pvLine);
			}
		}

		// Beta cutoff: 
		// Move is too good and the opponent already knows a way to defend this attack.
		if (value >= beta) {
			// stats
			if (legalMoves == 1) {
				s->fhf++;
			}
			s->fh++;

			storeTT(b, currentMove, searchToHash(b, beta), ss->staticEval, TT_BETA, ttDepth);
			return beta;
		}
	}

	if (alpha > oldAlpha) {
		Assert(bestMove != MOVE_NONE);
		Assert(abs(bestValue) <= VALUE_INFTY);
		storeTT(b, bestMove, searchToHash(b, bestValue), ss->staticEval, TT_VALUE, ttDepth);
	} else if (bestMove) {
		Assert(bestMove != MOVE_NONE);
		Assert(abs(alpha) <= VALUE_INFTY);
		storeTT(b, bestMove, searchToHash(b, alpha), ss->staticEval, TT_ALPHA, ttDepth);
	}

	Assert(alpha >= oldAlpha);
	Assert(abs(bestValue) < VALUE_INFTY);

	return bestValue;
}

void printSearchInfo(board_t* b, search_t* s) {
	cout << "\n";
	//cout << "AlphaBeta-Nodes: " << s->nodes << " Q-Nodes: " << s->qnodes << endl;
	cout << "Ordering percentage: \t\t" << std::setprecision(4) << fixed << (float)(s->fhf / s->fh) << endl;
	cout << "TTable hits/probed: \t\t" << std::setprecision(4) << fixed << (float)(b->tt->hit) / (b->tt->probed) << endl;
	cout << "TTable valueHits/hits: \t\t" << std::setprecision(4) << fixed << (float)(b->tt->valueHit) / (b->tt->hit) << endl;
	cout << "T table memory used: \t\t" << std::setprecision(4) << fixed << (float)(b->tt->stored) / (b->tt->buckets) << endl;
	cout << "PTable hit percentage: \t\t" << std::setprecision(4) << fixed << (float)(b->pt->hit) / (b->pt->probed) << endl;
	cout << "PTable memory used: \t\t" << std::setprecision(4) << fixed << (float)(b->pt->stored) / (b->pt->entries) << endl;
	cout << "PTcollisions: \t\t\t" << std::setprecision(4) << fixed << b->pt->collided << endl;
	//cout << "Futile failed/tried: \t\t\t" << s->futileFH << "/" << s->futileCnt << endl;
	cout << "Futile moves pruned: \t\t" << s->futileCnt << endl;
	cout << endl;

	//printTTStatus(b);
	cout << endl;
}

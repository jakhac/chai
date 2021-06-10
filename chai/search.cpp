#include "search.h"

void initSearch() {
	// Calculate MCP depths
	for (int depth = 1; depth < moveCountPruningDepth; depth++) {
		moveCountPruning[depth] = (int)(8.0 + 3.0 * depth * depth / 3.5);
		//cout << "d=" << depth << " " << moveCountPruning[depth] << endl;
	}
}

void checkSearchInfo(search_t* s) {
	if (s->timeSet && getTimeMs() > s->stopTime) {
#ifdef INFO
		cout << "Forced search quit (set time is up)" << endl;
#endif // !INFO
		s->stopped = true;
	}

	// read input causes tests to wait for cin and does not terminate
#ifndef TESTING
	readInput(s);
#endif // TESTING
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
	if (!hasSafePawnPush(b, b->stm)) {
		return true;
	}

	return false;
}

// check if non-pawn material exists on board for given side
static int nonPawnPieces(board_t* b, int side) {
	bitboard_t pieces = getPieces(b, Piece::BISHOP, side)
		| getPieces(b, Piece::KNIGHT, side)
		| getPieces(b, Piece::ROOK, side)
		| getPieces(b, Piece::QUEEN, side);

	return countBits(pieces);
}

// check if there are non-king non-pawn pieces on the board
static bool nonPawnPieces(board_t* b) {
	return b->occupied & ~b->pieces[Piece::PAWN] & ~b->pieces[Piece::KING];
}

// following moves are tactical: caps, proms, eps
static bool moveIsTactical(board_t* b, move_t move) {
	return isCapture(b, move)
		|| isPromotion(move)
		|| isEnPassant(move);
}

// return true if any pawns are on 2nd, resp. 7th, rank
static bool promotablePawns(board_t* b, bool side) {
	if (side == WHITE) {
		return getPieces(b, Piece::PAWN, WHITE) & RANK_7_HEX;
	} else {
		return getPieces(b, Piece::PAWN, BLACK) & RANK_2_HEX;
	}
}

// check if move pushes pawn on 2nd/7th rank (call AFTER! pushing the move)
static bool dangerousPawnPush(board_t* b, move_t m) {
	if (piecePawn[pieceAt(b, toSq(m))]) {
		if (b->stm == WHITE) {
			return squareToRank[toSq(m)] == RANK_7;
		} else {
			return squareToRank[toSq(m)] == RANK_2;
		}
	}
	return false;
}

static void updatePvLine(move_t* pv, move_t move, move_t* childPv) {
	Assert(pv && childPv);
	for (*pv++ = move; childPv && *childPv != NO_MOVE; ) {
		*pv++ = *childPv++;
	}
	*pv = NO_MOVE;
}

template <nodeType_t nodeType>
value_t alphaBeta(value_t alpha, value_t beta, int depth, board_t* b, search_t* s) {
	Assert(checkBoard(b));
	Assert(alpha < beta);

	// TODO 2.5.5
	// Code cleanup:
	// clean repo: googletest name, vcxproj issue

	// Initialize node
	bool rootNode = b->ply == 0;
	bool pvNode = nodeType == PV;
	bool mateThreat = false;
	bool improving = false;
	int searchExt = 0;
	int newDepth = depth - 1;

	move_t pvLine[MAX_DEPTH];
	searchStack_t* ss = &sStack[b->ply];

	Assert(pvNode || (alpha == beta - 1));

	// Check if position is immediate draw at a non-root node
	if (!rootNode) {

		if (isRepetition(b) || insufficientMaterial(b)) {
			return contemptFactor(b);
		}

		// 50-move rule might checkmate / stalemate
		if (b->fiftyMove >= 100) {
			return (b->fiftyMove == 100 && isCheck(b, b->stm) && !hasEvadingMove(b)) ? -MATE_VALUE + b->ply
				: contemptFactor(b);
		}
	}

	// Drop into quiescence if maximum depth is reached
	if (depth <= 0 || b->ply > MAX_DEPTH) {
		value_t quietScore = quiescence<nodeType>(alpha, beta, 0, b, s);
		Assert(abs(quietScore) < INF);

		return quietScore;
	}

	// Early TT prefetch
	prefetchTTEntry(b);

	// check for time and depth
	if ((s->nodes & 2047) == 0) {
		checkSearchInfo(s);
	}
	s->nodes++;

	// Mate Distance Pruning:
	// If alpha or beta are already mate scores, bounds can be adjusted to prune irrelevant subtrees.
	// Mates are delivered faster, but does not speed are search.
	if (!rootNode) {
		alpha = max(alpha, (value_t)(-MATE_VALUE + b->ply));
		beta = min(beta, (value_t)(MATE_VALUE - b->ply - 1));
		if (alpha >= beta)
			return alpha;
	}

	// Transposition Table Probing:
	// Probe the TTable and look for useful information from previous transpositions. Return hashScore if hash table
	// stored a better score at same or greater depth. Do not return if close to 50-move draw.
	value_t hashValue = -INF;
	value_t hashEval = -INF;
	int hashDepth = -1;
	uint8_t hashFlag = TT_NONE;
	move_t hashMove = NO_MOVE;

	bool hashStored = probeTT(b, &hashMove, &hashValue, &hashEval, &hashFlag, &hashDepth);
	bool ttCapture = hashStored && isCapture(b, hashMove);
	if (hashStored
		&& !pvNode
		&& !(hashFlag & TT_EVAL)) {
		b->tt->hit++;

		// Look for valueHit: This position has already been searched before with greater depth.
		if (hashDepth >= depth && b->fiftyMove < 90) {
			Assert(hashDepth >= 1 && hashDepth <= MAX_DEPTH);
			Assert(hashFlag >= TT_ALPHA && hashFlag <= TT_VALUE);
			Assert(hashValue > -INF && hashValue < INF);

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

	// Static evaluation of position:
	// Either use previos evaluations from TT hit / null-move or do calculate evaluation score.
	value_t value = -INF;
	value_t bestValue = -INF;
	value_t posValue;

	if (hashStored) {
		Assert(abs(hashEval) < ISMATE);
		ss->staticEval = posValue = hashEval;

		// Try to improve static evaluation with hashValue from TT hit
		if (hashFlag & TT_ALPHA && hashValue > ss->staticEval)
			posValue = hashValue;

		if (hashFlag & TT_BETA && hashValue <= ss->staticEval)
			posValue = hashValue;

	} else if (ss->currentMove == NULL_MOVE) {
		ss->staticEval = posValue = -(ss - 1)->staticEval;

	} else {
		ss->staticEval = posValue = evaluation(b);
		//storeTT(b, NULL_MOVE, NO_VALUE, ss->staticEval, TT_EVAL, MAX_DEPTH);
	}
	Assert(ss->staticEval != NO_VALUE && abs(ss->staticEval) < ISMATE);
	Assert(ss->currentMove == getCurrentMove(b));

	// Basic check extension
	bool inCheck = ss->isCheck;
	Assert(inCheck == isCheck(b, b->stm));
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
		&& posValue <= 10000) {

		int margin = (depth - improving) * (pieceValues[Piece::BISHOP] - 38);
		if (posValue - margin >= beta) {
			return beta;
		}
	}

	// Null Move Pruning:
	// Give opposite side a free move and use nullScore as lower bound (alpha).
	// If this position is still winning, e.g. fails high, it will never be reached.
	// Restrict NMP to reasonable positions (zugzwang, depth, checks).
	if (ss->currentMove != NULL_MOVE
		&& !inCheck
		&& !pvNode
		&& depth > 2
		//&& ss->staticEval >= beta
		&& posValue >= beta
		&& abs(beta) < ISMATE
		&& nonPawnPieces(b, b->stm)
		&& countBits(b->occupied) > 7) {

		int reduction = (depth > 6) ? R_3 : R_2;

		(ss + 1)->isCheck = false;
		(ss + 1)->currentMove = NULL_MOVE;
		pushNull(b);
		value_t nullValue = -alphaBeta<NoPV>(-beta, -beta + 1, depth - 1 - reduction, b, s);
		pop(b);

		if (s->stopped) {
			return 0;
		}

		// Even after giving opponent free move, position is winning..
		if (nullValue >= beta && nullValue < ISMATE) {

			// nullScore acts as lower bound to this position
			if (depth - 1 - reduction > 0) {
				storeTT(b, NULL_MOVE, searchToHash(b, nullValue), ss->staticEval, TT_ALPHA, depth - 1 - reduction);
			}
			return nullValue;
		}

		// Mate threat detection
		if (nullValue < -ISMATE) {
			mateThreat = true;
		}
	}

	// "Internal Iterative Deepening":
	// If ttable probing does not find a hash move, there is no good move to start searching this
	// position. Reduces these searches as replacement for IID.
	if (pvNode
		&& depth >= 6
		&& hashMove == NO_MOVE) {
		newDepth--;
	}

	moveList_t moveList[1];
	generateMoves(b, moveList, inCheck);
	scoreMoves(b, moveList, hashMove);

	move_t currentMove;
	move_t bestMove = NO_MOVE;
	int moveNum = 0;
	int legalMoves = 0;
	int oldAlpha = alpha;

	// This position could not be refuted yet. Therefore, moves are generated
	// and searching continues.
	for (int i = 0; i < moveList->cnt; i++) {

		getNextMove(moveList, i);
		currentMove = moveList->moves[i];
		Assert(currentMove != NO_MOVE);
		Assert(moveList->scores[i] >= 0);

		if (rootNode && getTimeMs() > s->startTime + 750) {
			cout << "info"
				<< " depth " << depth
				<< " currmove " << getStringMove(b, currentMove)
				<< " currmovenumber " << legalMoves << endl;
		}

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
			&& abs(alpha) < ISMATE
			&& abs(beta) < ISMATE
			&& bestValue > -ISMATE
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

				if (!see_ge(b, currentMove, -depth * (SEEPieceValues[Piece::BISHOP]))) {
					continue;
				}
			}

			// Futile quiet moves: cannot raise alpha
			if (!inCheck
				&& !mateThreat
				&& !moveGivesCheck
				&& !moveIsTactical(b, currentMove)) {

				if (ss->staticEval + (depth * pieceValues[Piece::BISHOP] + 34) < alpha) {
					continue;
				}

				// Higher histScore by a move <=> Higher chance the move can raise alpha
				int histScore = histHeuristic[b->stm][fromSq(currentMove)][toSq(currentMove)];

				// The lower the histScore, the higher the scaled score
				// => bad moves score near histmax and good moves score near 0
				int scaledHistScore = histMax / (histScore ? histScore : 1);
				bool specialQuiet = moveList->scores[i] > COUNTER_SCORE;

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

		legalMoves++;

		// PVS / LMR:
		// Always assume that the first move is part of the principal variation and scores in
		// alpha - beta bounds. Late moves wont raise alpha in most cases, try to prove this
		// with reduced search around alpha. If the move improves alpha, a research
		// is required.
		if (legalMoves == 1) {

			(ss + 1)->pvLine = pvLine;
			(ss + 1)->pvLine[0] = NO_MOVE;

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

				if (!pvNode && moveList->scores[i] < KILLER_SCORE_2)
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
					(ss + 1)->pvLine[0] = NO_MOVE;

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
			if (value >= ISMATE) {
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

				if (histMax > HISTORY_MAX) {
					for (int c = 0; c < 2; c++) {
						for (int i = 0; i < NUM_SQUARES; i++) {
							for (int j = 0; j < NUM_SQUARES; j++) {
								histHeuristic[c][i][j] >>= 2;
							}
						}
					}
					histMax >>= 2;
				}
			}

			// Counter-Move Heuristic:
			// If currentMove caused a beta cutoff, store previous move FROM and TO
			// and assign currentMove as an intuitive counter move for consideration in
			// move ordering. Counter moves are not stored if in check or currentMove is
			// a capture or promotion (already considered in move ordering). Only possible if ply > 0.
			if (!inCheck
				&& b->ply > 0
				&& !moveIsCapOrPromo
				&& pseudoValidBitMove(currentMove)) {

				move_t prevMove = b->undoHistory[b->ply - 1].move;
				if (pseudoValidBitMove(prevMove)) {
					counterHeuristic[fromSq(prevMove)][toSq(prevMove)][b->stm] = currentMove;
				}
			}

			Assert(currentMove != NO_MOVE);
			Assert(abs(beta) <= INF);

			// Store position with bestMove and beta flag
			storeTT(b, currentMove, searchToHash(b, beta), ss->staticEval, TT_BETA, depth);
			return beta;
		}

		// If the currentMove scores higher than bestMove, update score and move.
		// This might be useful when this position is stored into the ttable.
		if (value > bestValue) {
			bestValue = value;
			bestMove = currentMove;
			Assert(bestValue < INF);

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
			return -MATE_VALUE + b->ply;
		} else {
			// Stalemate
			return 0;
		}
	}

	// If alpha has been changed, a better and exact score can be stored to transposition table.
	// Alpha scores are stored, if no move could improve over alpha.
	if (alpha != oldAlpha) {
		Assert(bestMove != NO_MOVE);
		Assert(abs(bestValue) <= INF);

		storeTT(b, bestMove, searchToHash(b, bestValue), ss->staticEval, TT_VALUE, depth);
	} else if (bestMove) {
		Assert(bestMove != NO_MOVE);
		Assert(abs(alpha) <= INF);

		storeTT(b, bestMove, searchToHash(b, alpha), ss->staticEval, TT_ALPHA, depth);
	}

	return alpha;
}

template <nodeType_t nodeType>
value_t quiescence(value_t alpha, value_t beta, int depth, board_t* b, search_t* s) {
	Assert(checkBoard(b));

	// check for time and depth
	if ((s->nodes & 2047) == 0 || (s->qnodes & 2047) == 0) {
		checkSearchInfo(s);
	}

	s->qnodes++;
	bool pvNode = nodeType == PV;
	selDepth = max(selDepth, b->ply);
	move_t currPvLine[MAX_DEPTH + 1];
	searchStack_t* ss = &sStack[b->ply];

	if (pvNode) {
		(ss + 1)->pvLine = currPvLine;
		ss->pvLine[0] = NO_MOVE;
	}

	// Draw detection
	if (isRepetition(b) || insufficientMaterial(b)) {
		return contemptFactor(b);
	} else if (b->fiftyMove >= 100) {
		// 50-move rule might checkmate / stalemate
		return (b->fiftyMove == 100 && isCheck(b, b->stm) && !hasEvadingMove(b)) ? -MATE_VALUE + b->ply
			: contemptFactor(b);
	}

	if (b->ply > MAX_DEPTH - 1) {
#ifdef INFO
		cout << "Warning: MAX_DEPTH reached in quiescence." << endl;
#endif // !LICHESS
		return evaluation(b);
	}

	//bool inCheck = ss->isCheck;
	//Assert(inCheck == isCheck(b, b->side));
	bool inCheck = isCheck(b, b->stm);

	value_t value;
	value_t bestValue = -INF;
	value_t staticEval = evaluation(b);
	Assert(abs(staticEval) < INF);

	// Stand Pat Pruning:
	// Side to move can reject a recapture, if static evaluation is already good enough.
	// Stand pat is not valid when in check, because this could result in misleading
	// evaluation in checkmate or stalemate position.
	if (!inCheck) {
		bestValue = staticEval;
		if (bestValue >= beta) {
			return bestValue;
		}

		if (bestValue > alpha) {
			alpha = bestValue;
		}
	}

	int legalMoves = 0;
	value_t oldAlpha = alpha;
	move_t bestMove = NO_MOVE;

	moveList_t moveList[1];
	generateQuiescence(b, moveList, inCheck);

	// Checkmate Detection:
	// Quiescence generates check evasions. If in check and no evading moves possible, return this is mate.
	if (moveList->cnt == 0 && inCheck) {
		// Checkmate
		return -MATE_VALUE + b->ply;
	}

	// First ply of quiescence generates checkers, if there is no check at current position.
	if (!inCheck && depth == 0) {
		generateQuietCheckers(b, moveList);
	}

	scoreMoves(b, moveList, NO_MOVE);

	// There are no cutoffs in this node yet. Generate all captures, promotions or check evasions
	// and expand this node.
	for (int i = 0; i < moveList->cnt; i++) {

		getNextMove(moveList, i);
		move_t currentMove = moveList->moves[i];
		Assert(currentMove != NO_MOVE);
		Assert(moveList->scores[i] >= 0);

		// SEE-Pruning:
		// If SEE score is negative, this is a losing capture. Since there is little to no chance that
		// this move raises alpha, it can be pruned.
		if (bestValue > -ISMATE
			&& moveList->scores[i] < BAD_CAPTURE + MVV_LVA_UBOUND) {
			continue;
		}

		//bool moveGivesCheck = checkingMove(b, currentMove);
		//(ss + 1)->isCheck = moveGivesCheck;
		//(ss + 1)->currentMove = currentMove;
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

			return beta;
		}
	}

	//if (pvNode && alpha != oldAlpha) {
	//	Assert(bestMove != NO_MOVE);
	//	Assert(abs(bestValue) <= INF);
	//	storeTT(b, bestMove, searchToHash(b, bestValue), staticEval, TT_VALUE, 0);
	//} else if (bestMove) {
	//	Assert(bestMove != NO_MOVE);
	//	Assert(abs(alpha) <= INF);
	//	storeTT(b, bestMove, searchToHash(b, alpha), staticEval, TT_ALPHA, 0);
	//}

	Assert(alpha >= oldAlpha);
	Assert(abs(bestValue) < INF);

	return bestValue;
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
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < NUM_SQUARES; j++) {
			for (int k = 0; k < NUM_SQUARES; k++) {
				histHeuristic[i][j][k] = 0;
			}
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

	b->pt->collided = 0;
	b->pt->probed = 0;
	b->pt->hit = 0;

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

value_t search(board_t* b, search_t* s) {


	move_t pvLine[MAX_DEPTH + 1]{};
	int currentDepth = 1;
	value_t score = -INF;
	move_t bestMove = 0;

	clearForSearch(b, s);

	sStack[b->ply].isCheck = isCheck(b, b->stm);
	sStack[b->ply].staticEval = evaluation(b);
	sStack[b->ply].currentMove = NO_MOVE;
	sStack[b->ply].pvLine = pvLine;

	for (; currentDepth <= s->depth; currentDepth++) {
		selDepth = 0;
		b->ply = 0;

		score = alphaBeta<PV>(-INF, INF, currentDepth, b, s);
		Assert(abs(score) < INF);

		// forced stop, break and use pv line of previous iteration
		if (s->stopped)
			break;

		printUCI(s, currentDepth, selDepth, score);
		//printTTablePV(b, currentDepth, selDepth);
		printPvLine(b, pvLine, currentDepth, score);
		fflush(stdout);

		bestMove = probePV(b);
		//printSearchInfo(b, s);

		if (abs(score) > ISMATE) {
			cout << "\n";
			cout << "bestmove " << getStringMove(b, bestMove) << "\n";
			return score;
		}

		cout << endl;
	}

	cout << "\n";
	cout << "bestmove " << getStringMove(b, bestMove) << "\n";
	return score;
}

void printSearchInfo(board_t* b, search_t* s) {
	cout << "\n";
	//cout << "AlphaBeta-Nodes: " << s->nodes << " Q-Nodes: " << s->qnodes << endl;
	cout << "Ordering percentage: \t\t" << setprecision(4) << fixed << (float)(s->fhf / s->fh) << endl;
	cout << "TTable hits/probed: \t\t" << setprecision(4) << fixed << (float)(b->tt->hit) / (b->tt->probed) << endl;
	cout << "TTable valueHits/hits: \t\t" << setprecision(4) << fixed << (float)(b->tt->valueHit) / (b->tt->hit) << endl;
	cout << "T table memory used: \t\t" << setprecision(4) << fixed << (float)(b->tt->stored) / (b->tt->buckets) << endl;
	cout << "PTable hit percentage: \t\t" << setprecision(4) << fixed << (float)(b->pt->hit) / (b->pt->probed) << endl;
	cout << "PTable memory used: \t\t" << setprecision(4) << fixed << (float)(b->pt->stored) / (b->pt->entries) << endl;
	cout << "PTcollisions: \t\t\t" << setprecision(4) << fixed << b->pt->collided << endl;
	//cout << "Futile failed/tried: \t\t\t" << s->futileFH << "/" << s->futileCnt << endl;
	cout << "Futile moves pruned: \t\t" << s->futileCnt << endl;
	cout << endl;

	//printTTStatus(b);
	cout << endl;
}

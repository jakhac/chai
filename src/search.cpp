#include "search.h"

using namespace Threads;


// MCP depth limit
const int moveCountPruningDepth = 5;

// MCP movecount according to all depths
int volatile moveCountPruning[moveCountPruningDepth];

const Value aspiration = 18;

const unsigned long long aspirationWindows[] = {
    100, 2500, 32000, 64000
};


void Search::init() {
    // Calculate MCP depths
    for (int depth = 1; depth < moveCountPruningDepth; depth++) {
        moveCountPruning[depth] = (int)(8.0 + 3.0 * depth * depth / 3.5);
    }
}

void checkTime(Thread thread) {

    if (thread->id == 0
        && thread->instr.timeSet
        && !isTimeLeft(&thread->instr)) {
        
        // Time is up: Set abort flag to signal other 
        // threads to stop searching immediatly.
        ABORT_SEARCH = true;
    }
}

bool isRepetition(Board* b) {

    for (int index = std::max(0, b->undoPly - b->fiftyMove); index < b->undoPly - 1; ++index) {
        Assert(index >= 0 && index < MAX_GAME_MOVES);

        if (b->zobristKey == b->undoHistory[index].zobKey) {
            return true;
        }
    }

    return false;
}

void swapMove(MoveList* moveList, int id1, int id2) {

    if (id1 == id2)
        return;

    // swap score and move in BOTH lists
    int tempMove  = moveList->moves[id1];
    int tempScore = moveList->scores[id1];

    moveList->moves[id1]  = moveList->moves[id2];
    moveList->scores[id1] = moveList->scores[id2];
    moveList->moves[id2]  = tempMove;
    moveList->scores[id2] = tempScore;
}

void getNextMove(MoveList* moveList, int curIdx) {

    int bestScore = 0;
    int bestIdx   = curIdx;

    for (int i = curIdx; i < moveList->cnt; i++) {
        if (moveList->scores[i] > bestScore) {
            bestScore = moveList->scores[i];
            bestIdx = i;
        }
    }
    swapMove(moveList, curIdx, bestIdx);
}

bool zugzwang(Board* b) {

    Bitboard bb;

    // Less than 5 pieces on the board is obvious endgame
    if (popCount(b->occupied) <= 5) {
        return true;
    }

    // Pawn endgame, no pieces left
    bb = b->pieces[KNIGHT] 
       | b->pieces[BISHOP] 
       | b->pieces[ROOK  ] 
       | b->pieces[QUEEN ];
    if (!bb) {
        return true;
    }

    // No pawns left on the board
    bb = b->pieces[PAWN];
    if (!bb) {
        return true;
    }

    // Knight endgame (only pieces are knight, cannot lose tempo)
    bb = b->pieces[BISHOP] 
       | b->pieces[ROOK  ] 
       | b->pieces[QUEEN ];
    if (!bb) {
        return true;
    }

    // All pawns of one side are blocked (cannot safely advance)
    if (!hasSafePawnPush(b, b->stm))
        return true;

    return false;
}

// following moves are tactical: caps, proms, eps
static bool moveIsTactical(Board* b, Move move) {
    return isCapture(b, move)
        || isPromotion(move)
        || isEnPassant(move);
}

// return true if any pawns are on 2nd, resp. 7th, rank
static bool promotablePawns(Board* b, bool side) {

    if (side == WHITE) {
        return getPieces(b, PAWN, WHITE) & RANK_7_HEX;
    } else {
        return getPieces(b, PAWN, BLACK) & RANK_2_HEX;
    }
}

// check if move pushes pawn on 2nd/7th rank (call AFTER! pushing the move)
static bool dangerousPawnPush(Board* b, Move m) {

    if (piecePawn[pieceAt(b, toSq(m))]) {
        int rank = toRank(toSq(m));
        return (b->stm == WHITE) ? rank == relSeventh<WHITE>()
                                  : rank == relSeventh<BLACK>();

    }

    return false;
}

// Calculate biggest material swing BEFORE for all possible moves
static Value biggestMaterialSwing(Board* b) {

    // Start with roughly 2 pawns to compensate activity
    Value swing = pieceValues[PAWN] * 2;
    int opponent = !b->stm;
    int MVPiece = QUEEN;

    // Add value most valuable piece
    for (; MVPiece > NO_PTYPE; MVPiece--) {
        if (b->color[opponent] & b->pieces[MVPiece])
            break;
    }
    swing += pieceValues[MVPiece];

    // Add queen value if promotions are possible
    if (promotablePawns(b, b->stm))
        swing += pieceValues[QUEEN] - pieceValues[PAWN];


    return swing;
}

static void updatePvLine(Move* pv, Move move, Move* childPv) {

    Assert(pv);
    Assert(childPv);

    for (*pv++ = move; childPv && *childPv != MOVE_NONE; ) {
        *pv++ = *childPv++;
    }
    *pv = MOVE_NONE;
}


static void resetSearchParameters(Thread thread) {

    Board* b = &thread->b;
    Stats* s = &thread->s;
    Instructions* i = &thread->instr;

    b->ply = 0;

    thread->selDepth = 0;
    thread->depth    = 0;
    thread->nodes    = 0;
    thread->qnodes   = 0;

    // Reset stats
    tt->probed   = 0;
    tt->hit      = 0;
    tt->valueHit = 0;

    // Reset term stats
    tt->collided = 0;
    tt->stored   = 0;
    pt->collided = 0;
    pt->probed   = 0;
    pt->hit      = 0;


    i->startTime = getTimeMs();
    i->stopped   = false;

    s->tbHit     = 0;
    s->fhf       = 0;
    s->fh        = 0;
    s->futileFH  = 0;
    s->futileCnt = 0;

    // NNUE
    b->accum[b->ply].compState[WHITE] = EMPTY;
    b->accum[b->ply].compState[BLACK] = EMPTY;
}

Value Search::search(Board* b, Stats* s, Instructions* i) {
    
    // 1. Prepare each thread for search
    resetAllThreadStates(b, s, i);

    // 2. Start helper threads
    ABORT_SEARCH = false;
    startAllThreads();

    // 3. Start main thread (process)
    iid(threadPool[0]);
    ABORT_SEARCH = true;

    // 4. Wait for threads to return
    waitAllThreads();
    cout << "All " << NUM_THREADS << " threads returned from search" << endl;

    // // Compare thread data
    // for (int t = 0; t < NUM_THREADS; t++) {
    // 	cout << "T" << t << " (q+)nodes = " << (threadPool[t]->nodes + threadPool[t]->qnodes) << endl;
    // 	cout << "T" << t << " bestScore = " << threadPool[t]->bestScore << endl;
    // 	cout << "T" << t << " bestMove = " << getStringMove(b, threadPool[t]->bestMove) << endl;
    // 	cout << "T" << t << " depth reached = " << threadPool[t]->depth << endl;
    // 	cout << endl;
    // }

    // Select "best" thread and return data
    int totalNodes  = totalNodeCount();
    Thread t        = threadPool[selectBestThreadIndex()];
    s->depthReached = t->depth;

    // UCI commands
    printUCI(&t->instr, &t->s, t->depth, t->selDepth, t->bestScore, totalNodes);
    printPvLine(b, t->pvLine, t->depth, t->bestScore);
    cout << endl;

    printUCIBestMove(b, t->bestMove);
    return t->bestScore;
}

void Threads::iid(Thread thread) {

    Board* b  = &thread->b;
    Stats* s  = &thread->s;
    Instructions* instr = &thread->instr;

    Value previousBestScore = VALUE_NONE;
    Value tempBestScore     = VALUE_NONE;
    thread->bestScore       = tempBestScore;
     thread->bestMove        = MOVE_NONE;

    // Search setup 
    resetSearchParameters(thread);

    // Set up variables used before first recursive call
    SearchStack* ss = &thread->ss[b->ply];
    ss->currentMove = MOVE_NONE;
    ss->isCheck     = isCheck(b, b->stm);
    ss->staticEval  = evaluation(b);
    ss->pvLine      = thread->pvLine;

    // Iterative deepening
    int d = 1;
    Assert(instr->depth <= MAX_DEPTH);
    while (d <= instr->depth) {

        // Open window for small depths, then aspiration window
        if (d > 3) {
            tempBestScore = Search::aspirationSearch(thread, d, thread->bestScore);
        } else {
            tempBestScore = Search::alphaBeta<PV>(thread, -VALUE_INFTY, VALUE_INFTY, d);
        }

        // Use previous bestMove if forced stop in ongoing search
        if (ABORT_SEARCH)
            break;

        // Search was within limits, thus update thread variables
        thread->bestScore = tempBestScore;
        thread->depth     = d;
        previousBestScore = thread->bestScore;

        // Main thread has two extra tasks:
        // 		1) Print UCI information since this is the main search.
        // 		2) Adjust time if there was a jump between previous 
        // 		   and current bestScore.
        if (thread->id == 0) {

            // 1)
            printUCI(instr, s, thread->depth, thread->selDepth, thread->bestScore, 
                thread->nodes + thread->qnodes);
            printPvLine(b, thread->pvLine, thread->depth, thread->bestScore);
            cout << endl;

            // 2)
            Value scoreDifference = abs(thread->bestScore - previousBestScore);
            if (d > 1 
                && instr->timeSet
                && scoreDifference > 30) {
                instr->allocatedTime = std::min(instr->timeLeft - 50., instr->allocatedTime * 1.075);
            }
        }

        // Update best move after every complete search iteration
        thread->bestMove = thread->pvLine[0];
        Assert(thread->bestMove != MOVE_NONE);

        // Leave IID when mate is found
        if (abs(thread->bestScore) > VALUE_IS_MATE_IN) {
            break;
        }

        d++;
    }
}

Value Search::aspirationSearch(Thread thread, int d, Value bestScore) {

    Assert(abs(bestScore) < VALUE_INFTY);

    Value score   = -VALUE_INFTY;
    int researchCnt = 0;

    // Determine initial alpha/beta values
    int64_t alpha = std::max(-VALUE_INFTY, bestScore - aspiration);
    int64_t beta  = std::min(VALUE_INFTY, (Value)(bestScore + aspiration));
    int64_t newAlpha, newBeta;

    // Research, until score is within bounds
    while (!ABORT_SEARCH) {

        //cout << "d=" << d << " " << alpha << " " << beta << endl;
        score = Search::alphaBeta<PV>(thread, alpha, beta, d);

        // Score within bound is returned immediatly
        if (alpha < score && score < beta) {
            return score;
        }

        // Alpha window was too high: decrease
        if (score <= alpha) {
            newAlpha = alpha - aspirationWindows[researchCnt++];

            alpha = std::max((int64_t)-VALUE_INFTY, newAlpha);
        }

        // Beta window was too low: increase
        if (score >= beta) {
            newBeta = beta + aspirationWindows[researchCnt++];

            beta = std::min((int64_t)VALUE_INFTY, newBeta);
        }
    }

    Assert(abs(score) < VALUE_INFTY);
    return score;
}

template <nodeType_t nodeType>
Value Search::alphaBeta(Thread thread, Value alpha, Value beta, int depth) {

    // Initialize node
    Instructions* instr = &thread->instr;
    Board* b            = &thread->b;
    Stats* s            = &thread->s;

    bool mainThread = thread->id == 0;
    bool pvNode     = nodeType == PV;
    bool rootNode   = b->ply == 0;
    bool mateThreat = false;
    bool improving  = false;
    int newDepth    = depth - 1;

    Value value     = -VALUE_INFTY;
    Value bestValue = -VALUE_INFTY;
    Value posValue;

    Move pvLine[MAX_DEPTH];
    SearchStack* ss = &thread->ss[b->ply];

    Assert(checkBoard(&thread->b));
    Assert(alpha < beta);
    Assert(pvNode || (alpha == beta - 1));

    if (ABORT_SEARCH || insufficientMaterial(b)) {
        return 0;
    }

    if (b->ply >= MAX_DEPTH) {
        return evaluation(b);
    }

    // Check if position is draw at a non-root node
    if (!rootNode) {
        if (isRepetition(b)) {
            return 0;
        }

        // 50-move rule might checkmate / stalemate
        if (b->fiftyMove >= 100) {
            return (b->fiftyMove == 100 
                    && isCheck(b, b->stm) 
                    && !hasEvadingMove(b)) ? -VALUE_MATE + b->ply
                                           : 0;
        }
    }

    // Drop into quiescence if maximum depth is reached
    if (depth <= 0) {
        Value quietScore = Search::quiescence<nodeType>(thread, alpha, beta, 0);
        Assert(abs(quietScore) < VALUE_INFTY);

        return quietScore;
    }

    // Early TT prefetch
    TT::prefetchTT(b);

    // check for time and depth
    if (mainThread && (thread->nodes & 2047) == 0) {
        checkTime(thread);
    }
    thread->nodes++;

    // Mate Distance Pruning:
    // If alpha or beta are already mate scores, bounds can be adjusted to prune irrelevant subtrees.
    // Mates are delivered faster, but does not speed up search in general.
    if (!rootNode) {
        alpha = std::max(alpha, (Value)(-VALUE_MATE + b->ply));
        beta = std::min(beta, (Value)(VALUE_MATE - b->ply - 1));
        if (alpha >= beta)
            return alpha;
    }

    // Transposition Table Probing:
    // Probe the TTable and look for useful information from previous transpositions. Return hashScore if hash table
    // stored a better score at same or greater depth. Do not return if close to 50-move draw.
    Move hashMove    = MOVE_NONE;
    Value hashValue  = -VALUE_INFTY;
    Value hashEval   = -VALUE_INFTY;
    int8_t hashDepth = -1;
    uint8_t hashFlag = TT_NONE;

    bool hashStored = TT::probeTT(b, &hashMove, &hashValue, &hashEval, &hashFlag, &hashDepth);
    if (hashStored
        && !pvNode
        && !(hashFlag & TT_EVAL)) {
        tt->hit++;

        // Look for valueHit: This position has already been searched before with greater depth.
        if (hashDepth >= depth && b->fiftyMove < 90) {
            Assert(hashDepth >= QS_DEPTH && hashDepth <= MAX_DEPTH);
            Assert(hashFlag >= TT_ALPHA && hashFlag <= TT_VALUE);
            Assert(hashValue > -VALUE_INFTY && hashValue < VALUE_INFTY);

            if (hashFlag & TT_VALUE) {
                tt->valueHit++;
                return hashValue;
            }

            if (hashFlag & TT_BETA && hashValue >= beta) {
                tt->valueHit++;
                return hashValue;
            }

            if (hashFlag & TT_ALPHA && hashValue <= alpha) {
                tt->valueHit++;
                return hashValue;
            }

        }
    }

    // EGTB:
    // Endgame-Tablebase probing.
    int tbFlag;
    unsigned int tbResult = EGTB::probeTB(b);
    if (tbResult != TB_RESULT_FAILED) {
        s->tbHit++;

        Value tbValue = tbResult == TB_LOSS ? -VALUE_TB_WIN + b->ply
            : tbResult == TB_WIN ? VALUE_TB_WIN - b->ply : 0;

        tbFlag = tbResult == TB_LOSS ? TT_BETA
            : tbResult == TB_WIN ? TT_ALPHA : TT_VALUE;

        if (tbFlag == TT_VALUE
            || (tbFlag == TT_ALPHA && tbValue >= beta)
            || (tbFlag == TT_BETA && tbValue <= alpha)) {

            Assert(abs(TT::searchToHash(b->ply, tbValue)) != VALUE_INFTY);
            TT::storeTT(b, MOVE_NULL, TT::searchToHash(b->ply, tbValue), VALUE_NONE, tbFlag, depth);
            return tbValue;
        }
    }

    // Static evaluation of position:
    // Either use previos evaluations from TT hit / null-move or calculate evaluation score.
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
        //TT::storeTT(b, NULL_MOVE, NO_VALUE, ss->staticEval, TT_EVAL, MAX_DEPTH);
    }
    Assert(ss->staticEval != VALUE_NONE && abs(ss->staticEval) < VALUE_IS_MATE_IN);

    // Basic check extension
    bool inCheck = ss->isCheck;
    Assert3(inCheck == isCheck(b, b->stm), std::to_string(b->ply), getFEN(b));
    if (inCheck) {
        newDepth++;
    }

    if (!inCheck && b->ply > 2) {
        improving = !(ss - 2)->isCheck ? ss->staticEval > (ss - 2)->staticEval
                                       : ((b->ply > 4) ? ss->staticEval > (ss - 4)->staticEval
                                                       : false);
    }

    // Reverse Futility Pruning:
    // Similar idea like NMP, but (eval - margin) is acting as the lower bound.
    if (!pvNode
        && !inCheck
        && depth < 5
        && nonPawnPieces(b)
        //&& !promotablePawns(b, b->side ^ 1)
        && posValue <= VALUE_WIN) {

        int margin = (depth - improving) * (pieceValues[BISHOP] - 38);
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
        Value nullValue = -alphaBeta<NoPV>(thread, -beta, -beta + 1, depth - 1 - reduction);
        popNull(b);

        if (ABORT_SEARCH) {
            return 0;
        }

        // Even after giving opponent free move, position is winning..
        if (nullValue >= beta && nullValue < VALUE_IS_MATE_IN) {

            // nullScore acts as lower bound to this position
            if (depth - 1 - reduction > 0) {
                TT::storeTT(b, MOVE_NULL, TT::searchToHash(b->ply, nullValue),
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

    MoveList moveList;
    generateMoves(b, &moveList, inCheck);
    MoveOrder::scoreMoves(thread, &moveList, hashMove);

    Move currentMove = MOVE_NONE;
    Move bestMove    = MOVE_NONE;
    int legalMoves   = 0;
    int oldAlpha     = alpha;

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
        if (   !rootNode
            && legalMoves
            && depth < 5
            && abs(alpha) < VALUE_IS_MATE_IN
            && abs(beta)  < VALUE_IS_MATE_IN
            && bestValue  > -VALUE_IS_MATE_IN
            && nonPawnPieces(b, b->stm)
            && !isPromotion(currentMove)) {

            // Move count based pruning
            if (   !pvNode
                && !inCheck
                && !moveGivesCheck
                && depth < moveCountPruningDepth
                && legalMoves > moveCountPruning[depth]
                && !moveCaptured) {
                continue;
            }

            // Futile captures / checks: bad SEE score
            if (moveCaptured || moveGivesCheck) {

                if (!MoveOrder::see_ge(b, currentMove, -depth * (MoveOrder::SEEPieceValues[BISHOP]))) {
                    continue;
                }
            }

            // Futile quiet moves: cannot raise alpha
            if (   !inCheck
                && !mateThreat
                && !moveGivesCheck
                && !moveIsTactical(b, currentMove)) {

                if (ss->staticEval + (depth * pieceValues[BISHOP] + 34) < alpha) {
                    continue;
                }

                // Higher histScore by a move <=> Higher chance the move can raise alpha
                int histScore = thread->histHeuristic[b->stm][fromSq(currentMove)][toSq(currentMove)];

                // The lower the histScore, the higher the scaled score
                // => bad moves score near histmax and good moves score near 0
                int scaledHistScore = thread->histMax / (histScore ? histScore : 1);
                bool specialQuiet = moveList.scores[i] > MoveOrder::COUNTER_SCORE;

                if (ss->staticEval + (depth * 234) + (specialQuiet * 50) < alpha
                    && scaledHistScore > 2 * thread->histMax / 3) {
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

        if (   mainThread
            && rootNode
            && getTimeMs() > instr->startTime + 750
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
        (ss + 1)->pvLine = pvLine;
        (ss + 1)->pvLine[0] = MOVE_NONE;

        if (legalMoves == 1) {

            value = -alphaBeta<PV>(thread, -beta, -alpha, newDepth);

        } else {
            // Late Move Reductions, do reduced zero window search
            if (   depth > 3
                && legalMoves > 2 + rootNode
                && !isPromotion(currentMove)
                ) {

                lmrDepth = newDepth - 2;

                // Increase reduction for late moves
                if (legalMoves > 6)
                    lmrDepth--;

                if (!moveGivesCheck &&
                    (   ss->staticEval + pieceValues[moveCaptured] + depth * 243 < alpha
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

                if (!pvNode && moveList.scores[i] < MoveOrder::KILLER_SCORE_2)
                    lmrDepth--;

                if (pvNode && dangerousPawnPush(b, currentMove))
                    lmrDepth++;

                // Minimum depth is 1 and maximum depth is newDepth
                lmrDepth = std::min(newDepth, std::max(1, lmrDepth));
                lmrSameDepth = newDepth == lmrDepth;

                value = -alphaBeta<NoPV>(thread, -(alpha + 1), -alpha, lmrDepth);
            } else {
                // Ensure, that PVS is always exexcuted if LMR is not applicable
                value = alpha + 1;
            }

            // Principal Variation Search
            if (value > alpha) {

                // Ensure fresh PV
                (ss + 1)->pvLine = pvLine;
                (ss + 1)->pvLine[0] = MOVE_NONE;

                // Do not research with same depth as LMR
                if (!lmrSameDepth)
                    value = -alphaBeta<NoPV>(thread, -(alpha + 1), -alpha, newDepth);

                if (value > alpha && (rootNode || value < beta)) {

                    value = -alphaBeta<PV>(thread, -beta, -alpha, newDepth);
                }
            }
        }

        // Pop move from board
        pop(b);

        if (ABORT_SEARCH) {
            return 0;
        }

        // If the currentMove scores higher than bestMove, update score and move.
        // This might be useful when this position is stored into the ttable.
        if (value > bestValue) {
            bestValue = value;
            bestMove  = currentMove;
            Assert(bestValue < VALUE_INFTY);

            // If the currentMove scores higher than alpha, the principal variation
            // is updated because a better move for this position has been found.
            if (value > alpha) {
                alpha = value;

                if (pvNode)
                    updatePvLine(ss->pvLine, bestMove, (ss + 1)->pvLine);

            }
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
                && thread->killer[0][b->ply] != currentMove) {

                thread->killer[1][b->ply] = thread->killer[0][b->ply];
                thread->killer[0][b->ply] = currentMove;
                Assert(thread->killer[1][b->ply] != thread->killer[0][b->ply]);
            }

            // Mate Killers:
            // If a move scores near checkmate, order it above standard killers.
            if (value >= VALUE_IS_MATE_IN) {
                thread->mateKiller[b->ply] = currentMove;
            }

            // History Heuristic:
            // Color-From-To information is stored for each side. Do not apply, if capture, promoting or
            // in check (already considered in move ordering). Remember maximum history score and
            // rescale if needed to prevent overflow.
            if (!inCheck
                && !moveIsCapOrPromo) {
                int from = fromSq(currentMove);
                int to = toSq(currentMove);

                thread->histHeuristic[b->stm][from][to] += depth * depth;
                thread->histMax = std::max(thread->histHeuristic[b->stm][from][to], thread->histMax);

                if (thread->histMax >= HISTORY_MAX) {
                    for (int x = 0; x < 2; x++) {
                        for (int y = 0; y < NUM_SQUARES; y++) {
                            for (int z = 0; z < NUM_SQUARES; z++) {
                                thread->histHeuristic[x][y][z] >>= 3;
                            }
                        }
                    }
                    thread->histMax >>= 3;
                    Assert(thread->histMax < HISTORY_MAX);
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

                Move prevMove = b->undoHistory[b->ply - 1].move;
                if (pseudoValidBitMove(prevMove)) {
                    thread->counterHeuristic[fromSq(prevMove)][toSq(prevMove)][b->stm] = currentMove;
                }
            }

            Assert(currentMove != MOVE_NONE);
            Assert(abs(beta) <= VALUE_INFTY);

            // Store position with bestMove and beta flag
            TT::storeTT(b, currentMove, TT::searchToHash(b->ply, beta), ss->staticEval, TT_BETA, depth);
            return beta;
        }

        // // If the currentMove scores higher than bestMove, update score and move.
        // // This might be useful when this position is stored into the ttable.
        // if (value > bestValue) {
        // 	bestValue = value;
        // 	bestMove  = currentMove;
        // 	Assert(bestValue < VALUE_INFTY);

        // 	// If the currentMove scores higher than alpha, the principal variation
        // 	// is updated because a better move for this position has been found.
        // 	if (value > alpha) {
        // 		alpha = value;

        // 		if (pvNode)
        // 			updatePvLine(ss->pvLine, bestMove, (ss + 1)->pvLine);

        // 	}
        // }
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

        TT::storeTT(b, bestMove, TT::searchToHash(b->ply, bestValue), ss->staticEval, TT_VALUE, depth);
    } else if (bestMove) {
        Assert(bestMove != MOVE_NONE);
        Assert(abs(alpha) <= VALUE_INFTY);

        TT::storeTT(b, bestMove, TT::searchToHash(b->ply, alpha), ss->staticEval, TT_ALPHA, depth);
    }

    return alpha;
}

template <nodeType_t nodeType>
Value Search::quiescence(Thread thread, Value alpha, Value beta, int depth) {

    Assert(checkBoard(&thread->b));

    Board* b = &thread->b;
    Stats* s = &thread->s;
    bool mainThread = thread->id == 0;

    // Early TT prefetch 
    TT::prefetchTT(b);

    // Check for remaining time and depth. Helper threads have no constraints
    // as they are stopped as soon as the main thread returns.
    if (mainThread && (thread->qnodes & 2047) == 0) {
        checkTime(thread);
    }

    if (ABORT_SEARCH || insufficientMaterial(b)) {
        return 0;
    }

    thread->qnodes++;
    thread->selDepth = std::max(thread->selDepth, b->ply);

    bool pvNode = nodeType == PV;
    Move currPvLine[MAX_DEPTH + 1];
    SearchStack* ss = &thread->ss[b->ply];

    if (pvNode) {
        (ss + 1)->pvLine = currPvLine;
        ss->pvLine[0] = MOVE_NONE;
    }

    // Draw detection
    if (isRepetition(b) || insufficientMaterial(b)) {
        return 0;
    } else if (b->fiftyMove >= 100) {
        // 50-move rule might checkmate / stalemate
        return (b->fiftyMove == 100 
                && isCheck(b, b->stm) 
                && !hasEvadingMove(b)) ? -VALUE_MATE + b->ply
                                       : 0;
    }

    if (b->ply > MAX_DEPTH - 1) {
        return evaluation(b);
    }

    //bool inCheck = ss->isCheck;
    //bool inCheck = (depth == 0) ? ss->isCheck : isCheck(b, b->stm);
    bool inCheck = isCheck(b, b->stm);
    int ttDepth  = (inCheck) ? QS_DEPTH_CHECK : QS_DEPTH;
    Assert(inCheck == isCheck(b, b->stm));

    // Transposition Table:
    // Probe transposition table and try to cutoff, if this node is non-pv.
    int8_t  hashDepth;
    Move  hashMove   = MOVE_NONE;
    Value hashValue  = -VALUE_INFTY;
    Value hashEval   = -VALUE_INFTY;
    uint8_t hashFlag = TT_NONE;

    bool hashStored = TT::probeTT(b, &hashMove, &hashValue, &hashEval, &hashFlag, &hashDepth);
    if (hashStored && !pvNode) {
        tt->hit++;

        Assert(hashDepth <= MAX_DEPTH);
        Assert(hashFlag >= TT_ALPHA && hashFlag <= TT_VALUE);
        Assert(hashValue > -VALUE_INFTY && hashValue < VALUE_INFTY);

        if (hashDepth >= ttDepth) {

            if (hashFlag & TT_VALUE) {
                tt->valueHit++;
                return hashValue;
            }

            if (hashFlag & TT_BETA && beta <= hashValue) {
                tt->valueHit++;
                return hashValue;
            }

            if (hashFlag & TT_ALPHA && alpha >= hashValue) {
                tt->valueHit++;
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
    Value value;
    Value bestValue = -VALUE_INFTY;

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
    Value oldAlpha = alpha;
    Move bestMove  = MOVE_NONE;

    MoveList moveList;
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

    MoveOrder::scoreMoves(thread, &moveList, hashMove);

    // There are no cutoffs in this node yet. Loop through all captures, promotions or check evasions
    // and expand search.
    for (int i = 0; i < moveList.cnt; i++) {

        getNextMove(&moveList, i);
        Move currentMove = moveList.moves[i];
        Assert(currentMove != MOVE_NONE);
        Assert(moveList.scores[i] >= 0);

        // SEE-Pruning:
        // If SEE score is negative, this is a losing capture. Since there is little to no chance that
        // this move raises alpha, it can be pruned.
        if (bestValue > -VALUE_IS_MATE_IN
            && moveList.scores[i] < MoveOrder::BAD_CAPTURE + MoveOrder::MVV_LVA_UBOUND) {
            continue;
        }

        //(ss + 1)->isCheck = moveGivesCheck;
        (ss + 1)->currentMove = currentMove;
        if (!push(b, currentMove)) {
            Assert(!inCheck); // Position cannot be check because all check evasions are legal
            continue;
        }

        legalMoves++;
        value = -quiescence<nodeType>(thread, -beta, -alpha, depth - 1);
        pop(b);

        if (ABORT_SEARCH) {
            return 0;
        }

        if (value > bestValue) {
            bestValue = value;
            bestMove  = currentMove;

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

            TT::storeTT(b, currentMove, TT::searchToHash(b->ply, beta), ss->staticEval, TT_BETA, ttDepth);
            return beta;
        }
    }

    if (alpha > oldAlpha) {
        Assert(bestMove != MOVE_NONE);
        Assert(abs(bestValue) <= VALUE_INFTY);
        TT::storeTT(b, bestMove, TT::searchToHash(b->ply, bestValue), ss->staticEval, TT_VALUE, ttDepth);
    } else if (bestMove) {
        Assert(bestMove != MOVE_NONE);
        Assert(abs(alpha) <= VALUE_INFTY);
        TT::storeTT(b, bestMove, TT::searchToHash(b->ply, alpha), ss->staticEval, TT_ALPHA, ttDepth);
    }

    Assert(alpha >= oldAlpha);
    Assert(abs(bestValue) < VALUE_INFTY);

    return bestValue;
}

void Search::printSearchInfo(Stats* s) {

    cout << "\n";
    //cout << "AlphaBeta-Nodes: " << s->nodes << " Q-Nodes: " << s->qnodes << endl;
    cout << "Ordering percentage: \t\t" 	<< std::setprecision(4) << fixed << (float)(s->fhf / s->fh) << endl;
    cout << "TTable hits/probed: \t\t" 		<< std::setprecision(4) << fixed << (float)(tt->hit) / (tt->probed) << endl;
    cout << "TTable valueHits/hits: \t\t" 	<< std::setprecision(4) << fixed << (float)(tt->valueHit) / (tt->hit) << endl;
    cout << "T table memory used: \t\t" 	<< std::setprecision(4) << fixed << (float)(tt->stored) / (tt->buckets) << endl;
    cout << "PTable hit percentage: \t\t" 	<< std::setprecision(4) << fixed << (float)(pt->hit) / (pt->probed) << endl;
    cout << "PTable memory used: \t\t" 		<< std::setprecision(4) << fixed << (float)(pt->stored) / (pt->entries) << endl;
    cout << "PTcollisions: \t\t\t" 			<< std::setprecision(4) << fixed << pt->collided << endl;
    cout << "Futile moves pruned: \t\t" 	<< s->futileCnt << endl;
    //cout << "Futile failed/tried: \t\t\t" << s->futileFH << "/" << s->futileCnt << endl;
    cout << endl;

    //printTTStatus(b);
    cout << endl;
}


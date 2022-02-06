#include "moveOrdering.h"

// using namespace MoveOrder;
namespace MoveOrder {


int MVV_LVA[13][13];

void init() {

    int attacker;
    int victim;

    for (attacker = Piece::P; attacker <= Piece::k; attacker++) {
        for (victim = Piece::P; victim <= Piece::k; victim++) {
            MVV_LVA[victim][attacker] = victimScore[victim] + 6 - (victimScore[attacker] / 100);
        }
    }
}

Bitboard getLeastValuablePiece(Board* b, Bitboard atkDef, int side) {

    atkDef &= b->color[side];
    if (!atkDef) {
        return 0;
    }

    Bitboard lva;
    for (int i = PAWN; i <= KING; i++) {
        lva = atkDef & b->pieces[i];
        if (lva) {
            return setMask[getLSB(lva)];
        }
    }

    Assert(false);
    return 0;
}

int see(Board* b, const int move) {

    int to = toSq(move);
    int d  = 0;
    int atkPiece = pieceAt(b, fromSq(move));
    int gain[32]{};
    
    Color side = b->stm;

    Bitboard occ     = b->occupied;
    Bitboard mayXray = b->pieces[PAWN  ] 
                     | b->pieces[BISHOP]
                     | b->pieces[ROOK  ] 
                     | b->pieces[QUEEN ];

    Bitboard attadef    = squareAtkDef(b, to);
    Bitboard from       = setMask[fromSq(move)];
    Bitboard used       = 0;
    Bitboard discovered = 0;

    gain[d] = pieceValues[capPiece(b, move)];
    do {

        Assert(pieceValid(atkPiece));
        d++; // next depth and side
        gain[d] = -gain[d - 1] + pieceValues[atkPiece]; // speculative store, if defended

        if (std::max(-gain[d - 1], gain[d]) < 0)
            break; // pruning does not influence the result

        attadef ^= from; // reset bit in set to traverse
        occ 	^= from; // reset bit in temporary occupancy (for magic move generation)
        used    |= from;

        if (from & mayXray) {
            discovered  = 0;
            discovered |= lookUpBishopMoves(to, occ) & (getPieces(b, QUEEN, side) | getPieces(b, BISHOP, side));
            discovered |= lookUpRookMoves(to, occ) & (getPieces(b, QUEEN, side) | getPieces(b, ROOK, side));
            attadef    |= discovered & ~used;
        }

        side     = !side;
        from     = getLeastValuablePiece(b, attadef, side);
        atkPiece = from ? pieceAt(b, getLSB(from)) : 0;

    } while (from && atkPiece);

    while (--d) {
        gain[d - 1] = -std::max(-gain[d - 1], gain[d]);
    }

    if (isEnPassant(move))
        gain[0] += 100;

    return gain[0];
}

bool see_ge(Board* b, Move move, int threshold) {

    bool color;
    int from = fromSq(move);
    int to   = toSq(move);

    int nextVictim = pieceAt(b, from);
    int balance    = SEEPieceValues[pieceAt(b, to)];

    if (isCastling(move))
        return false;

    if (isPromotion(move)) {
        nextVictim = promPiece(b, move);
        balance += (SEEPieceValues[nextVictim] - SEEPieceValues[PAWN]);
    }

    if (isEnPassant(move)) {
        balance = SEEPieceValues[PAWN];
    }

    balance -= threshold;
    balance -= SEEPieceValues[nextVictim];

    // Even after losing the moved piece balance is up
    if (balance >= 0) return true;

    // slider used to detect discovered attack
    Bitboard bishops = b->pieces[BISHOP] | b->pieces[QUEEN];
    Bitboard rooks   = b->pieces[ROOK] | b->pieces[QUEEN];

    // copy occupied to simulate captures on bitboard
    Bitboard occ = b->occupied;
    occ = (occ ^ (1ULL << from)) | (1ULL << to);
    if (isEnPassant(move)) {
        occ ^= (1ULL << b->enPas);
    }

    Bitboard nowAttacking;
    Bitboard attackers = squareAtkDefOcc(b, occ, to) & occ;

    color = !b->stm;

    while (1) {
        nowAttacking = attackers & b->color[color];

        // No more attackers left
        if (!nowAttacking) {
            break;
        }

        // Find weakest attacker
        for (nextVictim = PAWN; nextVictim <= KING; nextVictim++) {
            if (nowAttacking & b->pieces[nextVictim])
                break;
        }

        // Remove least valuable attacker
        Assert(nowAttacking & b->pieces[nextVictim]);
        occ ^= (1ULL << getLSB(nowAttacking & b->pieces[nextVictim]));

        if (   nextVictim == PAWN 
            || nextVictim == BISHOP 
            || nextVictim == QUEEN)
            attackers |= lookUpBishopMoves(to, occ) & bishops;

        if (   nextVictim == ROOK 
            || nextVictim == QUEEN)
            attackers |= lookUpRookMoves(to, occ) & rooks;

        attackers &= occ;
        color ^= 1;

        balance = -balance - 1 - SEEPieceValues[nextVictim];

        if (balance >= 0) {

            if (nextVictim == KING && (attackers & b->color[color])) {
                color ^= 1;
            }

            break;
        }

    }

    return b->stm != color;
}

void scoreMoves(Thread thread, MoveList* moveList, Move hashMove) {

    Board* b = &thread->b;
    Move currentMove;
    
    int mvvLvaScore   = 0;
    int capturedPiece = 0;

    for (int i = 0; i < moveList->cnt; i++) {
        currentMove = moveList->moves[i];
        Assert(currentMove != MOVE_NONE);
        Assert(currentMove != MOVE_NULL);

        // Hash move
        if (currentMove == hashMove) {
            moveList->scores[i] = HASH_MOVE;
            continue;
        }

        // Enpas move
        if (isEnPassant(currentMove)) {
            moveList->scores[i] = GOOD_CAPTURE + 105;
            continue;
        }

        // Capture moves
        capturedPiece = capPiece(b, currentMove);
        if (capturedPiece) {
            Assert(pieceValid(capturedPiece));

            if (promPiece(b, currentMove)) {
                // Promoting captures
                moveList->scores[i] = PROMOTING_CAPTURE + pieceAt(b, fromSq(currentMove));
            } else {
                // Plain Captures
                mvvLvaScore = MVV_LVA[capturedPiece][pieceAt(b, fromSq(currentMove))];
                Assert(mvvLvaScore > 0);
                Assert(mvvLvaScore < MVV_LVA_UBOUND);

                if (see_ge(b, currentMove, 0)) {
                    moveList->scores[i] = GOOD_CAPTURE + mvvLvaScore;
                } else {
                    moveList->scores[i] = BAD_CAPTURE + mvvLvaScore;
                }

            }
            continue;
        }

        /* Only quiet moves left */

        // Promotion
        if (promPiece(b, currentMove)) {
            moveList->scores[i] = PROMOTION + pieceAt(b, fromSq(currentMove));
            continue;
        }

        // Mate killer
        if (currentMove == thread->mateKiller[b->ply]) {
            moveList->scores[i] = MATE_KILLER;
            continue;
        }

        // First killer
        if (currentMove == thread->killer[0][b->ply]) {
            Assert(!capPiece(b, currentMove));
            moveList->scores[i] = KILLER_SCORE_1;
            continue;
        }

        // Second killer
        if (currentMove == thread->killer[1][b->ply]) {
            Assert(!capPiece(b, currentMove));
            moveList->scores[i] = KILLER_SCORE_2;
            continue;
        }

        // Counter move
        if (b->ply > 0) {
            Move prevMove    = b->undoHistory[b->ply - 1].move;
            Move counterMove = thread->counterHeuristic[fromSq(prevMove)][toSq(prevMove)][b->stm];

            if (currentMove == counterMove) {
                moveList->scores[i] = COUNTER_SCORE;
                continue;
            }

        }

        // Castle move
        if (isCastling(currentMove)) {
            moveList->scores[i] = CASTLE_SCORE;
            continue;
        }

        // Last resort: history heuristic
        int histScore = thread->histHeuristic[b->stm][fromSq(currentMove)][toSq(currentMove)];
        Assert(0 <= histScore && histScore <= HISTORY_MAX);
        moveList->scores[i] = QUIET_SCORE + (histScore);
        Assert3(moveList->scores[i] < COUNTER_SCORE,
                std::to_string(moveList->scores[i]), std::to_string(COUNTER_SCORE));
    }
}


}

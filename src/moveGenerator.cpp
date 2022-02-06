#include "moveGenerator.h"

void generateMoves(Board* b, MoveList* moveList, bool inCheck) {

    moveList->cnt = 0;
    moveList->attackedSquares = attackerSet(b, !b->stm);

    if (inCheck) {
        generateCheckEvasions(b, moveList);
        return;
    }
    Assert(!isCheck(b, b->stm));

    if (b->stm == WHITE) {
        whiteSinglePawnPush(b, moveList);
        whitePawnPushProm(b, moveList);
        whiteDoublePawnPush(b, moveList);
        whitePawnCaptures(b, moveList);
    } else {
        blackSinglePawnPush(b, moveList);
        blackPawnPushProm(b, moveList);
        blackDoublePawnPush(b, moveList);
        blackPawnCaptures(b, moveList);
    }

    addCapturesFor<KNIGHT>(b, moveList);
    addCapturesFor<BISHOP>(b, moveList);
    addCapturesFor<ROOK>(b, moveList);
    addCapturesFor<QUEEN>(b, moveList);

    addNonCapturesFor<KNIGHT>(b, moveList);
    addNonCapturesFor<BISHOP>(b, moveList);
    addNonCapturesFor<ROOK>(b, moveList);
    addNonCapturesFor<QUEEN>(b, moveList);

    addKingMoves(b, moveList);
    addKingCaptures(b, moveList);

}

void generateQuiescence(Board* b, MoveList* moveList, bool inCheck) {

    moveList->cnt = 0;
    moveList->attackedSquares = attackerSet(b, !b->stm);

    if (inCheck) {
        generateCheckEvasions(b, moveList);
        return;
    }
    Assert(!isCheck(b, b->stm));

    if (b->stm == WHITE) {
        whitePawnPushPromQ(b, moveList);
        whitePawnCaptures(b, moveList);

    } else {
        blackPawnPushPromQ(b, moveList);
        blackPawnCaptures(b, moveList);
    }

    addCapturesFor<KNIGHT>(b, moveList);
    addCapturesFor<BISHOP>(b, moveList);
    addCapturesFor<ROOK>(b, moveList);
    addCapturesFor<QUEEN>(b, moveList);

    addKingCaptures(b, moveList);

}

void generateCheckEvasions(Board* b, MoveList* moveList) {
    // Note: no need to generate attackerSet as this function is called in two cases:
    //		1. genQui / genAll: Then attackerSet is already calculated
    //		2. hasEvadingMove(): attackerSet is calculated there

    int kSq = getKingSquare(b, b->stm);
    int blockerSq;
    Bitboard pinnedDefender = getPinned(b, kSq, b->stm);
    Bitboard attacker = squareAttackedBy(b, kSq, !b->stm);

    Assert(isCheck(b, b->stm));
    Assert(popCount(attacker) > 0);

    // Step 1: Single check
    // Either block only sliding attacker ...
    // ... or capture checker with non-pinned pieces
    if (popCount(attacker) < 2) {
        // The attacker, that exclusivly checks stm->king
        int attackerSq = getLSB(attacker);
        int attackerPiece = pieceAt(b, attackerSq);

        // Step 1.1: Blocking
        // If attacker is sliding piece: Get attacking line -> check if a pieceType atkMask can block
        if (pieceRook[attackerPiece] || pieceBishopQueen[attackerPiece]) {
            Bitboard attackingLine = obstructed(kSq, attackerSq) & ~b->occupied;

            while (attackingLine) {
                blockerSq = popLSB(&attackingLine);
                addBlockersForSq(b, moveList, blockerSq, &pinnedDefender);
            }
        }

        // Step 1.2: Capture checker
        // Calculate attacks to square that delivers check (by non-pinned pieces!)
        Bitboard defender = squareAttackedBy(b, attackerSq, b->stm) & ~pinnedDefender;
        Bitboard promDefends = 0ULL;

        // Defenders that defend while promoting with capture
        if (attacker & (RANK_1_HEX | RANK_8_HEX)) {
            promDefends = defender & b->pieces[PAWN] & (RANK_2_HEX | RANK_7_HEX);
        }

        // Remove promotions from standard captures
        defender &= ~promDefends;

        // Non-Promoting moves, that capture the checker
        int defenderSq;
        while (defender) {
            defenderSq = popLSB(&defender);
            moveList->moves[moveList->cnt++] = serializeMove(defenderSq, attackerSq, NORMAL_MOVE, Piece::NO_PIECE);
        }

        // Promoting while capturing the checking piece
        while (promDefends) {
            defenderSq = popLSB(&promDefends);

            moveList->moves[moveList->cnt++] = serializeMove(defenderSq, attackerSq, PROM_MOVE, PROM_TO_QUEEN);
            moveList->moves[moveList->cnt++] = serializeMove(defenderSq, attackerSq, PROM_MOVE, PROM_TO_ROOK);
            moveList->moves[moveList->cnt++] = serializeMove(defenderSq, attackerSq, PROM_MOVE, PROM_TO_BISHOP);
            moveList->moves[moveList->cnt++] = serializeMove(defenderSq, attackerSq, PROM_MOVE, PROM_TO_KNIGHT);
        }

        // If a pawn is threatening check but can be captured en passant
        if (b->enPas != DEFAULT_EP_SQ) {
            if (b->stm == WHITE) {
                if (b->enPas == attackerSq + 8
                    && pieceAt(b, attackerSq - 1) == Piece::P
                    && (1ULL << (attackerSq - 1)) & RANK_5_HEX
                    && (setMask[attackerSq - 1] & ~pinnedDefender)) {
                    moveList->moves[moveList->cnt++] = serializeMove(attackerSq - 1, b->enPas, EP_MOVE, Piece::NO_PIECE);
                }
                if (b->enPas == attackerSq + 8
                    && pieceAt(b, attackerSq + 1) == Piece::P
                    && (1ULL << (attackerSq + 1)) & RANK_5_HEX
                    && (setMask[attackerSq + 1] & ~pinnedDefender)) {
                    moveList->moves[moveList->cnt++] = serializeMove(attackerSq + 1, b->enPas, EP_MOVE, Piece::NO_PIECE);
                }
            } else {
                if (b->enPas == attackerSq - 8
                    && pieceAt(b, attackerSq - 1) == Piece::p
                    && (1ULL << (attackerSq - 1)) & RANK_4_HEX
                    && (setMask[attackerSq - 1] & ~pinnedDefender)) {
                    moveList->moves[moveList->cnt++] = serializeMove(attackerSq - 1, b->enPas, EP_MOVE, Piece::NO_PIECE);
                }
                if (b->enPas == attackerSq - 8
                    && pieceAt(b, attackerSq + 1) == Piece::p
                    && (1ULL << (attackerSq + 1)) & RANK_4_HEX
                    && (setMask[attackerSq + 1] & ~pinnedDefender)) {
                    moveList->moves[moveList->cnt++] = serializeMove(attackerSq + 1, b->enPas, EP_MOVE, Piece::NO_PIECE);
                }
            }
        }
    }

    // Step 2. Always add king evasions to non-attacked squares
    addKingCheckEvasions(b, moveList);
}

void addBlockersForSq(Board* b, MoveList* moveList, int blockingSq, Bitboard* pinnedDefenders) {

    // Set of squares with pieces, that can block blockingSq
    // Pawns are only pushed, atkMask for pawns is not included in blockerSet
    Bitboard blocker = blockerSet(b, b->stm, blockingSq) & ~(*pinnedDefenders);

    // Remove promBlockers from standard blocker set
    Bitboard promRank = (b->stm == WHITE) ? RANK_7_HEX : RANK_2_HEX;
    Bitboard promBlocker = getPieces(b, PAWN, b->stm) & blocker & promRank;
    blocker &= ~promBlocker;

    int piece, sq;
    int flag;

    // Step 1:
    // Add non-capture promotion blocker
    while (promBlocker) {
        sq = popLSB(&promBlocker);
        moveList->moves[moveList->cnt++] = serializeMove(sq, blockingSq, PROM_MOVE, PROM_TO_QUEEN);
        moveList->moves[moveList->cnt++] = serializeMove(sq, blockingSq, PROM_MOVE, PROM_TO_ROOK);
        moveList->moves[moveList->cnt++] = serializeMove(sq, blockingSq, PROM_MOVE, PROM_TO_BISHOP);
        moveList->moves[moveList->cnt++] = serializeMove(sq, blockingSq, PROM_MOVE, PROM_TO_KNIGHT);
    }

    // Step 2:
    // All blocks, that do not promote
    while (blocker) {
        flag = 0;
        sq = popLSB(&blocker);
        piece = pieceAt(b, sq);
        Assert(piece != Piece::k && piece != Piece::K);
        Assert(pieceValid(pieceAt(b, sq)));

        if ((piece == Piece::P || piece == Piece::p)
            && b->enPas != DEFAULT_EP_SQ
            && sq == b->enPas) {
            flag = EP_MOVE;
        }

        moveList->moves[moveList->cnt++] = serializeMove(sq, blockingSq, flag, Piece::NO_PIECE);
    }

}

void generateQuietCheckers(Board* b, MoveList* moveList) {

    Assert(!isCheck(b, b->stm));

    int sq, atkSq;
    int oppkSq = getKingSquare(b, !b->stm);

    Bitboard pieces;
    Bitboard kingBoard = (1ULL << oppkSq);

    // Pawn pushes that check the king
    Bitboard pawns;
    if (b->stm == WHITE) {
        pawns = (getPieces(b, PAWN, WHITE) << 8) & ~b->occupied;

        pieces = pawns;
        while (pieces) {
            sq = popLSB(&pieces);
            if (pawnAtkMask[WHITE][sq] & kingBoard) {
                moveList->moves[moveList->cnt++] = serializeMove(sq - 8, sq, NORMAL_MOVE, Piece::NO_PIECE);
            }
        }

        pieces = (pawns << 8) & ~b->occupied & RANK_4_HEX;
        while (pieces) {
            sq = popLSB(&pieces);
            if (pawnAtkMask[WHITE][sq] & kingBoard) {
                moveList->moves[moveList->cnt++] = serializeMove(sq - 16, sq, NORMAL_MOVE, Piece::NO_PIECE);
            }
        }
    } else {
        pawns = (getPieces(b, PAWN, BLACK) >> 8) & ~b->occupied;

        pieces = pawns;
        while (pieces) {
            sq = popLSB(&pieces);
            if (pawnAtkMask[BLACK][sq] & kingBoard) {
                moveList->moves[moveList->cnt++] = serializeMove(sq + 8, sq, NORMAL_MOVE, Piece::NO_PIECE);
            }
        }

        pieces = (pawns << 8) & ~b->occupied & RANK_5_HEX;
        while (pieces) {
            sq = popLSB(&pieces);
            if (pawnAtkMask[BLACK][sq] & kingBoard) {
                moveList->moves[moveList->cnt++] = serializeMove(sq + 16, sq, NORMAL_MOVE, Piece::NO_PIECE);
            }
        }
    }

    // Sliders that attack king: check if a piece can slide to kingAtkLines -> found quiet check
    Bitboard kingAtkLines = lookUpBishopMoves(oppkSq, b->occupied);
    pieces = getPieces(b, BISHOP, b->stm) | getPieces(b, QUEEN, b->stm);

    Bitboard diagMoves;
    while (pieces) {
        sq = popLSB(&pieces);
        diagMoves  = lookUpBishopMoves(sq, b->occupied) & ~b->occupied;
        diagMoves &= kingAtkLines;

        while (diagMoves) {
            atkSq = popLSB(&diagMoves);
            moveList->moves[moveList->cnt++] = serializeMove(sq, atkSq, NORMAL_MOVE, Piece::NO_PIECE);
        }
    }

    kingAtkLines = lookUpRookMoves(oppkSq, b->occupied);
    Bitboard vertHoriMoves;
    pieces = getPieces(b, ROOK, b->stm) | getPieces(b, QUEEN, b->stm);
    while (pieces) {
        sq = popLSB(&pieces);
        vertHoriMoves  = lookUpRookMoves(sq, b->occupied) & ~b->occupied;
        vertHoriMoves &= kingAtkLines;

        while (vertHoriMoves) {
            atkSq = popLSB(&vertHoriMoves);
            moveList->moves[moveList->cnt++] = serializeMove(sq, atkSq, NORMAL_MOVE, Piece::NO_PIECE);
        }
    }

    // Knight moves that check king
    Bitboard knightChecks;
    Bitboard kingKnightPattern = knightAtkMask[oppkSq];
    pieces = getPieces(b, KNIGHT, b->stm);
    while (pieces) {
        sq = popLSB(&pieces);
        knightChecks = knightAtkMask[sq] & kingKnightPattern & ~b->occupied;

        while (knightChecks) {
            atkSq = popLSB(&knightChecks);
            moveList->moves[moveList->cnt++] = serializeMove(sq, atkSq, NORMAL_MOVE, Piece::NO_PIECE);
        }
    }
}

bool isLegal(Board* b, const Move move) {

    bool inCheck = isCheck(b, b->stm);
    MoveList moveList[1];
    generateMoves(b, moveList, inCheck);

    for (int i = 0; i < moveList->cnt; i++) {
        if (moveList->moves[i] == move) {
            return true;
        }
    }

    return false;
}

Bitboard hasSafePawnPush(Board* b, Color side) {

    int sq;

    Bitboard safeSquares     = 0;
    Bitboard defendedSquares = 0;
    Bitboard pawns           = getPieces(b, PAWN, side);
    Bitboard oppPawns        = getPieces(b, PAWN, !side);

    while (oppPawns) {
        sq = popLSB(&oppPawns);
        defendedSquares |= pawnAtkMask[side ^ 1][sq];
    }

    // Look for: single pawn pushes, non blocked and not defended by opposite pawns
    if (side == WHITE) {
        safeSquares |= (pawns << 8) 
                    & ~b->occupied 
                    & ~defendedSquares;

        pawns &= RANK_2_HEX;
        safeSquares |= (((pawns << 8) & ~b->occupied) << 8) & ~defendedSquares;

    } else {
        safeSquares |= (pawns >> 8) 
                    & ~b->occupied 
                    & ~defendedSquares;

        pawns &= RANK_7_HEX;
        safeSquares |= (((pawns >> 8) & ~b->occupied) >> 8) & ~defendedSquares;
    }

    return safeSquares;
}

bool hasEvadingMove(Board* b) {

    Assert(isCheck(b, b->stm));

    MoveList moveList;
    generateCheckEvasions(b, &moveList);
    return moveList.cnt > 0;
}

void whiteSinglePawnPush(Board* board, MoveList* moveList) {

    int sq;
    Bitboard pushedPawns = (getPieces(board, PAWN, WHITE) << 8) & ~board->occupied;

    // Divide proms and normal pushes
    pushedPawns = pushedPawns & ~RANK_8_HEX;

    // Normal pawn pushes
    while (pushedPawns) {
        sq = popLSB(&pushedPawns);
        moveList->moves[moveList->cnt++] = serializeMove(sq - 8, sq, NORMAL_MOVE, Piece::NO_PIECE);
    }
}

void blackSinglePawnPush(Board* board, MoveList* moveList) {

    int sq;
    Bitboard pushedPawns = (getPieces(board, PAWN, BLACK) >> 8) & ~board->occupied;

    // Divide proms and normal pushes
    pushedPawns = pushedPawns & ~RANK_1_HEX;

    // Normal pawn pushes
    while (pushedPawns) {
        sq = popLSB(&pushedPawns);
        moveList->moves[moveList->cnt++] = serializeMove(sq + 8, sq, NORMAL_MOVE, Piece::NO_PIECE);
    }
}

void whitePawnPushProm(Board* b, MoveList* moveList) {

    Bitboard pawns = getPieces(b, PAWN, WHITE);
    pawns = ((pawns & RANK_7_HEX) << 8) & ~b->occupied;

    int sq;
    while (pawns) {
        sq = popLSB(&pawns);
        moveList->moves[moveList->cnt++] = serializeMove(sq - 8, sq, PROM_MOVE, PROM_TO_QUEEN);
        moveList->moves[moveList->cnt++] = serializeMove(sq - 8, sq, PROM_MOVE, PROM_TO_ROOK);
        moveList->moves[moveList->cnt++] = serializeMove(sq - 8, sq, PROM_MOVE, PROM_TO_BISHOP);
        moveList->moves[moveList->cnt++] = serializeMove(sq - 8, sq, PROM_MOVE, PROM_TO_KNIGHT);
    }
}

void whitePawnPushPromQ(Board* b, MoveList* moveList) {

    Bitboard pawns = getPieces(b, PAWN, WHITE);
    pawns = ((pawns & RANK_7_HEX) << 8) & ~b->occupied;

    int sq;
    while (pawns) {
        sq = popLSB(&pawns);
        moveList->moves[moveList->cnt++] = serializeMove(sq - 8, sq, PROM_MOVE, PROM_TO_QUEEN);
    }
}

void blackPawnPushProm(Board* board, MoveList* moveList) {

    Bitboard pawns = getPieces(board, PAWN, BLACK);
    pawns = ((pawns & RANK_2_HEX) >> 8) & ~board->occupied;

    int sq;
    while (pawns) {
        sq = popLSB(&pawns);
        moveList->moves[moveList->cnt++] = serializeMove(sq + 8, sq, PROM_MOVE, PROM_TO_QUEEN);
        moveList->moves[moveList->cnt++] = serializeMove(sq + 8, sq, PROM_MOVE, PROM_TO_ROOK);
        moveList->moves[moveList->cnt++] = serializeMove(sq + 8, sq, PROM_MOVE, PROM_TO_BISHOP);
        moveList->moves[moveList->cnt++] = serializeMove(sq + 8, sq, PROM_MOVE, PROM_TO_KNIGHT);
    }
}

void blackPawnPushPromQ(Board* board, MoveList* moveList) {

    Bitboard pawns = getPieces(board, PAWN, BLACK);
    pawns = ((pawns & RANK_2_HEX) >> 8) & ~board->occupied;

    int sq;
    while (pawns) {
        sq = popLSB(&pawns);
        moveList->moves[moveList->cnt++] = serializeMove(sq + 8, sq, PROM_MOVE, PROM_TO_QUEEN);
    }
}

void whiteDoublePawnPush(Board* board, MoveList* moveList) {

    Bitboard pushedPawns = (getPieces(board, PAWN, WHITE) << 8) & ~board->occupied;
    pushedPawns = (pushedPawns << 8) & ~board->occupied & RANK_4_HEX;

    int sq;
    while (pushedPawns) {
        sq = popLSB(&pushedPawns);
        moveList->moves[moveList->cnt++] = serializeMove(sq - 16, sq, NORMAL_MOVE, Piece::NO_PIECE);
    }
}

void blackDoublePawnPush(Board* board, MoveList* moveList) {

    Bitboard pushedPawns = (getPieces(board, PAWN, BLACK) >> 8) & ~board->occupied;
    pushedPawns = (pushedPawns >> 8) & ~board->occupied & RANK_5_HEX;

    int sq;
    while (pushedPawns) {
        sq = popLSB(&pushedPawns);
        moveList->moves[moveList->cnt++] = serializeMove(sq + 16, sq, NORMAL_MOVE, Piece::NO_PIECE);
    }
}

void whitePawnCaptures(Board* board, MoveList* moveList) {

    int sq, atkSq;
    Bitboard atks;

    // Divide in prom and non prom attacks
    Bitboard whitePawns    = getPieces(board, PAWN, WHITE);
    Bitboard whitePawnProm = whitePawns & RANK_7_HEX;
    whitePawns &= ~RANK_7_HEX;

    // Enpas square
    if (board->enPas != DEFAULT_EP_SQ) {
        if ((whitePawns << 7 & ~FILE_H_HEX) & (1ULL << board->enPas)) {
            moveList->moves[moveList->cnt++] = serializeMove(board->enPas - 7, board->enPas, EP_MOVE, Piece::NO_PIECE);
        }
        if ((whitePawns << 9 & ~FILE_A_HEX) & (1ULL << board->enPas)) {
            moveList->moves[moveList->cnt++] = serializeMove(board->enPas - 9, board->enPas, EP_MOVE, Piece::NO_PIECE);
        }
    }

    while (whitePawns) {
        sq = popLSB(&whitePawns);
        atks = pawnAtkMask[WHITE][sq] & board->color[BLACK];
        while (atks) {
            atkSq = popLSB(&atks);
            moveList->moves[moveList->cnt++] = serializeMove(sq, atkSq, NORMAL_MOVE, Piece::NO_PIECE);
        }
    }

    // Promoting attacks
    while (whitePawnProm) {
        sq = popLSB(&whitePawnProm);
        atks = pawnAtkMask[WHITE][sq] & board->color[BLACK];
        while (atks) {
            atkSq = popLSB(&atks);
            moveList->moves[moveList->cnt++] = serializeMove(sq, atkSq, PROM_MOVE, PROM_TO_QUEEN);
            moveList->moves[moveList->cnt++] = serializeMove(sq, atkSq, PROM_MOVE, PROM_TO_ROOK);
            moveList->moves[moveList->cnt++] = serializeMove(sq, atkSq, PROM_MOVE, PROM_TO_BISHOP);
            moveList->moves[moveList->cnt++] = serializeMove(sq, atkSq, PROM_MOVE, PROM_TO_KNIGHT);
        }
    }
}

void blackPawnCaptures(Board* board, MoveList* moveList) {

    int sq, atkSq;
    Bitboard atks;

    // Divide in prom and non prom attacks
    Bitboard blackPawns = getPieces(board, PAWN, BLACK);
    Bitboard blackPawnProm = blackPawns & RANK_2_HEX;
    blackPawns &= ~RANK_2_HEX;

    // Enpas square
    if (board->enPas != DEFAULT_EP_SQ) {
        if ((blackPawns >> 7 & ~FILE_A_HEX) & (1ULL << board->enPas)) {
            moveList->moves[moveList->cnt++] = serializeMove(board->enPas + 7, board->enPas, EP_MOVE, Piece::NO_PIECE);
        }
        if ((blackPawns >> 9 & ~FILE_H_HEX) & (1ULL << board->enPas)) {
            moveList->moves[moveList->cnt++] = serializeMove(board->enPas + 9, board->enPas, EP_MOVE, Piece::NO_PIECE);
        }
    }

    while (blackPawns) {
        sq = popLSB(&blackPawns);
        atks = pawnAtkMask[BLACK][sq] & board->color[WHITE];
        while (atks) {
            atkSq = popLSB(&atks);
            moveList->moves[moveList->cnt++] = serializeMove(sq, atkSq, NORMAL_MOVE, Piece::NO_PIECE);
        }
    }

    // Promoting attacks
    while (blackPawnProm) {
        sq = popLSB(&blackPawnProm);
        atks = pawnAtkMask[BLACK][sq] & board->color[WHITE];
        while (atks) {
            atkSq = popLSB(&atks);
            moveList->moves[moveList->cnt++] = serializeMove(sq, atkSq, PROM_MOVE, PROM_TO_QUEEN);
            moveList->moves[moveList->cnt++] = serializeMove(sq, atkSq, PROM_MOVE, PROM_TO_ROOK);
            moveList->moves[moveList->cnt++] = serializeMove(sq, atkSq, PROM_MOVE, PROM_TO_BISHOP);
            moveList->moves[moveList->cnt++] = serializeMove(sq, atkSq, PROM_MOVE, PROM_TO_KNIGHT);
        }
    }
}

void addKingMoves(Board* b, MoveList* moveList) {

    int sq, kSq = getKingSquare(b, b->stm);
    Bitboard kingMoves = kingAtkMask[kSq] & ~moveList->attackedSquares & ~b->occupied;

    while (kingMoves) {
        sq = popLSB(&kingMoves);
        moveList->moves[moveList->cnt++] = serializeMove(kSq, sq, NORMAL_MOVE, Piece::NO_PIECE);
    }

    switch (b->stm) {
        case WHITE:
            if (castleValid(b, K_CASTLE, &moveList->attackedSquares)) {
                Assert(kSq == E1);
                moveList->moves[moveList->cnt++] = serializeMove(kSq, G1, CASTLE_MOVE, Piece::NO_PIECE);
            }
            if (castleValid(b, Q_CASTLE, &moveList->attackedSquares)) {
                Assert(kSq == E1);
                moveList->moves[moveList->cnt++] = serializeMove(kSq, C1, CASTLE_MOVE, Piece::NO_PIECE);
            }
            break;
        case BLACK:
            if (castleValid(b, k_CASTLE, &moveList->attackedSquares)) {
                Assert(kSq == E8);
                moveList->moves[moveList->cnt++] = serializeMove(kSq, G8, CASTLE_MOVE, Piece::NO_PIECE);
            }

            if (castleValid(b, q_CASTLE, &moveList->attackedSquares)) {
                Assert(kSq == E8);
                moveList->moves[moveList->cnt++] = serializeMove(kSq, C8, CASTLE_MOVE, Piece::NO_PIECE);
            }
            break;
        case BW: Assert(false); exit(1);
    }
}

void addKingCaptures(Board* b, MoveList* moveList) {

    int atkSq, kSq = getKingSquare(b, b->stm);
    Bitboard whiteKingAttacks = kingAtkMask[kSq]
                                & b->color[!b->stm]
                                & ~moveList->attackedSquares;

    while (whiteKingAttacks) {
        atkSq = popLSB(&whiteKingAttacks);
        moveList->moves[moveList->cnt++] = serializeMove(kSq, atkSq, NORMAL_MOVE, Piece::NO_PIECE);
    }
}

template<PieceType pType>
void addCapturesFor(Board* b, MoveList* moveList) {

    int sq, atkSq;
    Bitboard attackSet;
    Bitboard pieces = getPieces(b, pType, b->stm);

    while (pieces) {
        sq = popLSB(&pieces);
        attackSet = getMoveMask<pType>(sq, b->occupied);
        attackSet &= b->color[!b->stm];

        while (attackSet) {
            atkSq = popLSB(&attackSet);
            moveList->moves[moveList->cnt++] = serializeMove(sq, atkSq, NORMAL_MOVE, Piece::NO_PIECE);
        }
    }

}

template<PieceType pType>
void addNonCapturesFor(Board* b, MoveList* moveList) {

    int sq, atkSq;
    Bitboard attackSet;
    Bitboard pieces = getPieces(b, pType, b->stm);

    while (pieces) {
        sq = popLSB(&pieces);
        attackSet = getMoveMask<pType>(sq, b->occupied)
                  & ~b->occupied;

        while (attackSet) {
            atkSq = popLSB(&attackSet);
            moveList->moves[moveList->cnt++] = serializeMove(sq, atkSq, NORMAL_MOVE, Piece::NO_PIECE);
        }
    }

}

void printGeneratedMoves(Board* b, MoveList* moveList) {

    cout << "\nGenerated " << moveList->cnt << " moves: " << endl;

    for (int i = 0; i < moveList->cnt; i++) {
        cout << i << ". " << getStringMove(b, moveList->moves[i]) << " score: " << moveList->scores[i] << endl;
    }
}

void addKingCheckEvasions(Board* b, MoveList* moveList) {

    int to;
    int piece = (b->stm) == WHITE ? Piece::K : Piece::k;
    int kSq   = getKingSquare(b, b->stm);

    delPiece(b, piece, kSq, b->stm);
    Bitboard atkSquares = attackerSet(b, !b->stm);
    setPiece(b, piece, kSq, b->stm);

    Bitboard quietSquares = kingAtkMask[kSq] & ~atkSquares & ~b->occupied;
    Bitboard capSquares = kingAtkMask[kSq] & ~atkSquares & b->color[!b->stm];

    // generate quiet check evasions
    while (quietSquares) {
        to = popLSB(&quietSquares);
        moveList->moves[moveList->cnt++] = serializeMove(kSq, to, NORMAL_MOVE, Piece::NO_PIECE);
    }

    // generate capturing check evasions
    while (capSquares) {
        to = popLSB(&capSquares);
        moveList->moves[moveList->cnt++] = serializeMove(kSq, to, NORMAL_MOVE, Piece::NO_PIECE);
    }
}

bool stringIsValidMove(Board* b, std::string stringMove, Move* move) {
    if (stringMove.size() > 5 || stringMove.size() < 4) {
        return false;
    }

    MoveList moveList;
    generateMoves(b, &moveList, isCheck(b, b->stm));

    int parsedMove = parseMove(b, stringMove);
    for (int i = 0; i < moveList.cnt; i++) {
        if (parsedMove == moveList.moves[i]) {
            *move = parsedMove;
            return true;
        }
    }

    return false;
}
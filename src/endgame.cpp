#include "endgame.h"


bool isKvKNB(Board* b) {
    return popCount(b->occupied) == 4 && 
           (   (getPieces(b, Piece::B, WHITE) && getPieces(b, Piece::N, WHITE))
            || (getPieces(b, Piece::b, BLACK) && getPieces(b, Piece::n, BLACK)));
}

bool isKQvKR(Board* b) {
    return popCount(b->occupied) == 4 && 
           (   (getPieces(b, Piece::Q, WHITE) && getPieces(b, Piece::r, BLACK))
            || (getPieces(b, Piece::q, BLACK) && getPieces(b, Piece::R, WHITE)));
}

bool isKvKX(Board* b) {
    return popCount(b->pieces[PAWN]) == 0 && 
          (   popCount(b->color[WHITE]) == 1
           || popCount(b->color[BLACK]) == 1);
}

bool isKQvKP(Board* b) {
    return popCount(b->occupied) == 4 && 
           (   (getPieces(b, Piece::Q, WHITE) && getPieces(b, Piece::p, BLACK))
            || (getPieces(b, Piece::q, BLACK) && getPieces(b, Piece::P, WHITE)));
}

bool isKQvKQ(Board* b) {
    return popCount(b->occupied) == 4 
        && popCount(getPieces(b, QUEEN, WHITE)) 
        && popCount(getPieces(b, QUEEN, BLACK));
}


Value evaluate_KvKNB(Board* b) {

    Value eval = pieceValues[BISHOP] + pieceValues[KNIGHT];

    Color strongColor = (b->material > 0) ? WHITE : BLACK;
    Color bishopColor = (b->pieces[BISHOP] & SQUARES_WHITE) ? WHITE : BLACK;
    Assert(popCount(b->color[strongColor  ]) == 3);
    Assert(popCount(b->color[strongColor^1]) == 1);

    // 1) Minimize distance between kings
    int strongKingSq = getKingSquare(b, strongColor  );
    int weakKingSq   = getKingSquare(b, !strongColor);
    int kDistance    = manhatten[weakKingSq][strongKingSq];

    eval -= 28 - kDistance * 2;

    // 2) Drive weak king towards corner of bishopColor
    eval += cornerReward[bishopColor][weakKingSq] * 2;

    return (b->stm == strongColor) ? eval : -eval;
}

Value evaluate_KQvKR(Board* b) {

    Value eval = pieceValues[QUEEN] - pieceValues[ROOK];
    Color strongColor = (b->material > 0) ? WHITE : BLACK;

    // 1) Minimize distance between kings
    int strongKingSq = getKingSquare(b, strongColor  );
    int weakKingSq   = getKingSquare(b, !strongColor);
    int kDistance    = manhatten[weakKingSq][strongKingSq];

    eval -= 14 - kDistance;

    // 2) Drive weak king towards border
    eval += borderReward[weakKingSq] * 10;
    
    return (b->stm == strongColor) ? eval : -eval;
}

Value evaluate_KQvKQ(Board* b) {

    int kSq1 = getKingSquare(b, WHITE);
    int kSq2 = getKingSquare(b, BLACK);

    int kDistance = manhatten[kSq1][kSq2];
    return kDistance;
}

Value evaluate_KQvKP(Board* b) {

    Value eval = 0;
    Color strongColor = (b->material > 0) ? WHITE : BLACK;

    Assert(b->material != 0);
    Assert(popCount(getPieces(b, PAWN, !strongColor)) == 1);
    Assert(popCount(getPieces(b, QUEEN, strongColor)) == 1);
    Assert(popCount(b->occupied) == 4);

    // 1) Minimize distance between kings
    int strongKingSq = getKingSquare(b, strongColor);
    int weakKingSq   = getKingSquare(b, !strongColor);
    int kDistance    = manhatten[weakKingSq][strongKingSq];

    Assert(kDistance <= 14);
    eval -= 2 * (14 - kDistance);

    // 2) Bishop- or rook-pawns on seventh-rank supported 
    // by king are theoretical draw due to stalemate issues
    Bitboard pawnBoard    = getPieces(b, PAWN, !strongColor);
    Bitboard seventh      = (strongColor == WHITE) ? RANK_2_HEX : RANK_7_HEX;
    Bitboard winningFiles = FILE_B_HEX | FILE_D_HEX | FILE_E_HEX | FILE_G_HEX;

    // Winning case
    if (   pawnBoard & winningFiles
        || pawnBoard & ~seventh
        || !(kingAtkMask[weakKingSq] & pawnBoard)) {

        eval += pieceValues[QUEEN] - pieceValues[PAWN];
    }

    return (b->stm == strongColor) ? eval : -eval;
}

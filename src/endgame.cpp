#include "endgame.h"


namespace chai {

bool isKvKNB(board_t* b) {
    return popCount(b->occupied) == 4 && 
           (   (getPieces(b, Pieces::B, WHITE) && getPieces(b, Pieces::N, WHITE))
            || (getPieces(b, Pieces::b, BLACK) && getPieces(b, Pieces::n, BLACK)));
}

bool isKQvKR(board_t* b) {
    return popCount(b->occupied) == 4 && 
           (   (getPieces(b, Pieces::Q, WHITE) && getPieces(b, Pieces::r, BLACK))
            || (getPieces(b, Pieces::q, BLACK) && getPieces(b, Pieces::R, WHITE)));
}

bool isKvKX(board_t* b) {
    return popCount(b->pieces[PAWN]) == 0 && 
          (   popCount(b->color[WHITE]) == 1
           || popCount(b->color[BLACK]) == 1);
}

bool isKQvKP(board_t* b) {
    return popCount(b->occupied) == 4 && 
           (   (getPieces(b, Pieces::Q, WHITE) && getPieces(b, Pieces::p, BLACK))
            || (getPieces(b, Pieces::q, BLACK) && getPieces(b, Pieces::P, WHITE)));
}

bool isKQvKQ(board_t* b) {
    return popCount(b->occupied) == 4 
        && popCount(getPieces(b, QUEEN, WHITE)) 
        && popCount(getPieces(b, QUEEN, BLACK));
}

// value_t evaluate_KvKX(board_t* b) {
    
//     value_t eval = VALUE_WIN;
//     color_t strongColor = (b->material > 0) ? WHITE : BLACK;

//     // 1) Minimize distance between kings
//     int strongKingSq = getKingSquare(b, strongColor  );
//     int weakKingSq   = getKingSquare(b, strongColor^1);
//     int kDistance    = manhattenDistance[weakKingSq][strongKingSq];

//     eval -= 14 - kDistance;

//     // 2) Drive weak king towards border
//     eval += borderReward[weakKingSq];

//     if (getPieces(b, BISHOP, strongColor)) {
//         color_t bishopColor = (b->pieces[BISHOP] & SQUARES_WHITE) ? WHITE : BLACK;
//         eval += cornerReward[bishopColor][weakKingSq];
//     }

//     return (b->stm == strongColor) ? eval : -eval;

// }

value_t evaluate_KvKNB(board_t* b) {

    value_t eval = pieceValues[BISHOP] + pieceValues[KNIGHT];

    color_t strongColor = (b->material > 0) ? WHITE : BLACK;
    color_t bishopColor = (b->pieces[BISHOP] & SQUARES_WHITE) ? WHITE : BLACK;
    Assert(popCount(b->color[strongColor  ]) == 3);
    Assert(popCount(b->color[strongColor^1]) == 1);

    // 1) Minimize distance between kings
    int strongKingSq = getKingSquare(b, strongColor  );
    int weakKingSq   = getKingSquare(b, strongColor^1);
    int kDistance    = manhattenDistance[weakKingSq][strongKingSq];

    eval -= 28 - kDistance * 2;

    // 2) Drive weak king towards corner of bishopColor
    eval += cornerReward[bishopColor][weakKingSq] * 2;

    return (b->stm == strongColor) ? eval : -eval;
}

value_t evaluate_KQvKR(board_t* b) {

    value_t eval = pieceValues[QUEEN] - pieceValues[ROOK];
    color_t strongColor = (b->material > 0) ? WHITE : BLACK;

    // 1) Minimize distance between kings
    int strongKingSq = getKingSquare(b, strongColor  );
    int weakKingSq   = getKingSquare(b, strongColor^1);
    int kDistance    = manhattenDistance[weakKingSq][strongKingSq];

    eval -= 14 - kDistance;

    // 2) Drive weak king towards border
    eval += borderReward[weakKingSq] * 10;
    
    return (b->stm == strongColor) ? eval : -eval;
}

value_t evaluate_KQvKQ(board_t* b) {

    int kSq1 = getKingSquare(b, WHITE);
    int kSq2 = getKingSquare(b, BLACK);

    int kDistance = manhattenDistance[kSq1][kSq2];
    return kDistance;
}

value_t evaluate_KQvKP(board_t* b) {

    value_t eval = 0;
    color_t strongColor = (b->material > 0) ? WHITE : BLACK;

    Assert(b->material != 0);
    Assert(popCount(getPieces(b, PAWN, strongColor^1)) == 1);
    Assert(popCount(getPieces(b, QUEEN, strongColor)) == 1);
    Assert(popCount(b->occupied) == 4);

    // 1) Minimize distance between kings
    int strongKingSq = getKingSquare(b, strongColor);
    int weakKingSq   = getKingSquare(b, strongColor^1);
    int kDistance    = manhattenDistance[weakKingSq][strongKingSq];

    Assert(kDistance <= 14);
    eval -= 14 - kDistance;

    // 2) Bishop- or rook-pawns on seventh-rank supported 
    // by king are theoretical draw due to stalemate issues
    bitboard_t pawnBoard    = getPieces(b, PAWN, strongColor^1);
    bitboard_t seventh      = (strongColor == WHITE) ? RANK_2_HEX : RANK_7_HEX;
    bitboard_t winningFiles = FILE_B_HEX | FILE_D_HEX | FILE_E_HEX | FILE_G_HEX;

    // Winning case
    if (   pawnBoard & winningFiles
        || pawnBoard & ~seventh
        || !(kingAtkMask[weakKingSq] & pawnBoard)) {

        eval += pieceValues[QUEEN] - pieceValues[PAWN];
    }

    return (b->stm == strongColor) ? eval : -eval;
}


} // namespace chai
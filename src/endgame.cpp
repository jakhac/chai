#include "endgame.h"


bool isKvKNB(board_t* b) {
    return popCount(b->occupied) == 4 && 
           (   (getPieces(b, Piece::B, WHITE) && getPieces(b, Piece::N, WHITE))
            || (getPieces(b, Piece::b, BLACK) && getPieces(b, Piece::n, BLACK)));
}

bool isKQvKR(board_t* b) {
    return popCount(b->occupied) == 4 && 
           (   (getPieces(b, Piece::Q, WHITE) && getPieces(b, Piece::r, BLACK))
            || (getPieces(b, Piece::q, BLACK) && getPieces(b, Piece::R, WHITE)));
}

bool isKvKX(board_t* b) {
    return popCount(b->pieces[cPAWN]) == 0 && 
          (   popCount(b->color[WHITE]) == 1
           || popCount(b->color[BLACK]) == 1);
}

bool isKQvKP(board_t* b) {
    return popCount(b->occupied) == 4 && 
           (   (getPieces(b, Piece::Q, WHITE) && getPieces(b, Piece::p, BLACK))
            || (getPieces(b, Piece::q, BLACK) && getPieces(b, Piece::P, WHITE)));
}

bool isKQvKQ(board_t* b) {
    return popCount(b->occupied) == 4 
        && popCount(getPieces(b, cQUEEN, WHITE)) 
        && popCount(getPieces(b, cQUEEN, BLACK));
}


value_t evaluate_KvKNB(board_t* b) {

    value_t eval = pieceValues[cBISHOP] + pieceValues[cKNIGHT];

    color_t strongColor = (b->material > 0) ? WHITE : BLACK;
    color_t bishopColor = (b->pieces[cBISHOP] & SQUARES_WHITE) ? WHITE : BLACK;
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

value_t evaluate_KQvKR(board_t* b) {

    value_t eval = pieceValues[cQUEEN] - pieceValues[cROOK];
    color_t strongColor = (b->material > 0) ? WHITE : BLACK;

    // 1) Minimize distance between kings
    int strongKingSq = getKingSquare(b, strongColor  );
    int weakKingSq   = getKingSquare(b, !strongColor);
    int kDistance    = manhatten[weakKingSq][strongKingSq];

    eval -= 14 - kDistance;

    // 2) Drive weak king towards border
    eval += borderReward[weakKingSq] * 10;
    
    return (b->stm == strongColor) ? eval : -eval;
}

value_t evaluate_KQvKQ(board_t* b) {

    int kSq1 = getKingSquare(b, WHITE);
    int kSq2 = getKingSquare(b, BLACK);

    int kDistance = manhatten[kSq1][kSq2];
    return kDistance;
}

value_t evaluate_KQvKP(board_t* b) {

    value_t eval = 0;
    color_t strongColor = (b->material > 0) ? WHITE : BLACK;

    Assert(b->material != 0);
    Assert(popCount(getPieces(b, cPAWN, !strongColor)) == 1);
    Assert(popCount(getPieces(b, cQUEEN, strongColor)) == 1);
    Assert(popCount(b->occupied) == 4);

    // 1) Minimize distance between kings
    int strongKingSq = getKingSquare(b, strongColor);
    int weakKingSq   = getKingSquare(b, !strongColor);
    int kDistance    = manhatten[weakKingSq][strongKingSq];

    Assert(kDistance <= 14);
    eval -= 2 * (14 - kDistance);

    // 2) Bishop- or rook-pawns on seventh-rank supported 
    // by king are theoretical draw due to stalemate issues
    bitboard_t pawnBoard    = getPieces(b, cPAWN, !strongColor);
    bitboard_t seventh      = (strongColor == WHITE) ? RANK_2_HEX : RANK_7_HEX;
    bitboard_t winningFiles = FILE_B_HEX | FILE_D_HEX | FILE_E_HEX | FILE_G_HEX;

    // Winning case
    if (   pawnBoard & winningFiles
        || pawnBoard & ~seventh
        || !(kingAtkMask[weakKingSq] & pawnBoard)) {

        eval += pieceValues[cQUEEN] - pieceValues[cPAWN];
    }

    return (b->stm == strongColor) ? eval : -eval;
}

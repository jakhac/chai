#include "validate.h"

int squareOnBoard(int sq) {
    return ((sq >= 0) && (sq <= 64));
}

bool pieceValid(int piece) {
    return ((piece >= 1) && (piece <= 13));
}

bool pieceValidPromotion(int piece) {
    return piece == Piece::q || piece == Piece::r
        || piece == Piece::b || piece == Piece::n
        || piece == Piece::Q || piece == Piece::R
        || piece == Piece::B || piece == Piece::N;
}

bool fileValid(int file) {
    return (file >= 0) && (file <= 7);
}

bool pseudoValidBitMove(Move move) {
    if (move != MOVE_NULL && move != MOVE_NONE) {
        return true;
    }

    return false;
}

bool validEnPasSq(int sq) {
    return validEnPas[sq];
}

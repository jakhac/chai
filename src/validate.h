#pragma once

#include "defs.h"
#include "move.h"

const bool validEnPas[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

/**
 * Checks if index is square on board.
 *
 * @param  sq Square to check.
 *
 * @returns True if square is valid.
 */
int squareOnBoard(int sq);

/**
 * Checks if piece has valid index, where [1, 13] ist valid and zero is non valid.
 *
 * @param  piece Piece to check.
 *
 * @returns True if piece is valid.
 */
bool pieceValid(int piece);

bool pieceValidPromotion(int piece);

/**
 * File valid
 *
 * @param  file The file to check.
 *
 * @returns True if it succeeds, false if it fails.
 */
bool fileValid(int file);

/**
 * Checks if move is peudo-legal
 *
 * @param move The move to check
 *
 * @returns True if move is neither NULL_MOVE, NO_MOVE and from and to square are legal.
 */
bool pseudoValidBitMove(Move move);

bool validEnPasSq(int sq);
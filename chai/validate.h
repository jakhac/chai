#pragma once

#include "defs.h"

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
int pieceValid(int piece);

/**
 * File valid
 *
 * @param  file The file to check.
 *
 * @returns True if it succeeds, false if it fails.
 */
bool fileValid(int file);

//TODO
bool pseudoValidBitMove(int move);

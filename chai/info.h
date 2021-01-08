#pragma once

#include "defs.h"
#include "mask.h"
#include <iostream>
#include <bitset>

/**
 * Print referenced bitboard.
 *
 * @param  bb Bitboard.
 */
void printBitBoard(bitboard_t* bb);

/**
 * Print move in algebraic notation and promotions if possible
 *
 * @param  move Move.
 */
void printMove(const int move);

/**
 * Returns the algebraic notation of given move.
 *
 * @param  move The move.
 *
 * @returns The move in algebraic notation.
 */
string getStringMove(const int move);

/**
 * Print all flags and attributes of given move.
 *
 * @param  move Move.
 */
void printMoveStatus(int move);

/**
 * Print binary format of given integer.
 *
 * @param  x A bitboard_t to process.
 */
void printBinary(bitboard_t x);
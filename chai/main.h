#pragma once

#include <algorithm>
#include <iostream>
#include <bitset>
#include <string>
#include <chrono>
#include <iostream>

#include "board.h"
#include "defs.h"
#include "moveGenerator.h"
#include "legalMoveGenerator.h"
#include "perft.h"
#include "uci.h"
#include "search.h"

static board_t board;

static Perft perft;

static moveList_t move_s[1];

static search_t s[1];

static board_t* b = &board;

/**
 * Main function to run chess in console.
 *
 * @returns Exit-code for the process - 0 for success, else an error code.
 */
int main();

/**
 * Start a game in console.
 *
 * @param  b board_t instance.
 * @param  p MoveGenrator instance.
 * @param  s A search_t to process.
 */
void cli(board_t* b, Perft* p, search_t* s);

/**
 * Function to divide with cmd-line move after each perft.
 *
 * @param  b	 board_t reference.
 * @param  fen   The fen.
 * @param  depth The initial depth.
 */
void dividePerft(board_t* b, int depth);


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


static Board board;

static Perft perft;

static moveList_t move_s[1];

static search_t s[1];

/**
 * Main function to run chess in console.
 *
 * @returns Exit-code for the process - 0 for success, else an error code.
 */
int main();

/**
 * Start a game in console.
 *
 * @param  b Board instance.
 * @param  p MoveGenrator instance.
 * @param  s A search_t to process.
 */
void play(Board* b, Perft* p, search_t* s);

/**
 * Function to divide with cmd-line move after each perft.
 *
 * @param  b	 Board reference.
 * @param  fen   The fen.
 * @param  depth The initial depth.
 */
void dividePerft(Board* b, string fen, int depth);


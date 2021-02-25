#pragma once

#include <iostream>
#include <string>
#include <algorithm>
#include <chrono>
#include <string>
#include <sstream>
#include "io.h"

#include "board.h"
#include "search.h"

#ifndef VERSION
#define VERSION "v3.x"
#endif // !VERSION

/**
 * Enters UCI mode. Described in http://wbec-ridderkerk.nl/html/UCIProtocol.html
 *
 * @param  b A Board to process.
 * @param  s A search_t to process.
 */
void uciMode(Board* b, search_t* s);

/**
 * Parse a position from UCI command.
 *
 * @param  b   A Board to process.
 * @param  cmd The command.
 */
void uciParsePosition(Board* b, string cmd);

/**
 * Parse search information from UCI go command.
 *
 * @param  b   A Board to process.
 * @param  s   A search_t to process.
 * @param  cmd The command.
 */
void uciParseGo(Board* b, search_t* s, string cmd);

/**
 * Complete initialization for chess engine. Called once at startup.
 *
 * @param  b Board.
 */
void init(Board* b);

#pragma once


//#include <iostream>
//#include <string>
//#include <algorithm>
//#include <chrono>
//#include <string>
//#include <sstream>
//#include "io.h"

#include <iterator>

//#include "board.h"
#include "search.h"

#ifndef VERSION
#define VERSION "v2.5.5"
#endif // !VERSION

/**
 * Enters UCI mode. Described in http://wbec-ridderkerk.nl/html/UCIProtocol.html
 *
 * @param  b A board_t to process.
 * @param  s A search_t to process.
 */
void uciMode(board_t* b, search_t* s);

void uciSetOption(board_t* b, string cmd);

/**
 * Parse a position from UCI command.
 *
 * @param  b   A board_t to process.
 * @param  cmd The command.
 */
void uciParsePosition(board_t* b, string cmd);

/**
 * Parse search information from UCI go command.
 *
 * @param  b   A board_t to process.
 * @param  s   A search_t to process.
 * @param  cmd The command.
 */
void uciParseGo(board_t* b, search_t* s, string cmd);

/**
 * Complete initialization for chess engine. Called once at startup.
 *
 * @param  b board_t.
 */
void init(board_t* b);

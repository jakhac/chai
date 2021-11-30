#pragma once

#include <iterator>
#include <sstream>

#include "search.h"
#include "timeMan.h"

#ifndef VERSION
#define VERSION 0
#endif // !VERSION

/**
 * Enters UCI mode. Described in http://wbec-ridderkerk.nl/html/UCIProtocol.html
 *
 * @param  b A board_t to process.
 * @param  s A stats_t to process.
 */
void uciMode(board_t* b, stats_t* s, instr_t* i);

void uciSetOption(std::string cmd);

/**
 * Parse a position from UCI command.
 *
 * @param  b   A board_t to process.
 * @param  cmd The command.
 */
void uciParsePosition(board_t* b, std::string cmd);

/**
 * Parse search information from UCI go command.
 *
 * @param  b   A board_t to process.
 * @param  s   A stats_t to process.
 * @param  cmd The command.
 */
void uciParseGo(board_t* b, stats_t* s, instr_t* instr, std::string cmd);

/**
 * Complete initialization for chess engine. Called once at startup.
 */
void init();

#pragma once

#include <iterator>
#include <sstream>

#include "search.h"
#include "timeMan.h"
#include "perft.h"


#ifndef VERSION
#define VERSION 0
#endif // !VERSION


inline bool strStartsWith(std::string str, std::string start) {
	return str.rfind(start, 0) == 0;
}


namespace UCI {


/**
 * Start a game in console.
 */
void cli(board_t* b, instr_t* i, stats_t* s, Perft* p);

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


}

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

inline void parseMoveList(Board* b, std::string moves) {
    std::istringstream iss(moves);
    std::vector<std::string> tokens{ std::istream_iterator<std::string>{iss},
                        std::istream_iterator<std::string>{} };

    // Push all moves
    int parsedMove;
    for (std::string m : tokens) {
        parsedMove = parseMove(b, m);
        push(b, parsedMove);

        // Push increases ply by default. We have to keep it at zero,
        // otherwise some of our arrays are indexed out-of-bounds!
        b->ply = 0;
    }
}


namespace UCI {


/**
 * Start a game in console.
 */
void cli(Board* b, Instructions* i, Stats* s);

/**
 * Enters UCI mode. Described in http://wbec-ridderkerk.nl/html/UCIProtocol.html
 *
 * @param  b A Board to process.
 * @param  s A Stats to process.
 */
void uciMode(Board* b, Stats* s, Instructions* i);

void uciSetOption(std::string cmd);

/**
 * Parse a position from UCI command.
 *
 * @param  b   A Board to process.
 * @param  cmd The command.
 */
void uciParsePosition(Board* b, std::string cmd);

/**
 * Parse search information from UCI go command.
 *
 * @param  b   A Board to process.
 * @param  s   A Stats to process.
 * @param  cmd The command.
 */
void uciParseGo(Board* b, Stats* s, Instructions* instr, std::string cmd);


}

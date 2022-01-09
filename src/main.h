#pragma once

#include "perft.h"
#include "uci.h"
#include "search.h"


#ifdef ASSERT
static std::string info_ASSERT = "1";
#else
static std::string info_ASSERT = "0";
#endif // ASSERT

// TODO better way to obain info and distribute to main
#if defined(_MSC_VER)
static std::string info_COMPILER = "MSVC";
#elif defined(__GNUC__)
static std::string info_COMPILER = "GNU";
#endif

#if defined(USE_AVX2)
static std::string info_SIMD = "AVX2";
#elif defined(USE_SSSE3)
static std::string info_SIMD = "SSSE3";
#else
static std::string info_SIMD = "NATIVE";
#endif


// These are the main variables used in the engine.
static board_t _board;
static Perft _perft;
static instr_t _instr;
static stats_t _stats;

/**
 * Main function to run chai in console.
 *
 * @returns Exit-code for the process - 0 for success, else an error code.
 */
int main();

/**
 * Start a game in console.
 */
void cli(board_t* b, instr_t* i, stats_t* s, Perft* p);

/**
 * Function to divide with cmd-line move after each perft.
 *
 * @param  b	 board_t reference.
 * @param  fen   The fen.
 * @param  depth The initial depth.
 */
void dividePerft(Perft* p, board_t* b, int depth);


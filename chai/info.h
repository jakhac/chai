#pragma once

#include <iostream>
#include <bitset>
#include "io.h"
#include <fstream>
#include <chrono>  // chrono::system_clock
#include <ctime>   // localtime
#include <iomanip> // put_time
#include "windows.h"

#include "defs.h"
#include "mask.h"
#include "board.h"
#include "tt.h"

using namespace std;

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

/**
 * Print UCI info to console.
 *
 * @param s A search info struct.
 * @param d currentDepth.
 * @param selDpt selDepth.
 * @param score bestScore returned by search.
 */
void printUCI(search_t* s, int d, int selDpt, int score);

/**
 * Print pv line to console. Either hash-line form ttable probing or pvline struct.
 *
 * @param b The current board.
 * @param pvLine pvLine struct filled by alphaBeta search.
 */
void printPV(move_t* moves, int len);

void printTTablePV(board_t* b, int depth, int selDepth);
/**
 * Write logging information into log.txt file.
 *
 * @param  logMsg Message to write into log file.
 */
void log(string logMsg);

/**
 * Gets the time.
 *
 * @returns The time.
 */
string getTime();

/**
 * Read input from command line during search. Sets stop to true,
 * if search has to be aborted.
 *
 * @param  s The search info.
 */
void readInput(search_t* s);

/**
 * Determines if we can input waiting
 *
 * @returns True if input is waiting, else false.
 */
bool inputWaiting();
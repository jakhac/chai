#pragma once

#include "defs.h"
#include "mask.h"
#include "board.h"

#include <iostream>
#include <bitset>
#include "io.h"
#include <fstream>
#include <chrono>  // chrono::system_clock
#include <ctime>   // localtime
#include <iomanip> // put_time
#include "windows.h"

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
 * Print search information to console.
 *
 * @param  b Current board.
 * @param  s This search info printed to console.
 */
void printSearchInfo(Board* b, search_t* s);

/**
 * Write logging information into log.txt file.
 *
 * @param  logMsg Message to write into log file.
 */
void log(string logMsg);

/**
 * Gets the time
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
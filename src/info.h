#pragma once

#include "io.h" // _read()

#include "mask.h"
#include "board.h"
#include "tt.h"
#include "eval.h"
#include "thread.h"

//using namespace std;

/**
 * Print referenced bitboard.
 *
 * @param  bb Bitboard.
 */
void printBitBoard(bitboard_t* bb);

/**
 * Print initial UCI info (Author, options, ...).
 */
void printUCI_Info();

/**
 * Print move in algebraic notation and promotions if possible
 *
 * @param  move Move.
 */
void printMoveStatus(board_t* b, move_t move);

/**
 * Returns the algebraic notation of given move.
 *
 * @param  move The move.
 *
 * @returns The move in algebraic notation.
 */
std::string getStringMove(board_t* b, const move_t move);

///**
// * Print all flags and attributes of given move.
// *
// * @param  move Move.
// */
//void printMoveStatus(int move);

/**
 * Print binary format of given integer.
 *
 * @param  x A bitboard_t to process.
 */
void printBinary(bitboard_t x);

void printEngineMeta(std::string assert, std::string compiler, std::string simd);

/**
 * Print UCI info to console.
 *
 * @param s A search info struct.
 * @param d currentDepth.
 * @param selDpt selDepth.
 * @param score bestScore returned by search.
 */
void printUCI(instr_t* instr, stats_t* s, int d, int selDpt, int score, long totalNodes);

void printUCIBestMove(board_t* b, move_t bestMove);

void printCliHelp();

/**
 * Print pv line to console. Either hash-line form ttable probing or pvline struct.
 *
 * @param b The current board.
 * @param pvLine pvLine struct filled by alphaBeta search.
 */
void printPV(board_t* b, move_t* moves, int len);

void printTTablePV(board_t* b, int depth);

void printPvLine(board_t* b, move_t* pvLine, int d, int score);
/**
 * Write logging information into log.txt file.
 *
 * @param  logMsg Message to write into log file.
 */
void log(std::string logMsg);

/**
 * Read input from command line during search. Sets stop to true,
 * if search has to be aborted.
 */
// void readInput(stats_t* s);

/**
 * Determines if we can input waiting
 *
 * @returns True if input is waiting, else false.
 */
bool inputWaiting();
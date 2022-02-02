#pragma once

#include "io.h" // _read()

#include "board.h"
#include "tt.h"
#include "eval.h"
#include "thread.h"

/**
 * Print referenced bitboard.
 */
void printBitBoard(bitboard_t* bb);

/**
 * Print initial UCI info (Author, options, ...).
 */
void printUCI_Info();

/**
 * @brief Print evaluation of current board.
 */
void printEval(board_t* b);

/**
 * Print move in algebraic notation and promotions if possible
 */
void printMoveStatus(board_t* b, move_t move);

/**
 * @brief Returns the algebraic notation of given move.
 */
std::string getStringMove(board_t* b, const move_t move);

/**
 * @brief Print binary format of given integer.
 */
void printBinary(bitboard_t x);

/**
 * @brief Print engine meta to console.
 */
void printEngineMeta(std::string assert, std::string compiler, std::string simd);

/**
 * @brief Print UCI info to console.
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
 * @brief Print pv line to console. Either hash-line form ttable probing or pvline struct.
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
 * @brief Parse a given fen into board variables and print board. Updates zobristKey of board to new
 * generated zobristKey.
 *
 * @param  b board_t to call function.
 * @param  fen FEN Notation string of board.
 * @returns bool true if error occured or FEN invalid, else false.
 */
bool parseFen(board_t* b, std::string fen);

/**
 * @brief Return FEN from current position.
 */
std::string getFEN(board_t* b);

/**
 * Parse a move into a bit move. Sets flags and captures according to current board state.
 *
 * @param  b board_t to call function.
 * @param  move Move in algebraic notation.
 *
 * @returns int move.
 */
move_t parseMove(board_t* b, std::string move);
#pragma once

#include "board.h"
#include "tt.h"
#include "eval.h"
#include "thread.h"

/**
 * Print referenced bitboard.
 */
void printBitBoard(Bitboard* bb);

/**
 * Print initial UCI info (Author, options, ...).
 */
void printUCI_Info();

/**
 * @brief Print evaluation of current board.
 */
void printEval(Board* b);

/**
 * Print move in algebraic notation and promotions if possible
 */
void printMoveStatus(Board* b, Move move);

/**
 * @brief Returns the algebraic notation of given move.
 */
std::string getStringMove(Board* b, const Move move);

/**
 * @brief Print binary format of given integer.
 */
void printBinary(Bitboard x);

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
void printUCI(Instructions* instr, Stats* s, int d, int selDpt, int score, long totalNodes);
void printUCIBestMove(Board* b, Move bestMove);

void printCliHelp();

/**
 * @brief Print pv line to console. Either hash-line form ttable probing or pvline struct.
 *
 * @param b The current board.
 * @param pvLine pvLine struct filled by alphaBeta search.
 */
void printPV(Board* b, Move* moves, int len);
void printTTablePV(Board* b, int depth, int score);
void printPvLine(Board* b, Move* pvLine, int d, int score);

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
 * @param  b Board to call function.
 * @param  fen FEN Notation string of board.
 * @returns int number of characters parsed
 */
int parseFen(Board* b, std::string fen);

/**
 * @brief Return FEN from current position.
 */
std::string getFEN(Board* b);

/**
 * Parse a move into a bit move. Sets flags and captures according to current board state.
 *
 * @param  b Board to call function.
 * @param  move Move in algebraic notation.
 *
 * @returns int move.
 */
Move parseMove(Board* b, std::string move);
#pragma once

#include "board.h"
#include "moveGenerator.h"

/// <summary>Size of the transposition table</summary>
const int ttSize = 0x100000 * 128;

/// <summary>Size of the pawn table</summary>
const int pawnTableSize = 0x100000 * 8;

/// <summary>
/// Initialize transposition table: Clear used or allocated memory and re-allocate.
/// </summary>
/// <param name="tt">Transposition table</param>
void initTT(ttable_t* pvTable_s);

/// <summary>
/// Reset all variables in transposition table. Only used in initialization or new game.
/// </summary>
/// <param name="tt">Transposition table</param>
void clearTT(ttable_t* pvTable_s);

/// <summary>
/// Store a transposition entry containing score, score flag, move and depth in 
/// in transposition table if possible.
/// </summary>
/// <param name="b">Current board</param>
/// <param name="move">Best move found for current board</param>
/// <param name="score">Best score for current board</param>
/// <param name="flag">Flag for score type</param>
/// <param name="depth">Depth used in search for move and score</param>
void storeTT(Board* b, int move, int score, int flag, int depth);

int probePV(Board* b);

/// <summary>
/// Probe the transposition table. Always set the pv move, even if depth is not deep enough.
/// If stored depth is greater equal to the current depth, overwrite score reference 
/// if current score cannot beat alpha/beta anymore.
/// </summary>
/// <param name="b">Current board</param>
/// <param name="move">Reference to move variable used in search</param>
/// <param name="score">Reference to score variable used in search</param>
/// <param name="alpha">Current lower bound found in search</param>
/// <param name="beta">Current upper bound found in search</param>
/// <param name="depth">Current depth of search</param>
/// <returns>True if probing was successfull and alpha/beta/score was improved</returns>
bool probeTT(Board* b, int* move, int* score, int alpha, int beta, int depth);

/// <summary>
/// Walk through best move stored in transposition table to collect principal variation line.
/// </summary>
/// <param name="b">Current board</param>
/// <param name="maxDepth">Depth of principal variation</param>
/// <returns>Length of principal variation found in ttable</returns>
int getPVLine(Board* b, const int maxDepth);

/// <summary>
/// Initialize pawn table. Re-allocate memory.
/// </summary>
/// <param name="pawnTable">Pawn table declared in board</param>
void initPawnTable(pawntable_t* pawnTable);

/// <summary>
/// Reset all variables used in pawn table. Only used before new game.
/// </summary>
/// <param name="pawnTable">Pawn table to clear</param>
void clearPawnTable(pawntable_t* pawnTable);

/// <summary>
/// Store a pawn table entry with pawn evaluation score.
/// </summary>
/// <param name="b">Current board</param>
/// <param name="eval">Score for pawn structure</param>
void storePawnEntry(Board* b, const int eval);

/// <summary>
/// Probe pawn table and return score if found.
/// </summary>
/// <param name="b">Current board</param>
/// <returns></returns>
int probePawnEntry(Board* b);
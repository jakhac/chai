#pragma once

#include <chrono>
#include <iomanip>

#include "tt.h"
#include "eval.h"

/// <summary>Maximum ply reached in alphaBeta and quiesence search</summary>
extern int selDepth;

/// <summary>
/// Alpha beta algorithm root. Searches current board for best move and score.
/// </summary>
/// <param name="alpha">Lower bound, minimum score the side is assured of</param>
/// <param name="beta">Upper bound, maximum score the opponent is assured of</param>
/// <param name="depth">Current depth</param>
/// <param name="b">Current board</param>
/// <param name="s">Search info containing search parameters</param>
/// <param name="nullOk">Enables null move pruning in node</param>
/// <param name="pvNode">Determines the current node type</param>
/// <returns>Best score found in search</returns>
int alphaBeta(int alpha, int beta, int depth, Board* b, search_t* s, bool nullOk, bool pvNode);

/// <summary>
/// Quiesence search pushes all captures to evaluate a stable and quiet position.
/// </summary>
/// <param name="alpha">Lower bound, minimum score the side is assured of</param>
/// <param name="beta">Upper bound, maximum score the opponent is assured of</param>
/// <param name="b">Current board</param>
/// <param name="s">Search info containing search parameters</param>
/// <returns>Best score found in quiesences search</returns>
int quiesence(int alpha, int beta, Board* b, search_t* s);

/// <summary>
/// Root function that starts alphaBeta search in iterative deepening framework.
/// </summary>
/// <param name="b">Reference to current board</param>
/// <param name="s">Search info containing search parameters</param>
int search(Board* b, search_t* s);

/// <summary>
/// Apply offset of aspiration windows in alphaBeta call and try to score inside alpha and beta.
/// If score is outside of bounds, do a research with infinite bounds.
/// </summary>
/// <param name="b">Current board</param>
/// <param name="s">Search info containing search parameters</param>
/// <param name="depth">Current depth for alphaBeta call</param>
/// <param name="bestScore">Best score from previous search to determine aspiration windows</param>
/// <returns>Best score found in alphaBeta search</returns>
int search_aspiration(Board* b, search_t* s, int depth, int bestScore);

/// <summary>
/// Tries to find a single(!) repetition in undoPly array. Used in search to find upcoming draws.
/// </summary>
/// <param name="b">Current board</param>
/// <returns>True if repetition is found in undoPly array, else false</returns>
bool isRepetition(Board* b);

/// <summary>
/// Searches the complete undoPly array for a position (including current position) 
/// that occurred three times.
/// </summary>
/// <param name="b">Current board</param>
/// <returns>True if three fold repetition is found, else false</returns>
bool isThreeFoldRepetition(Board* b);

/// <summary>
/// Swaps the best move found in moveList with move at current index.
/// </summary>
/// <param name="b">Current board</param>
/// <param name="move_s">Move struct with all moves and scores</param>
/// <param name="curIdx">Current index in moveList</param>
void moveSwapper(Board* b, moveList_t* move_s, int curIdx);

/// <summary>
/// Swap to moves and their score in given MOVE_S struct. Returns if id1 is equal to id2.
/// </summary>
/// <param name="move_s">Move struct with all moves and scores</param>
/// <param name="id1">First index</param>
/// <param name="id2">Second index</param>
void swapMove(moveList_t* move_s, int id1, int id2);


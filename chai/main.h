#pragma once

#include "board.h"
#include "defs.h"
#include "moveGenerator.h"
#include "legalMoveGenerator.h"
#include "perft.h"

#include <algorithm>
#include <iostream>
#include <bitset>
#include <string>
#include <chrono>

/// <summary>
/// Main function to run chess in console.
/// </summary>
int main();

/// <summary>
/// Initialize keys, masks and magic bitboards.
/// </summary>
/// <param name="b">Board</param>
void init(Board* b);

/// <summary>
/// Start a game in console.
/// </summary>
/// <param name="b">Board instance</param>
/// <param name="moveGenerator">MoveGenrator instance</param>
void play(Board b, MoveGenerator moveGenerator);


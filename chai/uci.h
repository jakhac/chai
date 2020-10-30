#pragma once

#include <iostream>
#include <string>
#include <algorithm>
#include <chrono>
#include <string>
#include <sstream>
#include "io.h"

#include "board.h"
#include "search.h"

void uciMode(Board* b, SEARCH_S* s);

void uciParsePosition(Board* b, string cmd);

void uciParseGo(Board* b, SEARCH_S* s, string cmd);

/// <summary>
/// Initialize keys, masks and magic bitboards.
/// </summary>
/// <param name="b">Board</param>
void init(Board* b);

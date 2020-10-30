#pragma once

#include <chrono>
#include <iomanip>

#include "tt.h"
#include "eval.h"

using namespace std;

extern int selDepth;

extern int rootDepth;

int alphaBeta(int alpha, int beta, int depth, Board* b, SEARCH_S* s, bool nullOk, bool pvNode);

int quiesence(int alpha, int beta, Board* b, SEARCH_S* s);

void search(Board* b, SEARCH_S* s);

int search_aspiration(Board* b, SEARCH_S* s, int depth, int bestScore);

bool isRepetition(Board* b);

bool isThreeFoldRepetition(Board* b);

void moveSwapper(Board* b, MOVE_S* move_s, int curIdx);

void swapMove(MOVE_S* move_s, int id1, int id2);


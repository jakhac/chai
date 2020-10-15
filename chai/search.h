#pragma once

#include <chrono>
#include <iomanip>

#include "tt.h"
#include "eval.h"

using namespace std;

bool nullValid(Board* b, bool inCheck);

int alphaBeta(int alpha, int beta, int depth, Board* b, SEARCH_INFO_S* search_info_s, bool nullOk);

int quiesence(int alpha, int beta, Board* b, SEARCH_INFO_S* search_info_s);

void search(Board* b, SEARCH_INFO_S* s);

bool isRepetition(Board* b);

void moveSwapper(Board* b, MOVE_S* move_s, int curIdx);

void swapMove(MOVE_S* move_s, int id1, int id2);


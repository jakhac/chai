#pragma once

#include "board.h"
#include "moveGenerator.h"

const int ttSize = 0x100000 * 256;

void initTT(TT_S* pvTable_s);

void clearTT(TT_S* pvTable_s);

void storeTT(Board* b, int move, int score, int flag, int depth);

int probePV(Board* b);

int probeTT(Board* b, int* move, int* score, int alpha, int beta, int depth);

int getPVLine(Board* b, const int maxDepth);

// pawns
const int pawnTableSize = 0x100000 * 8;

void initPawnTable(PAWN_TABLE_S* pawnTable);

void clearPawnTable(PAWN_TABLE_S* pawnTable);

void storePawnEntry(Board* b, const int eval);

int probePawnEntry(Board* b);
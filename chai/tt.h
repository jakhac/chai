#pragma once

#include "board.h"
#include "moveGenerator.h"

const int pvSize = 0x100000 * 2;

void initPVTable(PV_TABLE_S* pvTable_s);

void clearPVTable(PV_TABLE_S* pvTable_s);

void storePV(Board* b, const int move);

int probePV(Board* b);

int getPVLine(Board* b, const int maxDepth);

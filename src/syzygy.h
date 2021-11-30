#pragma once

#include <iomanip> // setw

#include "egtb/tbprobe.h"
#include "board.h"

void initEGTB(const char*);

void freeEGTB();

int probeTB(board_t* b);
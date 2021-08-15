#pragma once

#include "egtb/tbprobe.h"
#include "board.h"

void initEGTB();

void freeEGTB();

int probeTB(board_t* b);
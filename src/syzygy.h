#pragma once

#include <iomanip> // setw

#include "egtb/tbprobe.h"
#include "board.h"

namespace EGTB {

void init(const char*);

void freeEGTB();

int probeTB(board_t* b);

} // namespace EGTB
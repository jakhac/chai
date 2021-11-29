#include "board.h"

int totalMaterial(board_t* b);

int remainingHalfMoves(board_t* b);

int allocateTime(board_t* b, int timeLeft, int inc);

bool isTimeLeft(instr_t* instr);
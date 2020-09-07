#pragma once

#include "defs.h"

int sqOnBoard(const int sq, int* fileBoard[]);
int sideValid(const int side);
int fileRankValid(const int fr);
int pieceValidEmpty(const int pce);
int pieceValid(const int pce);


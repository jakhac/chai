#pragma once

#include "board.h"

U64 getPinnendPieces(Board b, int kingSquare);


U64 xrayRookAttacks(U64 occ, U64 blockers, int rookSq);

void pins(Board b, U64 occ, U64 own, int kingSquare);

U64 pinnedPieces(const Board* pos, int c, int kingSquare);

U64 inBetween(int sq1, int sq2);

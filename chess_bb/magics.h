#pragma once

#include "defs.h"
#include "util.h"

const int RBits[64] = {
  12, 11, 11, 11, 11, 11, 11, 12,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  12, 11, 11, 11, 11, 11, 11, 12
};

extern U64 rook_magic_gen[64];

U64 index_to_U64(int index, int bits, U64 m);
U64 rmask(int sq);
U64 ratt(int sq, U64 block);
int transform(U64 b, U64 magic, int bits);
U64 find_magic(int sq, int m);
void magic();


#pragma once

#include "defs.h"
#include "bitUtils.h"

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

extern bitboard_t rook_magic_gen[64];

bitboard_t index_to_U64(int index, int bits, bitboard_t m);
bitboard_t rmask(int sq);
bitboard_t ratt(int sq, bitboard_t block);
int transform(bitboard_t b, bitboard_t magic, int bits);
bitboard_t find_magic(int sq, int m);
void magic();


#include "rays.h"

extern U64 rays[8][64];

/// <summary>
/// Generate rays for rays array for each direction on each square.
/// </summary>
void initRays() {
	for (int sq = 0; sq < 64; sq++) {
		rays[NORTH][sq] = 0x0101010101010100ULL << sq;
        rays[SOUTH][sq] = 0x0080808080808080ULL >> (63 - sq);
        rays[EAST][sq] = 2 * ((1ULL << (sq | 7)) - (1ULL << sq));
        rays[WEST][sq] = (1ULL << sq) - (1ULL << (sq & 56));
        rays[NORTH_WEST][sq] = westN(0x102040810204000ULL, 7 - col(sq)) << (row(sq) * 8);
        rays[NORTH_EAST][sq] = eastN(0x8040201008040200ULL, col(sq)) << (row(sq) * 8);
        rays[SOUTH_WEST][sq] = westN(0x40201008040201ULL, 7 - col(sq)) >> ((7 - row(sq)) * 8);
        rays[SOUTH_EAST][sq] = eastN(0x2040810204080ULL, col(sq)) >> ((7 - row(sq)) * 8);
	}
}

/// <summary>
/// Get rays in given direction of given square.
/// </summary>
U64 getRay(int dir, int sq) {
    return rays[dir][sq];
}
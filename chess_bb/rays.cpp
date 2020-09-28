#include "rays.h"

U64 Rays::rays[8][64];

/// <summary>
/// Generate rays for Rays::rays array for each direction on each square.
/// </summary>
void Rays::initRays() {
	for (int sq = 0; sq < 64; sq++) {
		Rays::rays[NORTH][sq] = 0x0101010101010100ULL << sq;
        Rays::rays[SOUTH][sq] = 0x0080808080808080ULL >> (63 - sq);
        Rays::rays[EAST][sq] = 2 * ((1ULL << (sq | 7)) - (1ULL << sq));
        Rays::rays[WEST][sq] = (1ULL << sq) - (1ULL << (sq & 56));
        Rays::rays[NORTH_WEST][sq] = _westN(0x102040810204000ULL, 7 - _col(sq)) << (_row(sq) * 8);
        Rays::rays[NORTH_EAST][sq] = _eastN(0x8040201008040200ULL, _col(sq)) << (_row(sq) * 8);
        Rays::rays[SOUTH_WEST][sq] = _westN(0x40201008040201ULL, 7 - _col(sq)) >> ((7 - _row(sq)) * 8);
        Rays::rays[SOUTH_EAST][sq] = _eastN(0x2040810204080ULL, _col(sq)) >> ((7 - _row(sq)) * 8);
	}
}

/// <summary>
/// Get rays in given direction of given square.
/// </summary>
U64 Rays::getRay(int dir, int sq) {
    return Rays::rays[dir][sq];
}
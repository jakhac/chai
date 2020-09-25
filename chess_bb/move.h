#pragma once

#include <iostream>
#include "defs.h"

/*
Move format is bits in hex
0000 0000 0000 0000 0000 0000 0001 -> 0x1
0000 0000 0000 0000 0000 0000 1111 -> 0xF

0000 0000 0000 0000 0000 0111 1111 -> From square bits
0000 0000 0000 0011 1111 1000 0000 -> To square bits >> 7
0000 0000 0011 1100 0000 0000 0000 -> captured piece >> 14
0000 0000 0100 0000 0000 0000 0000 -> ep bool
0000 0000 1000 0000 0000 0000 0000 -> pawn start
0000 1111 0000 0000 0000 0000 0000 -> Promoted piece >> 20
0001 0000 0000 0000 0000 0000 0000 -> castling bool 0x1000000
0010 0000 0000 0000 0000 0000 0000 -> prom flag 0x2000000
*/

//0000 0111 1100 0000 0000 0000
//0000 0011 1100 0000 0000 0000
class Move
{

public:

	int move;
	int score;

	Move() = default;
	Move(int _move, int _score) {
		move = _move;
		score = _score;
	}

	int createBitMove(int from, int to, int cap, int prom, int flag);

};


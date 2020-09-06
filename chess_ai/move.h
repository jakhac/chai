#pragma once
#include "stdlib.h"


class Move
{
public:

	/*
	Move format is bits in hex
	0000 0000 0000 0000 0000 0000 0001 -> hex 1
	0000 0000 0000 0000 0000 0000 1111 -> hex F

	0000 0000 0000 0000 0000 0111 1111 -> From square bits
	0000 0000 0000 0011 1111 1000 0000 -> To square bits <<7
	0000 0000 0011 1100 0000 0000 0000 -> captured piece <<14
	0000 0000 0100 0000 0000 0000 0000 -> ep bool
	0000 0000 1000 0000 0000 0000 0000 -> pawn start
	0000 1111 0000 0000 0000 0000 0000 -> promotions (which piece) <<20
	0001 0000 0000 0000 0000 0000 0000 -> castling bool 0x1000000
	*/
	
	Move(int _move);

	int move;
	int score;
};
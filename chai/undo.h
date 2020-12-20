#pragma once

#include "defs.h"

class Undo {

public:
	int move;
	int castle;
	int enPas;
	int fiftyMove;
	//U64 attackedSquares[2];
	bitboard_t zobKey;

};
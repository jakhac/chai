#pragma once

#include "Board.h"
#include "MoveGenerator.h"

class Perft {

public:

	int counter = 0;

	void perft(Board b, int depth);
	void perftRoot(Board b, int depth);


};


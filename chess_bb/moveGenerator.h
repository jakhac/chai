#pragma once

#include "board.h"
#include "move.h"

class MoveGenerator {

public:

	MoveGenerator() = default;

	// TODO use stack on move generation?
	Move moveList[MAX_GAME_MOVES];
	int moves = 0;

	void northOne(U64* bb);

	void generatePawnMoves(Board b);

	void whiteSinglePawnPush(Board b);
	void blackSinglePawnPush(Board b);

	void whiteDoublePawnPush(Board b);
	void blackDoublePawnPush(Board b);

	void printGeneratedMoves(Board b);

};


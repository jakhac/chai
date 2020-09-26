#pragma once

#include "board.h"
#include "move.h"

class MoveGenerator {

public:

	MoveGenerator() = default;

	// TODO use stack on move generation?
	Move moveList[MAX_GAME_MOVES];
	int moves = 0;

	void generatePawnMoves(Board b);

	void whiteSinglePawnPush(Board b);
	void blackSinglePawnPush(Board b);

	void whiteDoublePawnPush(Board b);
	void blackDoublePawnPush(Board b);

	void whitePawnAttacks(Board b, int shift, U64 forbiddenFile);
	void blackPawnAttacks(Board b, int shift, U64 forbiddenFile);

	void printGeneratedMoves(Board b);

};


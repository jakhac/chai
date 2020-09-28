#pragma once

#include "board.h"
#include "move.h"
#include <vector>

class MoveGenerator {

public:

	vector <Move> quietMoveList;
	vector <Move> capMoveList;

	void generatePawnMoves(Board b);

	// pawn
	void whiteSinglePawnPush(Board b);
	void blackSinglePawnPush(Board b);
	void whiteDoublePawnPush(Board b);
	void blackDoublePawnPush(Board b);
	void whitePawnCaptures(Board b);
	void blackPawnCaptures(Board b);

	// knight
	void whiteKnightMoves(Board b);
	void blackKnightMoves(Board b);
	void whiteKnightCaptures(Board b);
	void blackKnightCaptures(Board b);

	// king
	void whiteKingMoves(Board b); // TODO CASTLE PERM FUN
	void blackKingMoves(Board b); // TODO CASTLE PERM FUN

	void whiteKingCaptures(Board b); // TODO SQ ATTACKED
	void blackKingCaptures(Board b); // TODO SQ ATTACKED

	void printGeneratedMoves(Board b);

	int castleValid();
	int squareAttacked();

};


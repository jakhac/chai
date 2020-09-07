#pragma once

#include "move.h"
#include "Board.h"
#include "IO.h"
#include "defs.h"


class MoveGenerator
{
public:

	Move moveList[MAX_POSITION_MOVES];
	int moveCounter = 0;

	//IO io;

	void addQuietMove(const Board* b, int move);
	void addCaptureMove(const Board* b, int move);
	void addEnPasMove(const Board* b, int move);

	void printMoveList(Board* b);

	void generateAllMoves(Board* b);

	void addWhitePawnCapMove(Board* b, const int from, const int to, const int cap);
	void addWhitePawnMove(Board* b, const int from, const int to);


};


#pragma once

#include "move.h"
#include "Board.h"
#include <vector>
#include <list>


class MoveGenerator
{
public:

	Move moveList[MAX_POSITION_MOVES];
	//vector<Move> moveList = vector<Move>(MAX_POSITION_MOVES);

	//std::list<Move> moveList;

	int moveCounter = 0;

	void addQuietMove(const Board* b, int move);
	void addCaptureMove(const Board* b, int move);
	void addEnPasMove(const Board* b, int move);

	void generateAllMoves(const Board* b);
	void printMoveList(const Move* mList);

};


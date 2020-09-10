#pragma once

#include "move.h"
#include "Board.h"
#include "Definition.h"


class MoveGenerator
{
public:

	Move moveList[MAX_POSITION_MOVES];
	int moveCounter = 0;

	const int slideIndex[2] = { 0, 4}; // avoid if else structure with starting index for each side
	const int loopSlidingPiece[8] = { B, R, Q, 0, b, r, q, 0 }; // sliding pieces

	const int nonSlideIndex[2] = { 0, 3 }; // avoid if else structure with starting index for each side
	const int loopNonSlidingPiece[6] = { N, K, 0, n, k, 0 }; // non sliding pieces

	// directions indexed by piece
	const int pieceDir[13][8] = {
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ -8, -19,	-21, -12, 8, 19, 21, 12 },
		{ -9, -11, 11, 9, 0, 0, 0, 0 },
		{ -1, -10,	1, 10, 0, 0, 0, 0 },
		{ -1, -10,	1, 10, -9, -11, 11, 9 },
		{ -1, -10,	1, 10, -9, -11, 11, 9 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ -8, -19,	-21, -12, 8, 19, 21, 12 },
		{ -9, -11, 11, 9, 0, 0, 0, 0 },
		{ -1, -10,	1, 10, 0, 0, 0, 0 },
		{ -1, -10,	1, 10, -9, -11, 11, 9 },
		{ -1, -10,	1, 10, -9, -11, 11, 9 }
	};

	// how many moves are possible by piece
	const int numDir[13] = {
		0, 0, 8, 4, 4, 8, 8, 0, 8, 4, 4, 8, 8
	};

	// add static keyword?

	// three kind of add moves filter non captures / only captures in search
	void addQuietMove(int move);
	void addCaptureMove(int move);
	void addEnPasMove(int move);

	void printMoveList(Board* b);

	void generateAllMoves(Board* b);

	void addWhitePawnCapMove(Board* b, const int from, const int to, const int cap);
	void addWhitePawnMove(Board* b,  const int from, const int to);

	void addBlackPawnCapMove(Board* b,  const int from, const int to, const int cap);
	void addBlackPawnMove(Board* b,  const int from, const int to);


};


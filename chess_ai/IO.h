#pragma once

#include "BoardData.h"

class IO
{

	BoardData bd = BoardData();
	
	int* fileBoard = bd.fileBoard;
	int* rankBoard = bd.rankBoard;

	char* printSquare(const int square);
	char* printMove(const int move);

};


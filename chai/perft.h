#pragma once

#include "moveGenerator.h"
#include "legalMoveGenerator.h"

class Perft {

public:

	long leafNodes = 0;

	long perftRoot(Board b, int depth);
	void perft(Board b, int depth);

	long perftBulk(Board b, int depth);
	long perftBulkRoot(Board b, int depth);


	long perftRootLegal(Board b, int depth);
	void perftLegal(Board b, int depth);

	long perftBulkRootLegal(Board b, int depth);
	long perftBulkLegal(Board b, int depth);


};

#pragma once

#include "moveGenerator.h"
#include "legalMoveGenerator.h"
#include <chrono>


class Perft {

public:

	long long leafNodes = 0;

	long long perftRoot(Board* b, int depth);
	void perft(Board* b, int depth);

	long long perftLegalRoot(Board* b, int depth);
	void perftLegal(Board* b, int depth);

	long long perftBulkRoot(Board* b, int depth);
	long long perftBulk(Board* b, int depth);

};

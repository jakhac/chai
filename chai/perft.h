#pragma once

#include "moveGenerator.h"
#include "legalMoveGenerator.h"
#include "moveOrdering.h"
#include <chrono>


class Perft {

public:

	/**
	 * Counter for all nodes.
	 */
	long long leafNodes = 0;

	/**
	 * Standard perft root call.
	 *
	 * @param  b	 The Board reference.
	 * @param  depth Perft depth.
	 *
	 * @returns The number of leaf nodes counted at given depth.
	 */
	long long perftRoot(Board* b, int depth);

	/**
	 * Recursive call for perft using {@link #isLegal(Board, move_t)} isLegal function.
	 *
	 * @param  b	 The board reference.
	 * @param  depth Depth remaining.
	 */
	void perft(Board* b, int depth);

	long long perftLegalRoot(Board* b, int depth);

	/**
	 * Recursive call for perft function using {@link #isLegal(Board, move_t)} isLegal function.
	 *
	 * @param  b	 The board reference.
	 * @param  depth Depth remaining.
	 */
	void perftLegal(Board* b, int depth);

	//long long perftBulkRoot(Board* b, int depth);
	//long long perftBulk(Board* b, int depth);

};

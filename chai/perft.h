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
	 * @param  b	 The board_t reference.
	 * @param  depth Perft depth.
	 *
	 * @returns The number of leaf nodes counted at given depth.
	 */
	long long perftRoot(board_t* b, int depth);

	/**
	 * Recursive call for perft using {@link #isLegal(board_t, move_t)} isLegal function.
	 *
	 * @param  b	 The board reference.
	 * @param  depth Depth remaining.
	 */
	void perft(board_t* b, int depth);

	long long perftLegalRoot(board_t* b, int depth);

	/**
	 * Recursive call for perft function using {@link #isLegal(board_t, move_t)} isLegal function.
	 *
	 * @param  b	 The board reference.
	 * @param  depth Depth remaining.
	 */
	void perftLegal(board_t* b, int depth);

	//long long perftBulkRoot(board_t* b, int depth);
	//long long perftBulk(board_t* b, int depth);

};

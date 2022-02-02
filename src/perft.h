#pragma once

#include "moveGenerator.h"

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

};


/**
 * Function to divide with cmd-line move after each perft.
 *
 * @param  b	 board_t reference.
 * @param  fen   The fen.
 * @param  depth The initial depth.
 */
void dividePerft(Perft* p, board_t* b, int depth);
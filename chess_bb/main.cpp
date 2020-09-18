#include "main.h"

/// <summary>
/// Main function to run program.
/// </summary>
int main()
{
	Board b;
	b.init();

	b.parseFen(PAWN_FEN_W);
	b.generateZobristKey();

	b.printBoard();

	b.checkBoard();

	int move = MOVE(8, 24, EMPTY, EMPTY, EMPTY);

	b.push(move);
	
	return 0;
}

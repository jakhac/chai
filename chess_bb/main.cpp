#include "main.h"

/// <summary>
/// Main function to run program.
/// </summary>
int main()
{

	Board b;
	b.init();

	b.setPiece(PAWN, 8, WHITE);
	b.setPiece(PAWN, 9, WHITE);
	b.setPiece(PAWN, 10, WHITE);
	b.setPiece(PAWN, 11, WHITE);
	b.setPiece(PAWN, 63, BLACK);
	b.setPiece(PAWN, 62, BLACK);
	b.setPiece(PAWN, 61, BLACK);
	b.setPiece(PAWN, 60, BLACK);
	b.setPiece(KNIGHT, 50, WHITE);

	b.printBitBoard(&b.occupied);

	return 0;
}

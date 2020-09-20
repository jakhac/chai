#include "main.h"

/// <summary>
/// Main function to run program.
/// </summary>
int main()
{
	Board b;
	b.init();

	b.parseFen(STARTING_FEN);
	b.generateZobristKey();

	b.printBoard();

	b.checkBoard();
	
	//b.push(move);

	string m = "e2e4";
	int parsedMove = b.parseMove(m);

	cout << "Pawn start " << (parsedMove & MFLAGPS) << endl;
	cout << "EP capture " << (parsedMove & MFLAGEP) << endl;
	cout << "Castle move " << (parsedMove & MFLAGCA) << endl;
	cout << "Promoted piece " << PROMOTED(parsedMove) << endl;
	cout << "Capture " << (parsedMove & MFLAGCAP) << endl;

	b.push(parsedMove);
	b.printBoard();

	return 0;
}

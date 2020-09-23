#include "main.h"
#include "move.h"

/// <summary>
/// Main function to run chess in console.
/// </summary>
int main() {

	Board b;
	string m;

	b.init();

	b.parseFen(CASTLE);
	b.printBoard();
	b.checkBoard();

	//int move = b.parseMove("d5c6");
	//b.printMoveStatus(move);
	//b.push(move);

	while (true) {
		getline(cin, m);

		int parsedMove = b.parseMove(m);
		b.printMoveStatus(parsedMove);
		b.push(parsedMove);
	}

	return 0;
}

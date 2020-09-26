#include "main.h"

/// <summary>
/// Main function to run chess in console.
/// </summary>
int main() {

	Board b;
	string m;

	b.init();

	b.parseFen(PAWN_TEST_B);
	b.printBoard();
	b.checkBoard();

	MoveGenerator moveGenerator;
	moveGenerator.generatePawnMoves(b);
	moveGenerator.printGeneratedMoves(b);

	return 0;

	while (true) {
		getline(cin, m);

		if (m == "pop") {
			b.pop();
			continue;
		}

		int parsedMove = b.parseMove(m);
		b.printMoveStatus(parsedMove);
		b.push(parsedMove);
	}

	return 0;
}

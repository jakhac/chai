#include "main.h"

/// <summary>
/// Main function to run chess in console.
/// </summary>
int main() {

	Board b;
	string m;

	b.init();

	b.parseFen(EP_FEN);
	b.printBoard();
	b.checkBoard();

	//int move = b.parseMove("d5c6");
	//b.printMoveStatus(move);
	//b.push(move);

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

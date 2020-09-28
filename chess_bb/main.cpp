#include "main.h"


void init(Board* b) {
	initClearSetMask();
	b->initHashKeys();
	initSquareToRankFile();
	Rays::initRays();
	initAttackerMasks();
}

/// <summary>
/// Main function to run chess in console.
/// </summary>
int main() {

	string m;
	Board b;
	init(&b);

	b.parseFen(STARTING_FEN);
	b.printBoard();
	b.checkBoard();

	///*for (int i = 0; i < 64; i++) {
	//	cout << "Square " << i << endl;
	//	b.printBitBoard(&b.kingAtkMask[i]);
	//}*/

	//MoveGenerator moveGenerator;
	//moveGenerator.blackKingCaptures(b);
	//moveGenerator.blackKingCaptures(b);
	//moveGenerator.printGeneratedMoves(b);

	return 0;

	while (true) {
		getline(cin, m);

		if (m == "pop") {
			b.pop();
			continue;
		}

		int parsedMove = b.parseMove(m);
		printMoveStatus(parsedMove);
		b.push(parsedMove);
	}

	return 0;
}

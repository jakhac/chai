#include "main.h"

int main() {

	string m;
	Board b;

	init(&b);
	b.parseFen(MOVES_48);
	b.printBoard();
	b.checkBoard();

	MoveGenerator moveGenerator;
	moveGenerator.generateMoves(b);
	moveGenerator.printGeneratedMoves(b);

	return 0;

	while (true) {
		cout << "\n##################\n\n";
		moveGenerator.resetMoveLists();
		moveGenerator.generateMoves(b);

		//moveGenerator.addKingMoves

		//moveGenerator.printGeneratedMoves(b);
		vector<Move> allMoves = moveGenerator.getAllMoves();

		getline(cin, m);

		if (m == "pop") {
			b.pop();
			continue;
		}

		int parsedMove = b.parseMove(m);

		// try to find move
		bool flag_found = false;
		for (auto m : allMoves) {
			if (parsedMove == m.move) {
				flag_found = true;
				b.push(parsedMove);
				break;
			}
		}

		if (!flag_found) cout << "Tried to push non valid move on board. Try again." << endl;
	}

	return 0;
}

void init(Board* b) {

	auto start = std::chrono::high_resolution_clock::now();
	cout << "Init keys and masks ... ";
	initClearSetMask();
	initSquareToRankFile();
	b->initHashKeys();
	//initRays();
	initAttackerMasks();
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	cout << duration.count() << "ms\n";

	start = std::chrono::high_resolution_clock::now();
	cout << "Init magic tables for bishop and rooks ... ";
	initRookMasks();
	initRookMagicTable();
	initBishopMasks();
	initBishopMagicTable();
	stop = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	cout << duration.count() << "ms\n" << endl;
}

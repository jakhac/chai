#include "main.h"

/*
* - Elaborate tests
* - Undo struct 
* - Refactor code files: Types, Constants, del magics & rays?
* - replace int return by bool
* - Update documentation
*/

int main() {

	Board b;
	Perft p;

	init(&b);
	b.parseFen(STARTING_FEN);
	b.printBoard();
	b.checkBoard();

	p.perftBulkRoot(&b, 7);

	return 0;
}

void play(Board* b) {
	string m;

	while (true) {
		cout << "\n##################\n\n";
		getline(cin, m);

		if (m == "pop") {
			Undo pop = b->pop();
			cout << "Popped " << getStringMove(pop.move) << " from stack." << endl;
			b->printBoard();
			continue;
		}

		MOVE_S _move_s[1];
		generateMoves(b, _move_s);

		int parsedMove = b->parseMove(m);
		bool flag_found = false;
		for (int i = 0; i < _move_s->moveCounter; i++) {
			if (parsedMove == _move_s->moveList[i]) {
				flag_found = true;

				if (!b->push(parsedMove)) {
					cout << "Move " << getStringMove(parsedMove) << " remains check. Try again." << endl;
					continue;
				}

				cout << "Pushed " << getStringMove(parsedMove) << " on board." << endl;
				b->printBoard();
				break;
			}
		}

		if (!flag_found) cout << "Tried to push non valid move on board. Try again." << endl;
	}
}

void dividePerft(Board* b, string fen, int depth) {

	string move = "";
	Perft p;
	b->parseFen(fen);

	while (depth) {

		if (move != "") {
			int parsedMove = b->parseMove(move);
			b->push(parsedMove);
			depth--;
		}

		p.perftRoot(b, depth);

		cout << "\nDivide at move ";
		getline(cin, move);
	}

	cout << "Reached depth 0" << endl;
	return;

}

void init(Board* b) {

	auto start = std::chrono::high_resolution_clock::now();
	cout << "Init keys and masks ... ";
	initClearSetMask();
	initSquareToRankFile();
	b->initHashKeys();
	initAttackerMasks();

	// TODO performance
	initObstructed();
	initLine();

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

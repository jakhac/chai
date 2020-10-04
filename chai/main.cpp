#include "main.h"

int main() {

	Board b;
	Perft p;
	MoveGenerator mg;

	init(&b);

	b.parseFen(STARTING_FEN);
	//b.parseFen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");

	b.printBoard();
	b.checkBoard();

	mg.generateMoves(b);
	//mg.printGeneratedMoves(b);

	//play(b, mg);
	//p.perftRoot(b, 4);

	auto start = std::chrono::high_resolution_clock::now();

	p.perftRoot(b, 4);

	// with debug 7524ms
	// wout debug 

	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	cout << duration.count() << "ms\n";

	return 0;
}

void play(Board b, MoveGenerator mg) {
	string m;

	while (true) {
		cout << "\n##################\n\n";
		mg.resetMoveLists();
		mg.generateMoves(b);

		//mg.printGeneratedMoves(b);
		vector<Move> allMoves = mg.getAllMoves();

		getline(cin, m);

		if (m == "pop") {
			Undo pop = b.pop();
			cout << "Popped " << getStringMove(pop.move) << " from stack." << endl;
			b.printBoard();
			continue;
		}

		int parsedMove = b.parseMove(m);
		//printMoveStatus(parsedMove);

		// try to find move
		bool flag_found = false;
		for (auto m : allMoves) {
			if (parsedMove == m.move) {
				flag_found = true;

				if (!b.push(parsedMove)) {
					cout << "Move " << getStringMove(parsedMove) << " remains check. Try again." << endl;
					continue;
				}

				cout << "Pushed " << getStringMove(parsedMove) << " on board." << endl;
				b.printBoard();
				break;
			}
		}

		if (!flag_found) cout << "Tried to push non valid move on board. Try again." << endl;
	}
}

void init(Board* b) {

	auto start = std::chrono::high_resolution_clock::now();
	cout << "Init keys and masks ... ";
	initClearSetMask();
	initSquareToRankFile();
	b->initHashKeys();
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

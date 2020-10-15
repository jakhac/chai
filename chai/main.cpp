#include "main.h"

/*
* - Evaluation improvements: 
*		King Safety
*		Pawn Shield
*		Pawn Structure
*		Bishop Pair
*		Pins
*		Center Control
*		Mobility
*		
* - Game States
* - Opening Book
* - Null move pruning
* - Transposition table
*/

int main() {

	Board b;
	Perft p;
	MOVE_S move_s[1];
	SEARCH_INFO_S s[1];

	init(&b);
	b.parseFen(STARTING_FEN);
	//b.parseFen(WAC1);
	//b.parseFen("rnbqkbnr/pp2pppp/2p5/3p4/3PQ3/8/PPP1PPPP/RNB1KBNR w KQkq - 0 1");
	b.printBoard();

	//generateMoves(&b, move_s);
	//dividePerft(&b, BUG_FEN, 5); // 8312, 8031
	
	play(&b, &p, s);

	if (b.pvTable_s->pvTable != NULL) free(b.pvTable_s->pvTable);
	return 0;
}

void play(Board* b, Perft* p, SEARCH_INFO_S* s) {
	string m;

	while (true) {
		cout << "\n##################\n\n";
		getline(cin, m);

		// POP MOVE FROM BOARD
		if (m == "pop") {
			UNDO_S pop = b->pop();
			cout << "Popped " << getStringMove(pop.move) << " from stack." << endl;
			b->printBoard();
			continue;
		}

		// SEARCH POSITION
		if (m == "s") {
			s->depth = 12;
			s->startTime = getTimeMs();
			s->stopTime = getTimeMs() + 20000;
			s->timeSet = true;
			search(b, s);
			continue;
		}

		if (m == "uci") {
			uciMode(b, s);
		}

		// PRINT PV LINE FROM CURRENT BOARD
		if (m == "pv") {
			int d = getPVLine(b, MAX_DEPTH);
			cout << "PV line length " << d << endl;
			for (int i = 0; i < d; i++) {
				cout << "Best move " << i << " is: ";
				printMove(b->pvArray[i]);
				cout << endl;
			}
			continue;
		}

		if (m == "perft") {
			cout << "Perft this position to depth ";
			getline(cin, m);
			if (stoi(m) >= 1 && stoi(m) <= 15) {
				dividePerft(b, "-1", stoi(m));
			}
			continue;
		}

		if (m == "0000") {
			b->pushNull();

			cout << "Pushed " << getStringMove(b->parseMove(m)) << " on board." << endl;
			b->printBoard();

			continue;
		}

		// PUSH MOVE IN ALGEBRAIC NOTATION
		MOVE_S _move_s[1];
		generateMoves(b, _move_s);
		int parsedMove = b->parseMove(m);
		bool flag_found = false;
		for (int i = 0; i < _move_s->moveCounter; i++) {
			if (parsedMove == _move_s->moveList[i]) {
				flag_found = true;
				storePV(b, parsedMove);

				if (!b->push(parsedMove)) {
					cout << "Move " << getStringMove(parsedMove) << " remains check or is no valid move. Try again." << endl;
					continue;
				}

				cout << "Pushed " << getStringMove(parsedMove) << " on board." << endl;
				b->printBoard();
				break;
			}
		}

		if (!flag_found) cout << "Move or command not valid. Try again." << endl;
	}
}

void dividePerft(Board* b, string fen, int depth) {

	string move = "";
	Perft p;

	// parse fen if valid
	if (fen != "-1") {
		b->parseFen(fen);
	}

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

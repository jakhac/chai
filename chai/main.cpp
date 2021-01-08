#include "main.h"

int main() {
	init(&board);
	board.parseFen(END_FEN_1);
	//board.parseFen("1B6/P1P4k/8/1q6/8/K7/7p/5q2 b - - 0 1");
	board.printBoard();
	log("\nStartup");

	// development

	//moveList_t moveList[1];
	//generateMoves(&board, moveList, board.isCheck(board.side));
	//generateQuietCheckers(&board, moveList);
	//printGeneratedMoves(moveList);
	//return 0;

	play(&board, &perft, s);

	if (board.tt->table != NULL) free(board.tt->table);
	if (board.pawnTable->table != NULL) free(board.pawnTable->table);
	return 0;
}

void play(Board* b, Perft* p, search_t* s) {
	string m;

	while (true) {
		cout << "\n##################\n\n";
		getline(cin, m);

		log(m);

		// POP MOVE FROM BOARD
		if (m == "pop") {
			undo_t pop = b->pop();
			cout << "Popped " << getStringMove(pop.move) << " from stack." << endl;
			b->printBoard();
			continue;
		}

		if (m == "autoplay") {
			while (true) {
				s->depth = 7;
				s->startTime = getTimeMs();
				s->stopTime = getTimeMs() + 10000;

				s->timeSet = true;
				s->depthSet = false;
				search(b, s);

				getPVLine(b, s->depth);

				moveList_t move_s[1];
				generateMoves(b, move_s, b->isCheck(b->side));
				b->push(move_s->moves[move_s->cnt / 3]);
				cout << "Push move " << b->halfMoves << ". " << getStringMove(move_s->moves[move_s->cnt / 2]) << endl;
				b->printBoard();
				cout << "###############" << endl;
			}
		}

		// SEARCH POSITION
		if (m == "s") {
			s->depth = 12;
			s->startTime = getTimeMs();
			s->stopTime = getTimeMs() + 5000;

			s->timeSet = false;
			s->depthSet = true;
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
			cout << "Skip or enter FEN: ";
			getline(cin, m);
			if (m.size() > 0) {
				b->parseFen(m);
				b->printBoard();
			}

			cout << "Perft this position to depth ";
			getline(cin, m);
			if (stoi(m) >= 1 && stoi(m) <= 15) {
				dividePerft(b, "-1", stoi(m));
			}
			continue;
		}

		if (m == "reps") {
			cout << "Board is three fold repetition " << isThreeFoldRepetition(b) << endl;
			cout << "Board is repetition " << isRepetition(b) << endl;
		}

		if (m == "0000") {
			b->pushNull();

			cout << "Pushed " << getStringMove(b->parseMove(m)) << " on board." << endl;
			b->printBoard();

			continue;
		}

		if (m == "p") {
			b->printBoard();
			continue;
		}

		// PUSH MOVE IN ALGEBRAIC NOTATION
		moveList_t _move_s[1];
		generateMoves(b, _move_s, b->isCheck(b->side));

		int parsedMove = b->parseMove(m);
		bool flag_found = false;
		for (int i = 0; i < _move_s->cnt; i++) {
			if (parsedMove == _move_s->moves[i]) {
				flag_found = true;
				//storeTT(b, parsedMove);

				if (!b->push(parsedMove)) {
					cout << "Move " << getStringMove(parsedMove) << " remains check or is no valid move. Try again." << endl;
					continue;
				}

				if (isThreeFoldRepetition(b)) {
					cout << "Position was repeated the third time. Both players can claim draw." << endl;
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

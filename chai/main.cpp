#include "main.h"


int main() {
	board_t* b = &board;
	//uciMode(&board, s);

	init(&board);
	parseFen(&board, STARTING_FEN);
	//parseFen(&board, BUG_FEN);
	//parseFen(&board, "8/2p5/3p4/1P6/K3Pp1r/6k1/6P1/1R6 b - e3 5 7");
	printBoard(&board);

	//moveList_t moveList[1];
	//generateMoves(&board, moveList, isCheck(&board, board.side));
	//printGeneratedMoves(b, moveList);
	//return 0;

	//cout << "Is checking move " << checkingMove(b, serializeMove(F4, E3, 0)) << endl;

	//perft.perftRoot(b, 4);

	play(&board, &perft, s);

	destroyTranspositionTables(&board);
	return 0;
}

void play(board_t* b, Perft* p, search_t* s) {
	string m;

	while (true) {
		cout << "\n##################\n\n";
		getline(cin, m);

		// POP MOVE FROM BOARD
		if (m == "pop") {
			undo_t undoPop = pop(b);
			cout << "Popped " << getStringMove(b, undoPop.move) << " from stack." << endl;
			printBoard(b);
			continue;
		}

		if (m == "autoplay") {
			while (true) {
				s->depth = 40;
				s->depthSet = false;

				s->startTime = getTimeMs();
				s->stopTime = getTimeMs() + 15000;

				s->timeSet = true;
				search(b, s);

				getPVLine(b, 1);
				move_t bestMove = b->pvArray[0];

				//moveList_t move_s[1];
				//generateMoves(b, move_s, isCheck(b, b->side));
				//push(b, move_s->moves[move_s->cnt / 3]);
				push(b, bestMove);
				cout << "Push move " << b->halfMoves << ". " << getStringMove(b, bestMove) << endl;
				printBoard(b);
				cout << "###############" << endl;
				getline(cin, m);
			}
		}

		// SEARCH POSITION
		if (m == "s") {
			s->depth = 40;
			s->startTime = getTimeMs();
			s->stopTime = getTimeMs() + 10000;

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
				cout << "Best move " << i << " is: " << getStringMove(b, b->pvArray[i]) << endl;
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

		if (m == "movegen") {
			moveList_t moveList[1];
			generateMoves(&board, moveList, isCheck(&board, board.side));
			printGeneratedMoves(b, moveList);
			continue;
		}

		if (m == "fen") {
			cout << "Enter FEN: ";
			cin.ignore();
			getline(cin, m);
			cout << "Parsed FEN \"" << m << "\" into board." << endl;
			parseFen(b, m);
			printBoard(b);
			continue;
		}

		if (m == "reps") {
			cout << "Number of repetitions " << getRepetitions(b) << endl;
			cout << "board_t is repetition " << isRepetition(b) << endl;
		}

		if (m == "0000") {
			pushNull(b);

			cout << "Pushed " << getStringMove(b, parseMove(b, m)) << " on board." << endl;
			printBoard(b);

			continue;
		}

		if (m == "p") {
			printBoard(b);
			continue;
		}

		// PUSH MOVE IN ALGEBRAIC NOTATION
		moveList_t _move_s[1];
		generateMoves(b, _move_s, isCheck(b, b->side));

		int parsedMove = parseMove(b, m);
		bool flag_found = false;
		for (int i = 0; i < _move_s->cnt; i++) {
			if (parsedMove == _move_s->moves[i]) {
				flag_found = true;
				//storeTT(b, parsedMove);

				if (!push(b, parsedMove)) {
					cout << "Move " << getStringMove(b, parsedMove) << " remains check or is no valid move. Try again." << endl;
					continue;
				}

				cout << "Pushed " << getStringMove(b, parsedMove) << " on board." << endl;
				printBoard(b);
				break;
			}
		}

		if (!flag_found) cout << "Move or command not valid. Try again." << endl;
	}
}

void dividePerft(board_t* b, string fen, int depth) {

	string move = "";
	Perft p;

	// parse fen if valid
	if (fen != "-1") {
		parseFen(b, fen);
	}

	while (depth) {

		if (move != "") {
			int parsedMove = parseMove(b, move);
			push(b, parsedMove);
			depth--;
		}

		p.perftRoot(b, depth);

		cout << "\nDivide at move ";
		getline(cin, move);

		if (move == "quit") {
			return;
		}

	}

	cout << "Reached depth 0" << endl;
	return;

}

#include "main.h"


int main() {
	//uciMode(&board, s);

	init(&board);
	parseFen(&board, STARTING_FEN);
	//parseFen(&board, BUG_FEN);
	//parseFen(&board, "1r4r1/q2b1p1k/p4P1p/1pn1P1p1/3N4/5B2/PPP2R1Q/4R1K1 w - - 1 0");

	// mate in 1 quiescence test
	//parseFen(&board, "7k/8/7K/8/8/8/8/R7 w - - 9 11");
	//cout << "in check " << isCheck(&board, board.side) << endl;
	//cout << "qui = " << quiescence(-INF, INF, 0, &board, s, nullptr) << endl;

	printBoard(&board);

	//moveList_t moveList[1];
	//generateMoves(&board, moveList, board.isCheck(board.side));
	//addKingCheckEvasions(&board, moveList);
	//printGeneratedMoves(moveList);
	//return 0;

	/*position startpos moves e2e4 c7c5 b2b3 d7d6 c1b2 g8f6 f1b5 c8d7 b5d7 b8d7 d1e2 g7g6 f2f4 f8g7 b1c3 e8g8 g1h3 e7e5 e1g1 a8c8 d2d3 e5f4 h3f4 g6g5 f4h3 h7h6 a2a4 d8e7 f1f5 e7e6 a1f1 f8e8 a4a5 d6d5 e2e1 d5e4 d3e4 f6e4 e1e4 g7c3 e4e6 f7e6 f5f7 c3b2 f7d7 b2d4 h3f2 e8f8 c2c3 d4c3 d7b7 c3d4 b7d7 f8f6 d7a7 c8f8 g1h1 f6f2 f1f2 f8f2 g2g3 g5g4 h2h3 g4h3 a7e7 e6e5 a5a6 c5c4 b3c4 f2a2 e7e6 g8g7 e6d6 h6h5 d6e6 g7h7 c4c5 d4f2 e6e5 h7g6 e5e8 g6f7 e8b8 f2g3 b8b7 f7e6 b7b1 e6d5 b1c1 a2a6 c1g1 g3f4 g1f1 a6a4 h1g1 f4e5 f1e1 e5d4 g1h2 a4a3 h2h1 d5c5 e1e7 c5d6 e7e1 a3d3 e1e8 h5h4 e8a8 d6d5 a8a5 d5e6 a5b5 d4e5 b5b6 e6e7 b6b1 d3d2 b1e1 e7d6 h1g1 h3h2*/


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
			cout << "Popped " << getStringMove(undoPop.move) << " from stack." << endl;
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
				cout << "Push move " << b->halfMoves << ". " << getStringMove(bestMove) << endl;
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
				cout << "Best move " << i << " is: " << getStringMove(b->pvArray[i]) << endl;
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

			cout << "Pushed " << getStringMove(parseMove(b, m)) << " on board." << endl;
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
					cout << "Move " << getStringMove(parsedMove) << " remains check or is no valid move. Try again." << endl;
					continue;
				}

				cout << "Pushed " << getStringMove(parsedMove) << " on board." << endl;
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

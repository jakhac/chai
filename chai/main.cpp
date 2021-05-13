#include "main.h"


int main() {
	board_t* b = &board;
	//uciMode(&board, s);

	init(&board);
	parseFen(&board, STARTING_FEN);
	//parseFen(&board, BUG_FEN);
	//parseFen(&board, "8/1p1q3k/p2Pr1pp/p1B3p1/1p3QP1/5R2/5K2/8 b - - 0 1");
	printBoard(&board);

	//position startpos moves e2e4 c7c5 g1f3 d7d6 d2d4 c5d4 f3d4 g8f6 b1c3 a7a6 c1e3 e7e6 f1e2 d8c7 a2a4 f8e7 e1g1 e8g8 g2g4 h7h6 f2f4 e6e5 d4f5 c8f5 e4f5 e5f4 f1f4 b8c6 e2c4 d6d5 c3d5 f6d5 c4d5 e7d6 f4e4 d6h2 g1f1 a8d8 e3c5 h2d6 c5e3 d6h2 c2c4 c6b4 a1a3 b4d5 c4d5 c7d6 a3d3 d8e8 d1f3 h2g3 b2b4 e8e4 f3e4 d6d7 a4a5 d7b5 e3c5 f8e8 e4f3 g3f4 f1f2 g7g5 f5g6 f7g6 d5d6 b5d7 f3d5 g8h7 d3f3 e8e5 d5c4 e5e6 c4f4

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

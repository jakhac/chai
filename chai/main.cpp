#include "main.h"


int main() {
	init(&board);
	parseFen(&board, STARTING_FEN);
	//parseFen(&board, BUG_FEN);
	//parseFen(&board, "4K3/8/2k4r/8/2n5/8/8/5R2 w - - 98 117");

	printBoard(&board);

	//moveList_t moveList[1];
	//generateMoves(&board, moveList, board.isCheck(board.side));
	//addKingCheckEvasions(&board, moveList);
	//printGeneratedMoves(moveList);
	//return 0;
	/*position startpos moves g1f3 d7d5 g2g3 g8f6 f1g2 c8f5 d2d3 e7e6 c2c4 b8c6 f3h4 f5g4 h2h3 g4h5 g3g4 h5g6 h4g6 h7g6 g4g5 f8b4 c1d2 f6h5 c4d5 b4d2 d1d2 e6d5 d3d4 c6e7 b1c3 e8g8 e2e4 c7c6 e1g1 d5e4 g2e4 d8d6 f2f4 h5g3 f1f3 g3e4 c3e4 d6d5 f3b3 b7b6 e4c3 d5c4 a1d1 f7f6 d2e1 e7f5 g5f6 g7f6 e1e2 c4e2 c3e2 a8e8 e2g3 f5e3 d1c1 e8e6 a2a4 f6f5 c1e1 f8e8 h3h4 e3d5 e1f1 e6e3 b3e3 e8e3 g1h2 e3b3 h4h5 b3b2 h2h1 g6h5 g3f5 a7a6 f1c1 g8f7 c1c6 d5f4 c6h6 a6a5 h1g1 f7g8 g1f1 b2b1 f1f2 b1b3 f2e1 b6b5 h6a6 b5a4 a6a5 a4a3 a5a8 g8f7 e1d1 f4d5 d1d2 d5b4 f5e3 a3a2 e3c4 b3b1 c4e5 f7e7 e5d3 b4d3 a8a2 d3b4 a2a7 e7d6 d2c3 h5h4 a7h7 b4d5 c3c2 b1h1 h7h6 d6e7 h6h7 e7e6 c2b3 h4h3 h7h5 h1b1 b3c2 b1b4 h5h6 e6f5 h6h3 b4d4 c2b2 d4b4 b2c2 f5e5 h3h7 b4b8 c2d2 e5d4 h7h4 d4c5 h4a4 b8d8 a4a3 c5c4 a3a4 d5b4 d2e2 d8e8 e2f3 c4b3 a4a1 b4c2 a1d1 b3c4 d1c1 c4c3 f3f4 e8e6 f4f5 e6e3 f5g5 e3f3 g5g6 f3f4 g6g5 f4f7 g5h5 f7g7 c1b1 c2d4 h5h6 g7g2 b1c1 c3d3 h6h7 d4b3 c1c8 g2g1 c8g8 g1h1 h7g7 b3c5 g7f7 h1h7 f7f6 d3d4 g8d8 d4c4 f6e5 h7h1 d8d4 c4c3 d4d5 c5b3 e5d6 c3c4 d5e5 h1d1 d6c7 b3d4 e5h5 d1e1 c7d7 e1e2 h5h1 e2e5 h1c1 c4d5 c1a1 d4b3 a1d1 d5c5 d7c7 b3a5 c7c8 a5c4 c8c7 e5e3 d1f1 e3e7 c7d8 e7g7 f1f5 c5c6 f5f1 g7h7 d8e8 h7h6*/

	play(&board, &perft, s);
	//position startpos moves g1f3 d7d5 g2g3 g8f6 f1g2 c8f5 d2d3 e7e6 c2c4 b8c6 f3h4 f5g4 h2h3 g4h5 g3g4 h5g6 h4g6 h7g6 g4g5 f8b4 c1d2 f6h5 c4d5 b4d2 d1d2 e6d5 d3d4 c6e7 b1c3 e8g8 e2e4 c7c6 e1g1 d5e4 g2e4 d8d6 f2f4 h5g3 f1f3 g3e4 c3e4 d6d5 f3b3 b7b6 e4c3 d5c4 a1d1 f7f6 d2e1 e7f5 g5f6 g7f6 e1e2 c4e2 c3e2 a8e8 e2g3 f5e3 d1c1 e8e6 a2a4 f6f5 c1e1 f8e8 h3h4 e3d5 e1f1 e6e3 b3e3 e8e3 g1h2 e3b3 h4h5 b3b2 h2h1 g6h5 g3f5 a7a6 f1c1 g8f7 c1c6 d5f4 c6h6 a6a5 h1g1 f7g8 g1f1 b2b1 f1f2 b1b3 f2e1 b6b5 h6a6 b5a4 a6a5 a4a3 a5a8 g8f7 e1d1 f4d5 d1d2 d5b4 f5e3 a3a2 e3c4 b3b1 c4e5 f7e7 e5d3 b4d3 a8a2 d3b4 a2a7 e7d6 d2c3 h5h4 a7h7 b4d5 c3c2 b1h1 h7h6 d6e7 h6h7 e7e6 c2b3 h4h3 h7h5 h1b1 b3c2 b1b4 h5h6 e6f5 h6h3 b4d4 c2b2 d4b4 b2c2 f5e5 h3h7 b4b8 c2d2 e5d4 h7h4 d4c5 h4a4 b8d8 a4a3 c5c4 a3a4 d5b4 d2e2 d8e8 e2f3 c4b3 a4a1 b4c2 a1d1 b3c4 d1c1 c4c3 f3f4 e8e6 f4f5 e6e3 f5g5 e3f3 g5g6 f3f4 g6g5 f4f7 g5h5 f7g7 c1b1 c2d4 h5h6 g7g2 b1c1 c3d3 h6h7 d4b3 c1c8 g2g1 c8g8 g1h1 h7g7 b3c5 g7f7 h1h7 f7f6 d3d4 g8d8 d4c4 f6e5 h7h1 d8d4 c4c3 d4d5 c5b3 e5d6 c3c4 d5e5 h1d1 d6c7 b3d4 e5h5 d1e1 c7d7 e1e2 h5h1 e2e5 h1c1 c4d5 c1a1 d4b3 a1d1 d5c5 d7c7 b3a5 c7c8 a5c4 c8c7 e5e3 d1f1 e3e7 c7d8 e7g7 f1f5 c5c6 f5f1 g7h7 d8e8 h7h6

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
			cout << "board_t is three fold repetition " << isThreeFoldRepetition(b) << endl;
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

				if (isThreeFoldRepetition(b)) {
					cout << "Position was repeated the third time. Both players can claim draw." << endl;
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

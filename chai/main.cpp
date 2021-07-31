#include "main.h"

int main() {
	cout << "CHAI " << VERSION << endl
		<< "assert=" << info_ASSERT
		<< " buckets=" << BUCKETS << endl
		<< "compiler=" << info_COMPILER
		<< " date=" << __DATE__ << endl;

	// Init all tables and parameters
	init(&board);
	initHashTables(b);

	const char* tbPath = "C:/egtb_files";
	if (!tb_init(tbPath)) {
		cout << "TB init failed." << endl;
		Assert(false);
		exit(1);
	}

	// TODO use with time uci comd
	//position startpos moves e2e4 c7c5 g1f3 b8c6 b1c3 e7e6 d2d4 c5d4 f3d4 d8c7 f2f4 f8c5 d4b3 c5e7 c1e3 b7b6 c3b5 c7b8 e4e5 g8h6 f1d3 e8g8 b5d6 e7d6 e5d6 f7f5 d3b5 c6b4 e1g1 h6g4 e3c1 b4d5 d1d4 b8d6 h2h3 g4f6 c2c4 a7a6 b5a4 b6b5 c4b5 a8b8 b3c5 a6b5 a4b3 d6b6 f1e1 b5b4 a2a4 b4a3 a1a3 d5c7 b3c4 b6c6 a3c3 d7d5 c4f1 c6d6 c3c2 c7b5 d4a4 c8d7 c5d7 d6d7 c2c5 b5d6 a4d7 f6d7 c5c6 d6e4 c6e6 g7g6 e1d1 d7f6 e6c6 b8c8 c6c8 f8c8 c1e3 c8b8 e3d4 g8f7 d4e5 b8b3 f1d3 f7e6 d3c2 b3b6 c2b1 h7h5 e5d4 b6b4 g1h2 h5h4 b1a2 e6e7 d1d3 b4a4 a2b1 e7d6 b1c2 a4c4 c2b3 c4b4 b3a2 b4a4 d4e5 d6c6 a2b3 a4b4 b3d1 f6d7 d3d4 b4d4 e5d4 d7c5 d4g7 c5d3 d1a4 c6b6 g7h6 d3b2 a4e8 d5d4 h6g7 b6c5 e8g6 e4d6 g7f6 b2d3 f6g5 c5c6 h2h1 c6d5 g6h7 d5e4 g5e7 d6c4 e7h4 d3f4 g2g4 d4d3 g4f5 d3d2 f5f6 e4f3 h7c2 c4e3 c2b3 d2d1r b3d1 e3d1 h4e1 f4e6 h1h2 e6d8 h3h4

	// TODO: use stacksize attr in gcc build

	parseFen(&board, STARTING_FEN);
	//parseFen(&board, "R7/6k1/P7/6p1/r5P1/3K4/8/8 w - - 0 1");

	// TODO gitid

	cout << "TB max " << TB_LARGEST << endl;

	printBoard(b);
	cli(b, &perft, s);

	// Free all hash tables before exit
	freeHashTables(b->tt, b->pt);
	tb_free();

	return 0;
}

void cli(board_t* b, Perft* p, search_t* s) {
	std::string userInput;

	while (1) {
		cin >> userInput;

		// start UCI protocol
		if (userInput == "uci") {
			uciMode(b, s);
			continue;
		}

		if (userInput == "quit") {
			return;
		}

		if (userInput == "test") {
			continue;
		}

		// POP MOVE FROM BOARD
		if (userInput == "pop") {
			undo_t undoPop = pop(b);
			cout << "Popped " << getStringMove(b, undoPop.move) << " from stack." << endl;
			printBoard(b);
			continue;
		}

		// start searching this position
		if (userInput == "s") {
			s->depth = 15;
			s->startTime = getTimeMs();
			s->stopTime = getTimeMs() + 10000;

			s->timeSet = false;
			s->depthSet = true;
			search(b, s);
			continue;
		}

		if (userInput == "perft") {
			cout << "Enter Perft depth: ";

			std::string perftDepth;
			cin >> perftDepth;

			if (stoi(perftDepth) >= 1 && stoi(perftDepth) <= 15) {
				dividePerft(b, stoi(perftDepth));
			} else {
				cerr << "Enter an integer between in [1, 15]." << endl;
			}
			cout << "Leaving perft option." << endl;
			continue;
		}

		if (userInput == "movegen") {
			moveList_t moveList[1];
			generateMoves(&board, moveList, isCheck(&board, board.stm));
			printGeneratedMoves(b, moveList);
			continue;
		}

		if (userInput == "fen") {
			cout << "Enter FEN: ";
			cin.ignore();
			getline(cin, userInput);
			cout << "Parsed FEN \"" << userInput << "\" into board." << endl;
			parseFen(b, userInput);
			printBoard(b);
			continue;
		}

		if (userInput == "print") {
			printBoard(b);
			continue;
		}

		if (userInput == "0000") {
			pushNull(b);
		}

		// Assume userInput is a move
		generateMoves(b, move_s, isCheck(b, b->stm));
		int parsedMove = parseMove(b, userInput);
		bool inputIsMove = false;
		for (int i = 0; i < move_s->cnt; i++) {
			if (parsedMove == move_s->moves[i]) {
				inputIsMove = true;
				break;
			}
		}

		if (inputIsMove) {
			push(b, parsedMove);
			printBoard(b);
			continue;
		}

		cout << "Command does not exist. Valid commands are:" << endl
			<< "\tuci\t(start uci protocol)" << endl
			<< "\ts\t(search current position)" << endl
			<< "\t[e2e4]\t(apply move)" << endl
			<< "\tpop\t(undo move)" << endl
			<< "\tfen\t(parse fen)" << endl
			<< "\tprint\t(print board status)" << endl
			<< "\tperft\t(perft this position)" << endl
			<< "\tquit\t(exit program)" << endl
			<< endl;
	}
}

void dividePerft(board_t* b, int depth) {

	std::string move = "";
	Perft p;

	while (depth) {

		if (move != "") {
			int parsedMove = parseMove(b, move);
			push(b, parsedMove);
			depth--;
		}

		p.perftRoot(b, depth);

		cout << "\nDivide at move ";
		cin >> move;

		if (move == "quit") {
			return;
		}

	}

	cout << "Reached depth 0" << endl;
	return;

}

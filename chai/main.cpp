#include "main.h"

int main() {
	cout << "chai " << TOSTRING(VERSION) << endl
		<< "assert=" << info_ASSERT
		<< " buckets=" << BUCKETS
		<< " threads=" << NUM_THREADS << endl
		<< "compiler=" << info_COMPILER
		<< " date=" << __DATE__ << endl;

	// TODO: use stacksize attr in gcc build
	// TODO gitid

	// Init all tables and parameters
	init();
	initHashTables(p_board);

#ifdef EGTB
	const char* tbPath = "C:/egtb_files";
	if (!tb_init(tbPath)) {
		cout << "TB init failed." << endl;
		Assert(false);
		exit(1);
	}
	cout << "TB max=" << TB_LARGEST << endl;
#endif // DEBUG

	parseFen(p_board, STARTING_FEN);
	//parseFen(&board, "R7/6k1/P7/6p1/r5P1/3K4/8/8 w - - 0 1");

	printBoard();

	initThreadPool();

	cli();

	// Free all hash tables before exit
	freeHashTables();
	
#ifdef EGTB
	tb_free();
#endif // DEBUG

	deleteThreadPool();

	return 0;
}

void cli() {
	std::string userInput;

	while (1) {
		cin >> userInput;

		// start UCI protocol
		if (userInput == "uci") {
			uciMode(p_board, s);
			continue;
		}

		if (userInput == "quit") {
			break;
		}

		if (userInput == "test") {
			continue;
		}

		// POP MOVE FROM BOARD
		if (userInput == "pop") {
			undo_t undoPop = pop(p_board);
			cout << "Popped " << getStringMove(p_board, undoPop.move) << " from stack." << endl;
			printBoard();
			continue;
		}

		// start searching this position
		if (userInput == "s") {
			s->depth = 15;
			s->startTime = getTimeMs();
			s->stopTime = getTimeMs() + 10000;

			s->timeSet = false;
			s->depthSet = true;
			search(p_board, s);
			continue;
		}

		if (userInput == "perft") {
			cout << "Enter Perft depth: ";

			std::string perftDepth;
			cin >> perftDepth;

			if (stoi(perftDepth) >= 1 && stoi(perftDepth) <= 15) {
				dividePerft(p_board, stoi(perftDepth));
			} else {
				cerr << "Enter an integer between in [1, 15]." << endl;
			}
			cout << "Leaving perft option." << endl;
			continue;
		}

		if (userInput == "movegen") {
			moveList_t moveList[1];
			generateMoves(p_board, moveList, isCheck(p_board, p_board->stm));
			printGeneratedMoves(p_board, moveList);
			continue;
		}

		if (userInput == "fen") {
			cout << "Enter FEN: ";
			cin.ignore();
			getline(cin, userInput);
			cout << "Parsed FEN \"" << userInput << "\" into board." << endl;
			parseFen(p_board, userInput);
			printBoard();
			continue;
		}

		if (userInput == "print") {
			printBoard();
			continue;
		}

		if (userInput == "0000") {
			pushNull(p_board);
		}

		// Assume userInput is a move
		generateMoves(p_board, move_s, isCheck(p_board, p_board->stm));
		int parsedMove = parseMove(p_board, userInput);
		bool inputIsMove = false;
		for (int i = 0; i < move_s->cnt; i++) {
			if (parsedMove == move_s->moves[i]) {
				inputIsMove = true;
				break;
			}
		}

		if (inputIsMove) {
			push(p_board, parsedMove);
			printBoard();
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

void dividePerft(board_t* pBoard, int depth) {

	std::string move = "";
	Perft p;

	while (depth) {

		if (move != "") {
			int parsedMove = parseMove(pBoard, move);
			push(pBoard, parsedMove);
			depth--;
		}

		p.perftRoot(pBoard, depth);

		cout << "\nDivide at move ";
		cin >> move;

		if (move == "quit") {
			return;
		}

	}

	cout << "Reached depth 0" << endl;
	return;

}

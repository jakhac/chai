#include "main.h"

int main() {
	cout << "CHAI " << VERSION << endl
		<< "assert=" << info_ASSERT
		<< " buckets=" << BUCKETS << endl
		<< "compiler=" << info_COMPILER
		<< " date=" << __DATE__ << endl;

	init(&board);
	initHashTables(b);

	parseFen(&board, STARTING_FEN);
	//parseFen(&board, "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 1 1");

	printBoard(b);
	cli(b, &perft, s);

	freeHashTables(b->tt, b->pt);
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
				cerr << "Enter a number between 1 and 15." << endl;
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

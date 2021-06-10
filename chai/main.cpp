#include "main.h"


int main() {
	cout << "CHAI " << VERSION << endl
		<< "info: assert="
#ifdef ASSERT
		<< "1"
#else
		<< "0"
#endif // ASSERT
		<< " buckets=" << BUCKETS << endl;

	cout << "compiler="
#if defined(_MSC_VER)
		<< "MSVC"
#elif defined(__GNUC__)
		<< "GCC"
#endif
		<< " date=" << __DATE__
		<< endl;

	init(&board);
	parseFen(&board, STARTING_FEN);

#ifdef INFO
	printBoard(&board);
#endif // INFO

	cli(b, &perft, s);

	freeTT(b->tt, b->pt);
	return 0;
}

void cli(board_t* b, Perft* p, search_t* s) {
	string userInput;

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
			cout << "Enter perft depth: ";

			string perftDepth;
			cin >> perftDepth;

			if (stoi(perftDepth) >= 1 && stoi(perftDepth) <= 15) {
				dividePerft(b, stoi(perftDepth));
			}
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
			<< "\t[move]\t(apply move)" << endl
			<< "\tpop\t(undo move)" << endl
			<< "\tfen\t(parse fen)" << endl
			<< "\tprint\t(print board status)" << endl
			<< "\tperft\t(perft this position)" << endl
			<< "\tquit\t(exit program)" << endl
			<< endl;
	}
}

void dividePerft(board_t* b, int depth) {

	string move = "";
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

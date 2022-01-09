#include "main.h"

int main() {
	// Print meta information at every startup
	printEngineMeta(info_ASSERT, info_COMPILER, info_SIMD);

	// Init all tables and parameters
	init();
	initHashTables();
	initEGTB("");
	initThreadPool();

	// Print status and drop into cli protocol
	parseFen(&_board, STARTING_FEN);
	// parseFen(&_board, "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1");

	printBoard(&_board);
	cli(&_board, &_instr, &_stats, &_perft);

	// Free hash tables and allocated memory before exit
	freeHashTables();
	freeEGTB();
	deleteThreadPool();

	return EXIT_SUCCESS;
}

void cli(board_t* b, instr_t* i, stats_t* s, Perft* p) {
	moveList_t moveList;
	move_t parsedMove;
	std::string userInput;

	parseNet(b);

	// bitboard_t t = wBackwardPawns(b);
	// printBitBoard(&t);

	// for (int j = 0; j < 64; j++) {
		// bitboard_t t = dangerZone[chai::BLACK][E4];
		// printBitBoard(&t);
	// }

	// tuple_t tup = t(2, 4);
	// cout << t1(tup) <<  " " << t2(tup) << endl;

	while (1) {
		cin >> userInput;

		// Start UCI protocol.
		if (userInput == "uci") {
			uciMode(b, s, i);
			break;
		}

		// Exit program.
		if (userInput == "quit") {
			break;
		}

		// Start searching this position.
		if (userInput == "s") {
			i->depth = 15;
			i->timeSet = false;

			search(b, s, i);
			continue;
		}

		// Perft current position.
		if (userInput == "perft") {
			cout << "Enter Perft depth: ";

			std::string perftDepth;
			cin >> perftDepth;

			if (stoi(perftDepth) >= 1 && stoi(perftDepth) <= 15) {
				dividePerft(p, b, stoi(perftDepth));
			} else {
				cerr << "Enter an integer between in [1, 15]." << endl;
			}
			cout << "Leaving perft option." << endl;
			continue;
		}

		// Generate and print all moves for current position.
		if (userInput == "movegen") {
			generateMoves(b, &moveList, isCheck(b, b->stm));
			printGeneratedMoves(b, &moveList);
			continue;
		}

		if (userInput == "eval") {
			cout << "Eval = " << evaluation(b) << " (white side)" << endl;
			continue;
		}

		// Parse fen into board variables.
		if (userInput == "fen") {
			cout << "Enter FEN: ";
			cin.ignore();
			getline(cin, userInput);
			cout << "Parsed FEN \"" << userInput << "\" into board." << endl;
			parseFen(b, userInput);
			printBoard(b);
			continue;
		}

		// Print board all game state variables.
		if (userInput == "print") {
			printBoard(b);
			continue;
		}

		// Pop move from board.
		if (userInput == "pop") {
			undo_t undoPop = pop(b);
			cout << "Popped " << getStringMove(b, undoPop.move) 
				 << " from stack." << endl;
			printBoard(b);
			continue;
		}

		// Push null move.
		if (userInput == "0000") {
			pushNull(b);
			continue;
		}

		// Check if a valid move was entered by user. If so, push move.
		if (stringIsValidMove(b, userInput, &parsedMove)) {
			cout << endl;
			push(b, parsedMove);
			printBoard(b);

			// NNUE debug
			propagate(b);

			// accumulateFeatures(b, chai::WHITE);
			// accumulateFeatures(b, chai::BLACK);

			// accum_t* accDstTest = &b->accum[b->ply];
			// assertActiveFeatures(b, chai::WHITE, accDstTest);
			// assertActiveFeatures(b, chai::BLACK, accDstTest);
			// cout << "Passed asserts" << endl;

			continue;
		}

		printCliHelp();
	}
}

void dividePerft(Perft* p, board_t* b, int depth) {
	std::string move = "";

	while (depth) {

		if (move != "") {
			int parsedMove = parseMove(b, move);
			push(b, parsedMove);
			depth--;
		}

		p->perftRoot(b, depth);

		cout << "\nDivide at move ";
		cin >> move;

		if (move == "quit") {
			return;
		}

	}

	cout << "Reached depth 0" << endl;
	return;

}

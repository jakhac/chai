#include "main.h"

int main() {
	// Print meta information at every startup
	printEngineMeta(info_ASSERT, info_COMPILER, info_SIMD);

	// Init all tables and parameters
	init();
	initHashTables();
	initEGTB("");
	initThreadPool();

#if defined(USE_NNUE) && defined(CUSTOM_EVALFILE)
	initIncNet();
	cout << "NNUE is now active." << endl;
#endif

	// Print status and drop into cli protocol
	// parseFen(&_board, STARTING_FEN);
	parseFen(&_board, "7Q/4P1k1/5p2/1p3Kp1/pB2p2p/P3P3/1P5R/8 b - - 91 93");

	printBoard(&_board);
	cli(&_board, &_instr, &_stats, &_perft); // TODO move cli func to info

// 2022-01-27 23:37:19.056-->2:position startpos moves c2c4 g8f6 b1c3 e7e5 e2e3 f8b4 d1b3 b8c6 g1e2 e8g8 a2a3 b4c3 e2c3 f8e8 d2d3 b7b6 f1e2 d7d6 e1g1 c8f5 b3d1 a7a5 f2f4 e5e4 d3d4 d8d7 h2h3 f5h3 g2h3 d7h3 f1f2 c6e7 f2h2 h3g3 h2g2 g3h3 d1f1 g8h8 c1d2 e8f8 g2h2 h3f1 a1f1 e7f5 g1f2 c7c6 d4d5 c6d5 c3d5 f6d5 c4d5 h8g8 d2c3 a5a4 f1g1 f7f6 e2g4 f5h6 g4e6 g8h8 c3b4 h6f7 g1c1 h7h6 c1c7 f7d8 f4f5 d8e6 f5e6 h8g8 f2g3 a8c8 b4d6 f8e8 g3f4 b6b5 c7c8 e8c8 d6b4 h6h5 d5d6 c8a8 d6d7 g7g5 f4f5 g8g7 e6e7 a8h8 d7d8q h5h4 d8h8

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

	// cout << "Propagate " << propagate(b) << endl;
	// cout << "nnue eval " << evaluateNNUE(b) << endl;

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
			i->depth = 40;
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

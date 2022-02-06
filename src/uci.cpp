#include "uci.h"

namespace UCI {


void cli(Board* b, Instructions* i, Stats* s, Perft* p) {
	MoveList moveList;
	Move parsedMove;
	std::string userInput;

	// cout << "Propagate " << propagate(b) << endl;
	// cout << "nnue eval " << evaluateNNUE(b) << endl;

	// Bitboard = 
	// printBitBoard(&t);

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

			Search::search(b, s, i);
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
			// cout << "Eval = " << evaluation(b) << " (white side)" << endl;
			printEval(b);
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
			Undo undoPop = pop(b);
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
			NNUE::propagate(b);

			continue;
		}

		printCliHelp();
	}
}

void uciMode(Board* b, Stats* s, Instructions* i) {
	std::string cmd;

	printUCI_Info();

	while (true) {
		getline(std::cin, cmd);
		fflush(stdout);

		if (!cmd.compare("quit")) break;
		if (!cmd.compare("uci")) printUCI_Info();
		if (!cmd.compare("isready")) cout << "readyok" << endl;
		if (!cmd.compare("ucinewgame")) uciParsePosition(b, "position startpos");

		if (strStartsWith(cmd, "position")) uciParsePosition(b, cmd);
		if (strStartsWith(cmd, "go")) uciParseGo(b, s, i, cmd);
		if (strStartsWith(cmd, "setoption")) {
 			try {
				uciSetOption(cmd);
			} catch(const std::invalid_argument& ia) {
				std::cerr << ia.what() << " error. Invalid user input." << endl;
			}
		}

		fflush(stdout);
	}
}

void uciSetOption(std::string cmd) {

	if (strStartsWith(cmd, "setoption name Hash value ")) {
		int newMbSize = stoi(cmd.substr(26, std::string::npos));

		if (!TT::resizeHashTables(newMbSize)) {
			cerr << "info string Requested hash size is not inside ["
				 << MIN_TT_SIZE << ", " << MAX_TT_SIZE << "]" << endl;
			return;
		}

		cout << "info string set Hash to " << newMbSize << "MB" << endl;
	}

	if (strStartsWith(cmd, "setoption name SyzygyPath value ")) {
		std::string syzygyPath = cmd.substr(32, std::string::npos);

        if (!syzygyPath.compare("")) {
        	cout << "info string SyzygyPath is empty." << endl;
			return;
		}

		EGTB::init(syzygyPath.c_str());
        cout << "info string Set SyzygyPath to " << syzygyPath 
			 << ". Max TB=" << TB_LARGEST << endl;
    }

	if (strStartsWith(cmd, "setoption name Threads value ")) {
		int requestedThreads = stoi(cmd.substr(29, std::string::npos));
		
		if (!Threads::resizePool(requestedThreads)) {
			cout << "info string Requested Thread value is invalid." << endl;
			return;
		}

        cout << "info string set total Threads to " << NUM_THREADS << endl;
    }

	if (strStartsWith(cmd, "setoption name EvalFile value ")) {
		std::string nnuePath = cmd.substr(30, std::string::npos);
  		std::ifstream nnueData(nnuePath, std::ios::binary);

		if (!NNUE::initNet(nnueData)) {
        	cerr << "info string Error: NNUE parsed incorrect hash value." << endl;
			return;
		}

    	cerr << "info string NNUE parsed successfully." << endl;
	}

}

void uciParsePosition(Board* b, std::string cmd) {

	if (strStartsWith(cmd, "position fen")) {
		std::string fen = cmd.substr(13, cmd.size());
		parseFen(b, fen);

		return;
	}

	if (strStartsWith(cmd, "position startpos moves")) {
		parseFen(b, STARTING_FEN);
		std::string move;

		std::istringstream iss(cmd.substr(24, cmd.size()));
		std::vector<std::string> tokens{ std::istream_iterator<std::string>{iss},
						  std::istream_iterator<std::string>{} };

		// Push all moves
		int parsedMove;
		for (std::string m : tokens) {
			parsedMove = parseMove(b, m);
			push(b, parsedMove);

			// Push increases ply by default. We have to keep it at zero,
			// otherwise some of our arrays are indexed out-of-bounds!
			b->ply = 0;
		}

		return;
	}

	if (strStartsWith(cmd, "position startpos")) {
		parseFen(b, STARTING_FEN);

		return;
	}

	fflush(stdout);
}

void uciParseGo(Board* b, Stats* s, Instructions* instr, std::string cmd) {
	instr->startTime = getTimeMs();

	int depth    = 0; // search to this depth
	int timeLeft = 0; // time left on (our) clock
	int inc;	      // increment

	std::istringstream iss(cmd);
	std::vector<std::string> tokens { 
		std::istream_iterator<std::string>{iss},
		std::istream_iterator<std::string>{} 
	};

	for (int i = 0; i < (int)tokens.size() - 1; i++) {
		if (tokens[i] == "infinite") continue;
		if (tokens[i] == "binc" && b->stm == BLACK) inc = stoi(tokens[i + 1]);
		if (tokens[i] == "winc" && b->stm == WHITE) inc = stoi(tokens[i + 1]);
		if (tokens[i] == "btime" && b->stm == BLACK) timeLeft = stoi(tokens[i + 1]);
		if (tokens[i] == "wtime" && b->stm == WHITE) timeLeft = stoi(tokens[i + 1]);
		if (tokens[i] == "depth") depth = stoi(tokens[i + 1]);
	}

	// Depth is either set as "go depth x" command or MAX_DEPTH of engine
	instr->depth = (depth != 0) ? depth : MAX_DEPTH;

	// If timeLeft is provided, search with time constraints
	instr->timeSet = false;
	if (timeLeft) {
		instr->timeSet       = true;
		instr->timeLeft      = timeLeft;
		instr->allocatedTime = allocateTime(b, timeLeft, inc);
	}

	Search::search(b, s, instr);
}


}

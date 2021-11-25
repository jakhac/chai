#include "uci.h"

static bool strStartsWith(std::string str, std::string start) {
	return str.rfind(start, 0) == 0;
}

void uciMode(board_t* b, search_t* s) {
	std::string cmd;

	printUCI_Info();

	while (true) {
		getline(std::cin, cmd);
		fflush(stdout);

		if (!cmd.compare("quit")) {
			break;
		}

		if (!cmd.compare("uci")) {
			printUCI_Info();
		}
 
		if (!cmd.compare("isready")) {
			init();
			initHashTables();

			uciParsePosition(b, "position startpos");
			cout << "readyok\n";
		}

		if (!cmd.compare("ucinewgame")) {
			uciParsePosition(b, "position startpos");
		}

		if (strStartsWith(cmd, "position")) {
			uciParsePosition(b, cmd);
		}

		if (strStartsWith(cmd, "go")) {
			uciParseGo(b, s, cmd);
		}

		if (strStartsWith(cmd, "setoption")) {
 			try {
				uciSetOption(cmd);
			} catch(const std::invalid_argument& ia) {
				std::cerr << ia.what() << " error. Invalid user input." << endl;
			}
		}

		fflush(stdout);
	}

	cout << "Left uci mode." << endl;

}

void uciSetOption(std::string cmd) {

	if (strStartsWith(cmd, "setoption name Hash value ")) {
		int newMbSize = stoi(cmd.substr(strlen("setoption name Hash value "), std::string::npos));
		if (resizeHashTables(newMbSize)) {
			cout << "info string set Hash to " << newMbSize << "MB" << endl;
		}
	}

	if (strStartsWith(cmd, "setoption name SyzygyPath value ")) {
		std::string syzygyPath = cmd.substr(strlen("setoption name SyzygyPath value "), std::string::npos);
		// freeEGTB(); error if used to reset path
        if (!syzygyPath.compare("")) {
        	cout << "info string Error: SyzygyPath is empty." << endl;
			return;
		}
		initEGTB(syzygyPath.c_str());
        cout << "info string set SyzygyPath to " 
			 << syzygyPath << ". Max TB=" << TB_LARGEST << endl;
    }

	if (strStartsWith(cmd, "setoption name Threads value ")) {
		int requestedThreads = stoi(cmd.substr(strlen("setoption name Threads value "), std::string::npos));
		
		if (!resizeThreadPool(requestedThreads)) {
			cout << "info string Threads value is invalid." << endl;
			return;
		}

        cout << "info string set total Threads to " << NUM_THREADS << endl;
    }

}

void uciParsePosition(board_t* b, std::string cmd) {

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
		}

		return;
	}

	if (strStartsWith(cmd, "position startpos")) {
		parseFen(b, STARTING_FEN);

		return;
	}

	fflush(stdout);
}
/*
void uciParseGo(board_t* b, search_t* s, std::string cmd) {
	s->startTime = getTimeMs();

	int depth = -1;

	// int movesLeft = 30;
	int movesLeft = -1;

	int moveTime = -1;
	int time = -1;
	int inc = 0;
	s->timeSet = false;

	std::istringstream iss(cmd);
	std::vector<std::string> tokens{ std::istream_iterator<std::string>{iss},
					 std::istream_iterator<std::string>{} };

	for (int i = 0; i < (int)tokens.size() - 1; i++) {
		if (tokens[i] == "infinite") continue;
		if (tokens[i] == "binc" && b->stm == chai::BLACK) inc = stoi(tokens[i + 1]);
		if (tokens[i] == "winc" && b->stm == chai::WHITE) inc = stoi(tokens[i + 1]);
		if (tokens[i] == "btime" && b->stm == chai::BLACK) time = stoi(tokens[i + 1]);
		if (tokens[i] == "wtime" && b->stm == chai::WHITE) time = stoi(tokens[i + 1]);
		if (tokens[i] == "movestogo") movesLeft = stoi(tokens[i + 1]);
		if (tokens[i] == "movetime") moveTime = stoi(tokens[i + 1]);
		if (tokens[i] == "depth") depth = stoi(tokens[i + 1]);
	}

	// if fixed search time is set
	if (moveTime != -1) {
		time = moveTime;
		movesLeft = 1;
	}

	//cout << "Parsed wbtime:" << time << " wbinc:" << inc << " movetime:" << moveTime << endl;

	// if stop is set
	// s->startTime = getTimeMs();
	if (time != -1) {
		// if (movesLeft < 1 || movesLeft > 30) movesLeft = 30;
		movesLeft = (movesLeft == -1) ? remainingHalfMoves(b)
									  : movesLeft;

		s->timeSet = true;

		// catch low time (ok for tc 20/0.3)
		if (time <= 3000) {
			s->stopTime = s->startTime + (inc / 8);
		} else if (time <= 4000) {
			s->stopTime = s->startTime + (inc / 4);
		} else {
			time /= movesLeft;
			time -= 50;
			//s->stopTime = s->startTime + time + inc;
			s->stopTime = s->startTime + time;
		}

		Assert(s->startTime < s->stopTime);
	}

	if (depth == -1) {
		s->depth = MAX_DEPTH;
	} else {
		s->depth = depth;
	}

	search(b, s);
}
*/

void uciParseGo(board_t* b, search_t* s, std::string cmd) {
	s->startTime = getTimeMs();

	int depth         = 0; // search to this depth
	int timeLeft      = 0; // time left on (our) clock
	int inc;  			// increment

	std::istringstream iss(cmd);
	std::vector<std::string> tokens{ std::istream_iterator<std::string>{iss},
					 std::istream_iterator<std::string>{} };

	for (int i = 0; i < (int)tokens.size() - 1; i++) {
		if (tokens[i] == "infinite") continue;
		if (tokens[i] == "binc" && b->stm == chai::BLACK) inc = stoi(tokens[i + 1]);
		if (tokens[i] == "winc" && b->stm == chai::WHITE) inc = stoi(tokens[i + 1]);
		if (tokens[i] == "btime" && b->stm == chai::BLACK) timeLeft = stoi(tokens[i + 1]);
		if (tokens[i] == "wtime" && b->stm == chai::WHITE) timeLeft = stoi(tokens[i + 1]);
		if (tokens[i] == "depth") depth = stoi(tokens[i + 1]);
	}

	// Depth is either set as go depth command or MAX_DEPTH of engine
	s->depth = (depth != 0) ? depth : MAX_DEPTH;

	// If timeLeft is provided, search with time constraints
	if (timeLeft) {
		s->timeSet       = true;
		s->timeLeft      = timeLeft;
		s->allocatedTime = allocateTime(b, timeLeft, inc);
	}

	search(b, s);
}

void init() {
#ifdef INFO
	auto start = std::chrono::high_resolution_clock::now();
#endif //INFO
	initClearSetMask();
	initSquareToRankFile();
	initAttackerMasks();
	initMVV_LVA();
	initEvalMasks();
	initManhattenMask();

	initObstructed();
	initLine();
	initSearch();

#ifdef INFO
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	cout << "Init keys and masks ... " << duration.count() << "ms\n";
	start = std::chrono::high_resolution_clock::now();
#endif // !INFO

	initRookMasks();
	initRookMagicTable();
	initBishopMasks();
	initBishopMagicTable();

#ifdef INFO
	stop = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	cout << "Init magic tables for bishop and rooks ... " << duration.count() << "ms\n" << endl;
#endif // !INFO
}


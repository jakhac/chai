#include "uci.h"

using namespace chai;


static bool strStartsWith(std::string str, std::string start) {
	return str.rfind(start, 0) == 0;
}

void init() {
	initClearSetMask();
	initSquareToRankFile();
	initAttackerMasks();
	initMVV_LVA();
	initEvalMasks();
	initManhattenMask();

	initHorizontalNeighbors();

	initObstructed();
	initLine();
	initSearch();

	initRookMasks();
	initRookMagicTable();
	initBishopMasks();
	initBishopMagicTable();
}

void uciMode(board_t* b, stats_t* s, instr_t* i) {
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

		if (!resizeHashTables(newMbSize)) {
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

		initEGTB(syzygyPath.c_str());
        cout << "info string Set SyzygyPath to " << syzygyPath << ". Max TB=" << TB_LARGEST << endl;
    }

	if (strStartsWith(cmd, "setoption name Threads value ")) {
		int requestedThreads = stoi(cmd.substr(29, std::string::npos));
		
		if (!resizeThreadPool(requestedThreads)) {
			cout << "info string Requested Thread value is invalid." << endl;
			return;
		}

        cout << "info string set total Threads to " << NUM_THREADS << endl;
    }

	if (strStartsWith(cmd, "setoption name EvalFile value ")) {
		std::string netDir   = "../nets/";
		std::string nnuePath = cmd.substr(30, std::string::npos);

  		std::ifstream nnueData(netDir + nnuePath, std::ios::binary);

		if (!initNet(nnueData)) {
        	cerr << "info string Error: NNUE parsed incorrect hash value." << endl;
			return;
		}

    	cerr << "info string NNUE parsed successfully." << endl;
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

void uciParseGo(board_t* b, stats_t* s, instr_t* instr, std::string cmd) {
	instr->startTime = getTimeMs();

	int depth         = 0; // search to this depth
	int timeLeft      = 0; // time left on (our) clock
	int inc;			   // increment

	std::istringstream iss(cmd);
	std::vector<std::string> tokens{ std::istream_iterator<std::string>{iss},
					 std::istream_iterator<std::string>{} };

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

	search(b, s, instr);
}


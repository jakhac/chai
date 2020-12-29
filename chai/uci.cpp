#include "uci.h"

void uciMode(Board* b, search_t* s) {
	string cmd;

	log("UCI MODE");

	cout << "id name chai Chess Engine\n";
	cout << "id author chai\n";
	cout << "uciok\n";

	while (true) {
		getline(cin, cmd);
		fflush(stdout);
		log("cmd received: " + cmd);

		if (!cmd.compare("quit")) {
			cout << "quit game\n";
			exit(0);
		} else if (!cmd.compare("isready")) {
			init(b);
			cout << "isreadyok\n";
		} else if (!cmd.compare("ucinewgame")) {
			// start new game with standard position
			uciParsePosition(b, "position startpos");
		} else if (!cmd.substr(0, 8).compare("position")) {
			// position startpos / fen / moves
			uciParsePosition(b, cmd);
		} else if (!cmd.substr(0, 2).compare("go")) {
			uciParseGo(b, s, cmd);
		}

		fflush(stdout);
	}
	log("Error: left uci loop");
	cout << "Error: left uci loop" << endl;
}

void uciParsePosition(Board* b, string cmd) {
	// parse fen
	log("Parse position: " + cmd);
	if (!cmd.substr(0, 12).compare("position fen")) {
		string fen = cmd.substr(13, cmd.size());
		b->parseFen(fen);

	} else if (!cmd.substr(0, 23).compare("position startpos moves")) {
		/*position startpos moves b2b4 a7a5 b4a5 b7b6 a5b6 h7h6 b6c7 g7g6 c7b8q d8c7*/
		b->parseFen(STARTING_FEN);
		int cnt = 24;
		string move;

		istringstream iss(cmd.substr(24, cmd.size()));
		vector<string> tokens{ istream_iterator<string>{iss},
						  istream_iterator<string>{} };

		int parsedMove;
		for (string move : tokens) {
			parsedMove = b->parseMove(move);
			b->push(parsedMove);
		}
		log("Pushed all moves from cmd");

		// reset ply to 0 (incremented for each push call) for search
		b->ply = 0;

	} else if (!cmd.compare("position startpos")) {
		b->parseFen(STARTING_FEN);
	} else {
		cout << "uciParsePosition failed\n";
		log("uciParsePosition failed");
	}
	log("Parsing complete");

	b->printBoard();

	fflush(stdout);
}

void uciParseGo(Board* b, search_t* s, string cmd) {
	int depth = -1, movesLeft = 30, moveTime = -1;
	int time = -1, inc = 0;
	s->timeSet = false;

	log("Parse go: " + cmd);

	istringstream iss(cmd);
	vector<string> tokens{ istream_iterator<string>{iss},
					  istream_iterator<string>{} };

	for (int i = 0; i < tokens.size() - 1; i++) {
		if (tokens[i] == "infinite") continue;
		if (tokens[i] == "binc" && b->side == BLACK) inc = stoi(tokens[i + 1]);
		if (tokens[i] == "winc" && b->side == WHITE) inc = stoi(tokens[i + 1]);
		if (tokens[i] == "btime" && b->side == BLACK) time = stoi(tokens[i + 1]);
		if (tokens[i] == "wtime" && b->side == WHITE) time = stoi(tokens[i + 1]);
		if (tokens[i] == "movestogo") movesLeft = stoi(tokens[i + 1]);
		if (tokens[i] == "movetime") moveTime = stoi(tokens[i + 1]);
		if (tokens[i] == "depth") depth = stoi(tokens[i + 1]);
	}

	// if fixed search time is set
	if (moveTime != -1) {
		time = moveTime;
		movesLeft = 1;
	}

	cout << "Parsed wbtime:" << time << " wbinc:" << inc << " movetime:" << moveTime << endl;

	// if stop is set
	s->startTime = getTimeMs();
	if (time != -1) {
		if (movesLeft < 1 || movesLeft > 30) movesLeft = 30;
		s->timeSet = true;

		// catch low time
		//if (time <= 3000) {
			//cout << "Set time to 750ms due to low move time. (was " << time << ")" << endl;
			//s->stopTime = s->startTime + 750;
		if (time <= 1500) {
			s->stopTime = s->startTime + 300;
		} else {
			time /= movesLeft;
			time -= 50;
			s->stopTime = s->startTime + time + inc;
		}

		ASSERT(s->startTime < s->stopTime);
	}

	if (depth == -1) {
		s->depth = MAX_DEPTH;
	} else {
		s->depth = depth;
	}

	cout << "time " << time << ", start " << s->startTime << ", stop " << s->stopTime
		<< ", depth " << s->depth << ", timeset " << s->timeSet << endl;

	log("Parse go finished, start search now");
	search(b, s);
}

void init(Board* b) {
	cout << "Initialize chai. " << VERSION << endl;

	auto start = std::chrono::high_resolution_clock::now();
	initClearSetMask();
	initSquareToRankFile();
	b->initHashKeys();
	initAttackerMasks();
	initMVV_LVA();
	initEvalMasks();
	initManhattenMask();

	initTT(b->tt);
	initPawnTable(b->pawnTable);

	// TODO performance
	initObstructed();
	initLine();

	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	cout << "Init keys and masks ... " << duration.count() << "ms\n";

	start = std::chrono::high_resolution_clock::now();
	initRookMasks();
	initRookMagicTable();
	initBishopMasks();
	initBishopMagicTable();
	stop = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	cout << "Init magic tables for bishop and rooks ... " << duration.count() << "ms\n" << endl;
}


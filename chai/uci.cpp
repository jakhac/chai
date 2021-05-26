#include "uci.h"

void uciMode(board_t* b, search_t* s) {
	string cmd;

	cout << "id name jakhac\n";
	cout << "id author chai\n";
	cout << "uciok\n";

	while (true) {
		getline(cin, cmd);
		fflush(stdout);

		if (!cmd.compare("quit")) {
			//cout << "quit uci protocol" << endl;
			exit(0);
			return;
		} else if (!cmd.compare("uci")) {
			cout << "id name chai" << VERSION << "\n";
			cout << "id author chai\n";
			cout << "uciok\n";
		} else if (!cmd.compare("isready")) {
			init(b);
			uciParsePosition(b, "position startpos");
			cout << "readyok\n";
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

}

void uciParsePosition(board_t* b, string cmd) {
	// parse fen
	if (!cmd.substr(0, 12).compare("position fen")) {
		string fen = cmd.substr(13, cmd.size());
		parseFen(b, fen);

	} else if (!cmd.substr(0, 23).compare("position startpos moves")) {
		/*position startpos moves b2b4 a7a5 b4a5 b7b6 a5b6 h7h6 b6c7 g7g6 c7b8q d8c7*/
		parseFen(b, STARTING_FEN);
		int cnt = 24;
		string move;

		istringstream iss(cmd.substr(24, cmd.size()));
		vector<string> tokens{ istream_iterator<string>{iss},
						  istream_iterator<string>{} };

		int parsedMove;
		for (string move : tokens) {
			parsedMove = parseMove(b, move);
			push(b, parsedMove);
		}

		// reset ply to 0 (incremented for each push call) for search
		b->ply = 0;

	} else if (!cmd.compare("position startpos")) {
		parseFen(b, STARTING_FEN);
	} else {
		cout << "uciParsePosition failed\n";
	}

	//b->printBoard();

	fflush(stdout);
}

void uciParseGo(board_t* b, search_t* s, string cmd) {
	int depth = -1, movesLeft = 30, moveTime = -1;
	int time = -1, inc = 0;
	s->timeSet = false;

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

#ifdef INFO
	cout << "Parsed wbtime:" << time << " wbinc:" << inc << " movetime:" << moveTime << endl;
#endif // !LICHESS

	// if stop is set
	s->startTime = getTimeMs();
	if (time != -1) {
		if (movesLeft < 1 || movesLeft > 30) movesLeft = 30;
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

void init(board_t* b) {
	auto start = std::chrono::high_resolution_clock::now();
	initClearSetMask();
	initSquareToRankFile();
	initAttackerMasks();
	initMVV_LVA();
	initEvalMasks();
	initManhattenMask();

	initTT(b->tt);
	initPawnTable(b->pawnTable);

	// TODO performance
	initObstructed();
	initLine();

#ifdef INFO
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	cout << "Init keys and masks ... " << duration.count() << "ms\n";
#endif // !1

	start = std::chrono::high_resolution_clock::now();
	initRookMasks();
	initRookMagicTable();
	initBishopMasks();
	initBishopMagicTable();
#ifdef INFO
	stop = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	cout << "Init magic tables for bishop and rooks ... " << duration.count() << "ms\n" << endl;
#endif // !LICHESS
}


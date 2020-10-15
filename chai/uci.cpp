#include "uci.h"

void uciMode(Board* b, SEARCH_INFO_S* s) {
	string cmd;

	cout << "id name chai Chess Engine\n";
	cout << "id author chai\n";
	cout << "uciok\n";

	while (true) {
		fflush(stdout);
		getline(cin, cmd);

		if (!cmd.compare("quit")) {
			cout << "quit game\n";
			exit(0);
		} else if (!cmd.compare("isready")) {
			init(b);
			cout << "isreadyok\n";
		} else if (!cmd.compare("ucinewgame")) {
			// start new game with standard position
			uciParsePosition(b, "position startpos\n");
		} else if (!cmd.substr(0, 8).compare("position")) {
			// position startpos / fen / moves
			uciParsePosition(b, cmd);
		} else if (!cmd.substr(0, 2).compare("go")) {
			uciParseGo(b, s, cmd);
		}
	}
}

void uciParsePosition(Board* b, string cmd) {
	fflush(stdout);

	// parse fen
	if (!cmd.substr(0, 12).compare("position fen")) {
		string fen = cmd.substr(13, cmd.size());
		b->parseFen(fen);

	} else if (!cmd.substr(0, 23).compare("position startpos moves")) {
		// position startpos moves b2b4 a7a5 b4a5 b7b6 a5b6 h7h6 b6c7 g7g6 c7b8q d8c7
		b->parseFen(STARTING_FEN);
		int cnt = 24;
		string move;

		while (cnt < cmd.size()) {
			move = cmd.substr(cnt, cnt + 5);
			int parsedMove = b->parseMove(move);
			b->push(parsedMove);

			while (cmd[cnt] != ' ') {
				cnt++;
			}
			cnt++;
		}
	} else if (!cmd.compare("position startpos")) {
		b->parseFen(STARTING_FEN);
	}
}

void uciParseGo(Board* b, SEARCH_INFO_S* s, string cmd) {
	int depth = -1,  movesLeft = 30, moveTime = -1;
	int time = -1, inc = 0;

	s->timeSet = false;

	using namespace std;
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
	 
	if (moveTime != -1) {
		time = moveTime;
		movesLeft = 1;
	}

	s->startTime = getTimeMs();
	s->depth = depth;

	if (time != -1) {
		s->timeSet = true;
		time /= movesLeft;
		time -= 50;
		s->stopTime = s->startTime + time + inc;
	}

	if (depth == -1) {
		s->depth = MAX_DEPTH;
	}

	cout << "time " << time << ", start " << s->startTime << ", stop " << s->stopTime
		<< ", depth " << s->depth  << ", timeset " << s->timeSet << endl;

	search(b, s);
}

void init(Board* b) {

	auto start = std::chrono::high_resolution_clock::now();
	initClearSetMask();
	initSquareToRankFile();
	b->initHashKeys();
	initAttackerMasks();
	initMVV_LVA();
	initPVTable(b->pvTable_s);
	initEval();

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


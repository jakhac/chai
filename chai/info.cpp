#include "info.h"

void printBitBoard(bitboard_t* bb) {
	bitboard_t shiftBit = 1ULL;
	int sq;

	std::cout << std::endl;
	for (int rank = RANK_8; rank >= RANK_1; rank--) {
		for (int file = FILE_A; file <= FILE_H; file++) {
			sq = 8 * rank + file;

			if ((shiftBit << sq) & *bb) {
				std::cout << "1 ";
			} else {
				std::cout << ". ";
			}
		}
		std::cout << std::endl;
	}
}

void printMove(const int move) {

	if (move == NULL_MOVE) {
		cout << "0000" << endl;
		return;
	}

	int promoted = promPiece(move);
	char promChar = ' ';

	if (promoted) {
		promChar = 'q';
		if (promoted == Piece::n || promoted == Piece::N) {
			promChar = 'n';
		} else if (promoted == Piece::r || promoted == Piece::R) {
			promChar = 'r';
		} else if (promoted == Piece::b || promoted == Piece::B) {
			promChar = 'b';
		}
	}

	string ret = "";
	ret += ('a' + squareToFile[fromSq(move)]);
	ret += ('1' + squareToRank[fromSq(move)]);
	ret += ('a' + squareToFile[toSq(move)]);
	ret += ('1' + squareToRank[toSq(move)]);

	cout << ret << promChar << endl;
}

string getStringMove(const int move) {
	if (move == NULL_MOVE) {
		return "0000";
	}

	int promoted = promPiece(move);
	string promChar = " ";

	if (promoted) {
		promChar = "q ";
		if (promoted == Piece::n || promoted == Piece::N) {
			promChar = "n ";
		} else if (promoted == Piece::r || promoted == Piece::R) {
			promChar = "r ";
		} else if (promoted == Piece::b || promoted == Piece::B) {
			promChar = "b ";
		}
	}

	string ret = "";
	ret += ('a' + squareToFile[fromSq(move)]);
	ret += ('1' + squareToRank[fromSq(move)]);
	ret += ('a' + squareToFile[toSq(move)]);
	ret += ('1' + squareToRank[toSq(move)]);

	ret += promChar;
	return ret;
}

void printMoveStatus(int move) {
	cout << "\n#### - Move Status: " << getStringMove(move) << endl;
	cout << "From " << fromSq(move) << " to " << toSq(move) << endl;
	cout << "Pawn start " << (move & MFLAG_PS) << endl;
	cout << "EP capture " << (move & MFLAG_EP) << endl;
	cout << "Castle move " << (move & MFLAG_CAS) << endl;
	cout << "Promoted " << (move & MCHECK_PROM) << endl;
	cout << "Promoted piece " << (promPiece(move)) << endl;
	cout << "Capture " << (move & MCHECK_CAP) << " with captured piece " << capPiece(move) << endl;
	cout << "####\n" << endl;
}

void printBinary(bitboard_t x) {
	std::bitset<64> b(x);
	cout << b << endl;
}

void printSearchInfo(board_t* b) {
	cout << "\n";
}

void log(string logMsg) {
	ofstream ofs("log.txt", std::ios_base::out | std::ios_base::app);
	ofs << getTime() << "\t" << logMsg << '\n';
	ofs.close();
}

void readInput(search_t* s) {
	int bytes;
	char input[256] = "", * endc;

	if (inputWaiting()) {
		s->stopped = true;
		do {
			bytes = _read(_fileno(stdin), input, 256);
		} while (bytes < 0);
		endc = strchr(input, '\n');
		if (endc)
			*endc = 0;

		if (strlen(input) > 0) {
			if (!strncmp(input, "quit", 4)) {
				cout << "READ INPUT: quit" << endl;
				s->quit = true;
			}
		}
		return;
	}
}

void printUCI(search_t* s, int d, int selDpt, int score) {
	string scoreStr = " score ";

	if (abs(score) >= ISMATE) {
		string sign = (score > 0) ? "" : "-";
		scoreStr += "mate " + sign + to_string(MATE_VALUE - abs(score));
	} else {
		scoreStr += "cp " + to_string(score);
	}

	cout << "info depth " << d
		<< " seldepth " << selDpt
		<< scoreStr
		<< " nodes " << (s->nodes + s->qnodes)
		<< " time " << (getTimeMs() - s->startTime);
}

void printPV(move_t* moves, int len) {
	cout << " pv ";

	for (int i = 0; i < len; i++) {
		cout << getStringMove(moves[i]);
	}

	//#define STRUCT_PV
	//#ifdef STRUCT_PV
	//	for (int i = 0; i < pvLine->len; i++) {
	//		cout << getStringMove(pvLine->line[i]);
	//	}
	//#else
	//	for (int i = 0; i < pvMoves; i++) {
	//		cout << getStringMove(b->pvArray[i]);
	//	}
	//#endif // STRUCT_PV
}

void printTTablePV(board_t* b, int depth) {
	int cnt = 0;
	cout << " pv ";

	for (int i = 0; i <= depth; i++) {
		move_t pvMove = probePV(b);

		if (pvMove != NO_MOVE && isLegal(b, pvMove)) {
			cout << getStringMove(pvMove);
			push(b, pvMove);
			cnt++;
		} else {
			// Ttable entry was overwritten or no further entries
			break;
		}
	}

	while (cnt--) pop(b);

}

string getTime() {
	char str[32]{};
	time_t a = time(nullptr);
	struct tm time_info;

	if (localtime_s(&time_info, &a) == 0) strftime(str, sizeof(str), "%H:%M:%S", &time_info);
	return str;
}

bool inputWaiting() {
	static int init = 0, pipe;
	static HANDLE inh;
	DWORD dw;

	if (!init) {
		init = 1;
		inh = GetStdHandle(STD_INPUT_HANDLE);
		pipe = !GetConsoleMode(inh, &dw);
		if (!pipe) {
			SetConsoleMode(inh, dw & ~(ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT));
			FlushConsoleInputBuffer(inh);
		}
	}
	if (pipe) {
		if (!PeekNamedPipe(inh, NULL, 0, NULL, &dw, NULL))
			return 1;
		return dw;
	} else {
		GetNumberOfConsoleInputEvents(inh, &dw);
		return dw <= 1 ? 0 : dw;

	}
}
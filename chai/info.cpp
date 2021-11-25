#include "info.h"

void logDebug(std::string errMsg) {
    std::ofstream ofs("./assertLog.txt", std::ios_base::app);
    ofs << errMsg;
    ofs.close();
}

void printUCI_Info() {
    cout << "id name chai_" << TOSTRING(VERSION) << "\n";
	cout << "id author Jakob Hackstein\n";
	cout << "option name Hash type spin default 256 min 2 max 8192" << endl;
	cout << "option name Threads type spin default " 
		 << NUM_THREADS << " min 1 max " << MAX_THREADS << endl;
	cout << "option name SyzygyPath type string default \"\"" << endl;
	cout << "uciok\n";
}

void printBitBoard(bitboard_t* bb) {
    bitboard_t shiftBit = 1ULL;
    int sq;

    std::cout << std::endl;
    for (int rank = RANK_8; rank >= RANK_1; rank--) {
        for (int file = FILE_A; file <= FILE_H; file++) {
            sq = 8 * rank + file;

            if ((shiftBit << sq) & *bb) {
                std::cout << "1 ";
            }
            else {
                std::cout << ". ";
            }
        }
        std::cout << std::endl;
    }
}

void printMove(board_t* b, const move_t move) {

    if (move == MOVE_NULL) {
        cout << "0000" << endl;
        return;
    }

    int promoted = promPiece(b, move);
    char promChar = ' ';

    if (promoted) {
        promChar = 'q';
        if (promoted == Pieces::n || promoted == Pieces::N) {
            promChar = 'n';
        }
        else if (promoted == Pieces::r || promoted == Pieces::R) {
            promChar = 'r';
        }
        else if (promoted == Pieces::b || promoted == Pieces::B) {
            promChar = 'b';
        }
    }

    std::string ret = "";
    ret += ('a' + squareToFile[fromSq(move)]);
    ret += ('1' + squareToRank[fromSq(move)]);
    ret += ('a' + squareToFile[toSq(move)]);
    ret += ('1' + squareToRank[toSq(move)]);

    cout << ret << promChar << endl;
}

std::string getStringMove(board_t* b, const move_t move) {
    if (move == MOVE_NULL) {
        return "0000";
    }

    int promoted = promPiece(b, move);
    std::string promChar = " ";

    if (promoted) {
        promChar = "q ";
        if (promoted == Pieces::n || promoted == Pieces::N) {
            promChar = "n ";
        }
        else if (promoted == Pieces::r || promoted == Pieces::R) {
            promChar = "r ";
        }
        else if (promoted == Pieces::b || promoted == Pieces::B) {
            promChar = "b ";
        }
    }

    std::string ret = "";
    ret += ('a' + squareToFile[fromSq(move)]);
    ret += ('1' + squareToRank[fromSq(move)]);
    ret += ('a' + squareToFile[toSq(move)]);
    ret += ('1' + squareToRank[toSq(move)]);

    ret += promChar;
    return ret;
}

void printMoveStatus(board_t* b, move_t move) {
    cout << "\n#### - Move Status: " << getStringMove(b, move) << endl;
    printBinary(move);
    cout << "From " << fromSq(move) << " to " << toSq(move) << endl;
    cout << "Pawn start " << isPawnStart(move, pieceAt(b, fromSq(move))) << endl;
    cout << "EP capture " << isEnPassant(move) << endl;
    cout << "Castle move " << isCastling(move) << endl;
    cout << "Promoted piece " << promPiece(b, move) << endl;
    cout << "Captured piece " << capPiece(b, move) << endl;
    cout << "####\n" << endl;
}

void printBinary(bitboard_t x) {
    std::bitset<64> b(x);
    cout << b << endl;
}

void log(std::string logMsg) {
    std::ofstream ofs("log.txt", std::ios_base::out | std::ios_base::app);
    ofs << getTime() << "\t" << logMsg << '\n';
    ofs.close();
}

void readInput(search_t* s) {
    // read input causes tests to wait for cin and does not terminate
#ifndef TESTING
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
#endif
}

void printUCI(search_t* s, int d, int selDpt, int score, long totalNodes) {
    std::string scoreStr = " score ";

    if (abs(score) >= VALUE_IS_MATE_IN) {
        std::string sign = (score > 0) ? "" : "-";
        scoreStr += "mate " + sign + std::to_string(VALUE_MATE - abs(score));
    }
    else {
        scoreStr += "cp " + std::to_string(score);
    }

    cout << "info depth " << d
        << " seldepth " << selDpt
        << scoreStr
        << " nodes " << totalNodes
        << " tbhits " << s->tbHit
        << " time " << (getTimeMs() - s->startTime);
}

void printUCIBestMove(board_t* b, move_t bestMove) {
    Assert(bestMove != MOVE_NONE);

    cout << "bestmove "
        << getStringMove(b, bestMove)
        << endl;
}

void printPV(board_t* b, move_t* moves, int len) {
    cout << " pv ";

    for (int i = 0; i < len; i++) {
        cout << getStringMove(b, moves[i]);
    }
}

void printTTablePV(board_t* b, int depth) {
    int cnt = 0;
    cout << " pv ";

    for (int i = 0; i <= depth; i++) {
        move_t pvMove = probePV(b);

        if (pvMove != MOVE_NONE && isLegal(b, pvMove)) {
            cout << getStringMove(b, pvMove);
            push(b, pvMove);
            cnt++;
        }
        else {
            // Ttable entry was overwritten or no further entries
            break;
        }
    }

    while (cnt--) pop(b);

}

void printPvLine(board_t* b, move_t* pvLine, int d, int score) {
    if (score >= VALUE_IS_MATE_IN)
        d = VALUE_MATE - score;

    cout << " pv ";
    for (int i = 0; i < d; i++) {
        cout << getStringMove(b, pvLine[i]);
    }
}

std::string getTime() {
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
    }
    else {
        GetNumberOfConsoleInputEvents(inh, &dw);
        return dw <= 1 ? 0 : dw;

    }
}
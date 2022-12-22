#include "uci.h"

namespace UCI {


void cli(Board* b, Instructions* i, Stats* s) {

    MoveList moveList;
    Move parsedMove;
    std::string userInput;

    while (1) {
        std::getline(cin, userInput);

        // Start UCI protocol.
        if (userInput == "uci") {
            uciMode(b, s, i);
            break;
        }

        // Exit program.
        if (userInput == "quit") {
            break;
        }

        // Exit program.
        if (userInput == "info") {
            printEngineMeta(info_ASSERT, info_CXX, info_SIMD);
            continue;
        }

        // Start searching this position.
        if (userInput == "s") {
            i->depth = 10;
            i->timeSet = false;

            Search::search(b, s, i);
            continue;
        }

        // Stats-Perft current position
        if (strStartsWith(userInput, "perft")) {
            int perftDepth = stoi(userInput.substr(6));

            if (perftDepth >= 1 && perftDepth <= 15) {
                dividePerft<StatPerft>(b, perftDepth);  
            } else {
                cerr << "Error: bad depth. Enter an integer between in [1, 15]." << endl;
            }

            continue;
        }

        // Stats-Perft current position
        if (strStartsWith(userInput, "fperft")) {
            int perftDepth = stoi(userInput.substr(7));

            if (perftDepth >= 1 && perftDepth <= 15) {
                dividePerft<FastPerft>(b, perftDepth);  
            } else {
                cerr << "Error: bad depth. Enter an integer between in [1, 15]." << endl;
            }

            continue;
        }

        // Generate and print all moves for current position.
        if (userInput == "movegen") {
            generateMoves(b, &moveList, isCheck(b, b->stm));
            printGeneratedMoves(b, &moveList);
            continue;
        }

        if (userInput == "eval") {
            printEval(b);
            continue;
        }

        // Parse fen into board variables.
        if (strStartsWith(userInput, "fen")) {
            std::string fen = userInput.substr(4);

            parseFen(b, fen);
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

        if (NNUE::initNet(nnueData)) {
            cout << "info string NNUE parsed successfully." << endl;
        } else {
            // cerr << "info string Error: NNUE parsed incorrect hash value." << endl;
            cerr << "Warning: Different hash values in NNUE. Network weights possibly corrupted.\n" << endl;
        }

    }

}

void uciParsePosition(Board* b, std::string cmd) {

    if (strStartsWith(cmd, "position fen")) {
        std::string fen = cmd.substr(13, cmd.size());
        int offset = parseFen(b, fen);

        // Possibly further moves after fen position
        // E.g. "position fen r2qkb1r/p2n1ppp/2p1pn2/1p6/2BPP1b1/2N2N2/PP3PPP/R1BQ1RK1 w kq b6 0 9 moves c4b3"
        if ((size_t)offset+1 < fen.size()) {
            std::string moves = fen.substr(offset+1, fen.size());
            if (strStartsWith(moves, "moves")) {
                parseMoveList(b, moves.substr(6, moves.size()));
            }
        }
        return;
    }

    if (strStartsWith(cmd, "position startpos moves")) {
        parseFen(b, STARTING_FEN);
        std::string move;

        parseMoveList(b, cmd.substr(24, cmd.size()));
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
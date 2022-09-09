#include "info.h"


void logDebug(std::string errMsg) {
    std::ofstream ofs("./assertLog.txt", std::ios_base::app);
    ofs << errMsg;
    ofs.close();
}

void printEval(Board* b) {

    cout << "Overall evaluation: "  << evaluation(b) 
         << " (positive value claims advantage for white)" << endl
         << "\tPSQT (opening)\t "  << b->psqtOpening << endl
         << "\tPSQT (endgame)\t "  << b->psqtEndgame << endl;

#ifdef USE_NNUE
    // Use NNUE on balanced positions
    if (canUseNNUE) {
        cout << "\tNNUE\t\t " << NNUE::evaluateNNUE(b) << endl;
    }
#endif // USE_NNUE

}

void printCliHelp() {
    cout << "Error: bad command. Valid commands are:" << endl
         << "\t- uci\t\t\t(start uci protocol)" << endl
         << "\t- <e2e4>\t\t(apply move)" << endl
         << "\t- pop\t\t\t(undo move)" << endl
         << "\t- fen <KQP/34KR/...>\t(parse provided fen)" << endl
#ifdef INFO
         << "\t- print\t\t\t(print board status)" << endl
#endif // INFO
         << "\t- perft <d>\t\t(perft to depth d)" << endl
         << "\t- fperft <d>\t\t(fast-perft to depth d using all available cores)" << endl
         << "\t- info\t\t\t(print engine info)" << endl
         << "\t- quit\t\t\t(exit program)" << endl
         << endl;
}

void printEngineMeta(std::string assert, std::string compiler, std::string simd) {
        cout << "chai " << TOSTRING(VERSION) << endl
             << "assert=" << assert
             << " buckets=" << BUCKETS
             << " threads=" << NUM_THREADS << "/" << getMaxPhysicalCores()
             << " hashMb=" << DEFAULT_TT_SIZE
             << " simd=" << simd << endl
             << "compiler=" << compiler
             << " date=" << __DATE__ << endl << endl;
}

void printUCI_Info() {
    cout << "id name chai_" << TOSTRING(VERSION) << endl
         << "id author Jakob Hackstein" << endl
         << "option name Hash type spin default 256 min 2 max 8192" << endl
         << "option name Threads type spin default " 
         << NUM_THREADS << " min 1 max " << getMaxPhysicalCores() << endl
         << "option name SyzygyPath type string default \"\"" << endl
         << "option name EvalFile type string default \"\"" << endl
         << "uciok" << endl;
}

void printBitBoard(Bitboard* bb) {
    Bitboard shiftBit = 1ULL;
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

void printMove(Board* b, const Move move) {

    if (move == MOVE_NULL) {
        cout << "0000" << endl;
        return;
    }

    int promoted = promPiece(b, move);
    char promChar = ' ';

    if (promoted) {
        promChar = 'q';
        if (promoted == Piece::n || promoted == Piece::N) {
            promChar = 'n';
        }
        else if (promoted == Piece::r || promoted == Piece::R) {
            promChar = 'r';
        }
        else if (promoted == Piece::b || promoted == Piece::B) {
            promChar = 'b';
        }
    }

    std::string ret = "";
    ret += ('a' + toFile(fromSq(move)));
    ret += ('1' + toRank(fromSq(move)));
    ret += ('a' + toFile(toSq(move)));
    ret += ('1' + toRank(toSq(move)));

    cout << ret << promChar << endl;
}

std::string getStringMove(Board* b, const Move move) {
    if (move == MOVE_NULL) {
        return "0000";
    }

    int promoted = promPiece(b, move);
    std::string promChar = " ";

    if (promoted) {
        promChar = "q ";
        if (promoted == Piece::n || promoted == Piece::N) {
            promChar = "n ";
        }
        else if (promoted == Piece::r || promoted == Piece::R) {
            promChar = "r ";
        }
        else if (promoted == Piece::b || promoted == Piece::B) {
            promChar = "b ";
        }
    }

    std::string ret = "";
    ret += ('a' + toFile(fromSq(move)));
    ret += ('1' + toRank(fromSq(move)));
    ret += ('a' + toFile(toSq(move)));
    ret += ('1' + toRank(toSq(move)));

    ret += promChar;
    return ret;
}

void printMoveStatus(Board* b, Move move) {
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

void printBinary(Bitboard x) {
    std::bitset<64> b(x);
    cout << b << endl;
}

void log(std::string logMsg) {
    std::ofstream ofs("log.txt", std::ios_base::out | std::ios_base::app);
    ofs << getTime() << "\t" << logMsg << '\n';
    ofs.close();
}

void printUCI(Instructions* instr, Stats* s, int d, int selDpt, int score, long totalNodes) {
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
        << " time " << (getTimeMs() - instr->startTime);
}

void printUCIBestMove(Board* b, Move bestMove) {
    Assert(bestMove != MOVE_NONE);

    cout << "bestmove "
        << getStringMove(b, bestMove)
        << endl;
}

void printPV(Board* b, Move* moves, int len) {
    cout << " pv ";

    for (int i = 0; i < len; i++) {
        cout << getStringMove(b, moves[i]);
    }
}

void printTTablePV(Board* b, int depth) {
    int cnt = 0;
    cout << " pv ";

    for (int i = 0; i <= depth; i++) {
        Move pvMove = TT::probePV(b);

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

void printPvLine(Board* b, Move* pvLine, int d, int score) {
    if (score >= VALUE_IS_MATE_IN)
        d = VALUE_MATE - score;

    cout << " pv ";
    int i = 0;

    while (i < d && pvLine[i] != MOVE_NONE) {
        cout << getStringMove(b, pvLine[i++]);
    }
}

std::string getTime() {
    char str[32]{};

#ifdef _WIN32
    time_t a = time(nullptr);
    struct tm time_info;
    
    if (localtime_s(&time_info, &a) == 0) {
        strftime(str, sizeof(str), "%H:%M:%S", &time_info);
    }

#else
    time_t     now = time(0);
    struct tm  tstruct;
    tstruct = *localtime(&now);

    strftime(str, sizeof(str), "%H:%M:%S", &tstruct);

#endif // _WIN32


    return str;
}

std::string getTimeAndDate() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}

bool parseFen(Board* board, std::string fen) {

    reset(board);

    // Shortest fen (2 kings, no rights) "8/8/8/k7/K7/8/8/8 w - - 0 1"
    if (fen.length() < 27) {
        return true;
    }

    int file = FILE_A, rank = RANK_8;
    int index = 0, square = 0, piece = 0, count = 0;

    while (rank >= RANK_1) {
        count = 1;
        switch (fen[index]) {
            case 'p': piece = Piece::p; break;
            case 'r': piece = Piece::r; break;
            case 'n': piece = Piece::n; break;
            case 'b': piece = Piece::b; break;
            case 'k': piece = Piece::k; break;
            case 'q': piece = Piece::q; break;
            case 'P': piece = Piece::P; break;
            case 'R': piece = Piece::R; break;
            case 'N': piece = Piece::N; break;
            case 'B': piece = Piece::B; break;
            case 'K': piece = Piece::K; break;
            case 'Q': piece = Piece::Q; break;

            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
                piece = NO_PTYPE;
                count = fen[index] - '0';
                break;

            case '/':
            case ' ':
                rank--;
                file = FILE_A;
                index++;
                continue;

            default:
                cout << "FEN error: " << fen[index] << endl;
                return true;
        }

        for (int i = 0; i < count; i++) {
            square = rank * 8 + file;

            if (piece != NO_PTYPE) {
                setPiece(board, piece, square, pieceCol[piece]);
            }

            file++;
        }
        index++;
    }

    // assert for correct position
    Assert(fen[index] == 'w' || fen[index] == 'b');
    board->stm = (fen[index] == 'w') ? WHITE 
                                     : BLACK;
    index += 2;

    // castle permission
    for (int i = 0; i < 4; i++) {
        if (fen[index] == ' ') {
            break;
        }
        switch (fen[index]) {
            case 'K': board->castlePermission |= K_CASTLE; break;
            case 'Q': board->castlePermission |= Q_CASTLE; break;
            case 'k': board->castlePermission |= k_CASTLE; break;
            case 'q': board->castlePermission |= q_CASTLE; break;
            default: break;
        }
        index++;
    }
    index++;
    Assert(board->castlePermission >= 0 && board->castlePermission <= 15);

    // en passant square
    if (fen[index] != '-') {
        file = fen[index] - 'a';
        rank = fen[index + 1] - '1';

        Assert(file >= FILE_A && file <= FILE_H);
        Assert(rank >= RANK_1 && rank <= RANK_8);

        board->enPas = fileRankToSq(file, rank);
        Assert(validEnPasSq(board->enPas));
        index += 3;
    } else {
        index += 2;
    }

    board->fiftyMove += atoi(&fen[index]);

    index += 2;

    std::string fullMoveStr = "";
    while (fen[index]) {
        fullMoveStr += fen[index];
        index++;
    }

    board->halfMoves += std::stoi(fullMoveStr) * 2;

    board->zobristPawnKey = generatePawnHashKey(board);
    board->zobristKey     = generateZobristKey(board);

    board->psqtOpening = calcPSQT(board, PSQT_OPENING);
    board->psqtEndgame = calcPSQT(board, PSQT_ENDGAME);
    board->material    = materialScore(board);

    checkBoard(board);
    return false;
}

std::string getFEN(Board* b) {

    int piece;
    int empty = 0;
    std::string fen = "";

    int i = 0;
    int r = RANK_8;
    int f = FILE_A;

    while (r >= RANK_1) {

        f = FILE_A;
        while (f <= FILE_H) {
            i = fileRankToSq(f, r);

            piece = pieceAt(b, i);
            if (pieceValid(piece)) {
                if (empty) {
                    fen += std::to_string(empty);
                    empty = 0;
                }
                fen += pieceChar[piece];
            } else {
                empty++;
            }

            f++;
        }

        if (empty) {
            fen += std::to_string(empty);
        }

        empty = 0;

        if (r != RANK_1) {
            fen += "/";
        }

        r--;
    }

    if (b->stm == WHITE) {
        fen += " w ";
    } else {
        fen += " b ";
    }

    if (b->castlePermission & K_CASTLE) fen += "K";
    if (b->castlePermission & Q_CASTLE) fen += "Q";
    if (b->castlePermission & k_CASTLE) fen += "k";
    if (b->castlePermission & q_CASTLE) fen += "q";
    fen += " ";

    if (b->enPas != DEFAULT_EP_SQ) {
        fen += ('a' + toFile(b->enPas));
        fen += ('1' + toRank(b->enPas));
    } else {
        fen += "-";
    }

    fen += " " + std::to_string(b->undoPly) + " " + std::to_string(b->halfMoves);

    return fen;
}

Move parseMove(Board* b, std::string move) {

    if (move == "0000")
        return MOVE_NULL;

    int from = fileRankToSq(move[0] - 97, move[1] - 49);
    int to   = fileRankToSq(move[2] - 97, move[3] - 49);

    int movingPiece = pieceAt(b, from);
    int promPiece   = 0;
    int MOVE_FLAG   = NORMAL_MOVE;

    // Set possible pawn flags
    if (piecePawn[movingPiece]) {

        // Set ep flag if to square is en passant
        if (   to == b->enPas
            && b->enPas != DEFAULT_EP_SQ ) {

            Assert(abs(from - to) == 7 || abs(from - to) == 9);
            MOVE_FLAG = EP_MOVE;

        } else if (   toRank(to) == RANK_1 
                   || toRank(to) == RANK_8) {

            MOVE_FLAG = PROM_MOVE;

            switch (move[4]) {
                case 'n': promPiece = PROM_TO_KNIGHT; break;
                case 'b': promPiece = PROM_TO_BISHOP; break;
                case 'r': promPiece = PROM_TO_ROOK; break;
                default:  promPiece = PROM_TO_QUEEN; break;
            }
        }
    }

    if (   abs(from - to) == 2
        && pieceKing[movingPiece])
        MOVE_FLAG = CASTLE_MOVE;
 
    return serializeMove(from, to, MOVE_FLAG, promPiece);
}

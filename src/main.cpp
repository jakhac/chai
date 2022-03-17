#include "uci.h"
#include "search.h"


Board board;
Perft perft;
Stats stats;
Instructions instr;

int main() {

    printEngineMeta(info_ASSERT, info_CXX, info_SIMD);

    MoveOrder::init();
    Mask::init();
    EGTB::init("");
    TT::init();
    Search::init();
    Threads::initPool();
    NNUE::initIncNet();

    parseFen(&board, STARTING_FEN);
    printBoard(&board);

    UCI::cli(&board, &instr, &stats, &perft);

    EGTB::freeEGTB();
    TT::freeHashTables();
    Threads::deletePool();

    return EXIT_SUCCESS;
}

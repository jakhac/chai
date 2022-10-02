#include "uci.h"
#include "search.h"

Board board;
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
    UCI::cli(&board, &instr, &stats);

    EGTB::freeEGTB();
    TT::freeHashTables();
    Threads::deletePool();

    return EXIT_SUCCESS;
}

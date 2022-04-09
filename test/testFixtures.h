
#include "../src/uci.h"
#include "../src/perft.h"
#include "../src/bitboard.h"

#include "gtest/gtest.h"

namespace SearchTest {
    class SearchTest : public ::testing::Test {

    protected:
        Board* pBoard;
        Stats* pSearch;
        Instructions* pInstr;
    
    public:
        virtual void SetUp() {
            pBoard = new Board();
            pSearch = new Stats();
            pInstr = new Instructions();
            
            MoveOrder::init();
            Mask::init();
            Search::init();
            TT::init();
            Threads::initPool();
        }

        virtual void TearDown() {
            TT::freeHashTables();
            Threads::deletePool();
            delete pBoard;
            delete pSearch;
            delete pInstr;
        }

    };
}

namespace PerftTest {
    class PerftTest : public ::testing::Test {

    protected:
        Board* pBoard;
        FastPerft perft;

        virtual void SetUp() {
            pBoard = new Board();

            MoveOrder::init();
            Mask::init();
            Search::init();
        }

        virtual void TearDown() {
            delete pBoard;
        }

    };
}

namespace SeeTest {
    class SeeTest : public ::testing::Test {

    protected:
        Board* pBoard;

        virtual void SetUp() {
            pBoard = new Board();

            MoveOrder::init();
            Mask::init();
            Search::init();
        }

        virtual void TearDown() {
            delete pBoard;
        }

    };
}

namespace EvalTest {
    class EvalTest : public ::testing::Test {

    protected:
        Board* pBoard;

        virtual void SetUp() {
            pBoard = new Board();

            MoveOrder::init();
            Mask::init();
            Search::init();
        }

        virtual void TearDown() {
            delete pBoard;
        }

    };
}

namespace PositionalTest {
    class PositionalTest : public ::testing::Test {

    protected:
        Board* pBoard;

        virtual void SetUp() {
            pBoard = new Board();

            MoveOrder::init();
            Mask::init();
            Search::init();
        }

        virtual void TearDown() {
            delete pBoard;
        }

    };
}

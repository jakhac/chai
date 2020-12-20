#include "pch.h"
#include "uci.h"

namespace {
    class SearchTest : public ::testing::Test {

    protected:
        Board* pBoard;
        search_t* pSearch;

        virtual void SetUp() {
            pBoard = new Board();
            pSearch = new search_t();

            init(pBoard);
        }

        virtual void TearDown() {
            delete pBoard;
            delete pSearch;
        }

    };

    // Mating positions
    TEST_F(SearchTest, MateIn6) {
        pBoard->parseFen("2rnqb2/1b1n1Nk1/p2PB1Pp/1p6/4Q3/8/PP4PP/R4RK1 w - - 1 0");
        pSearch->depth = 6;
        pSearch->depthSet = true;
        pSearch->timeSet = false;

        EXPECT_GE(search(pBoard, pSearch), 28500);
    }

    // Mating positions
    TEST_F(SearchTest, MateIn4_1) {
        pBoard->parseFen("r7/3bb1kp/q4p1N/1pnPp1np/2p4Q/2P5/1PB3P1/2B2RK1 w - - 1 0");
        pSearch->depth = 4;
        pSearch->depthSet = true;
        pSearch->timeSet = false;

        EXPECT_GE(search(pBoard, pSearch), 28500);
    }

    // Mating positions
    TEST_F(SearchTest, MateIn4_2) {
        pBoard->parseFen("rnb1Qb1r/pq3Bpp/4p3/B2n4/3P2kP/4P3/1P3PP1/R3K2R w KQ - 1 0");
        pSearch->depth = 4;
        pSearch->depthSet = true;
        pSearch->timeSet = false;

        EXPECT_GE(search(pBoard, pSearch), 28500);
    }

    // Mating positions
    TEST_F(SearchTest, MateIn7) {
        pBoard->parseFen("7k/2rR3p/1Pp1qp1B/p1p3p1/P3P3/7P/5PP1/6K1 w - - 1 0");
        pSearch->depth = 7;
        pSearch->depthSet = true;
        pSearch->timeSet = false;

        EXPECT_GE(search(pBoard, pSearch), 28500);
    }


}
#include "testFixtures.h"

using namespace SearchTest;
using namespace Search;


#if defined(USE_NNUE)
TEST_F(SearchTest, NNUE) {
    if (canUseNNUE) {
        parseFen(pBoard, STARTING_FEN);
        ASSERT_GE(evaluation(pBoard), 0);
        cout << "NNUE is used for evaluation." << endl;
    } else {
        cout << "HCE is used for evaluation." << endl;
    }
}
#endif

TEST_F(SearchTest, MateIn5_1) {
    parseFen(pBoard, "r7/3bb1kp/q4p1N/1pnPp1np/2p4Q/2P5/1PB3P1/2B2RK1 w - - 1 40");
    pInstr->depth = 5;
    pInstr->depthSet = true;
    pInstr->timeSet = false;

    evaluation(pBoard);

    EXPECT_GE(search(pBoard, pSearch, pInstr), VALUE_IS_MATE_IN);
}

TEST_F(SearchTest, MateIn5_2) {
    parseFen(pBoard, "rnb1Qb1r/pq3Bpp/4p3/B2n4/3P2kP/4P3/1P3PP1/R3K2R w KQ - 1 40");
    pInstr->depth = 5;
    pInstr->depthSet = true;
    pInstr->timeSet = false;

    EXPECT_GE(search(pBoard, pSearch, pInstr), VALUE_IS_MATE_IN);
}

TEST_F(SearchTest, MateIn7_1) {
    parseFen(pBoard, "7k/2rR3p/1Pp1qp1B/p1p3p1/P3P3/7P/5PP1/6K1 w - - 1 40");
    pInstr->depth = 10;
    pInstr->depthSet = true;
    pInstr->timeSet = false;

    EXPECT_GE(search(pBoard, pSearch, pInstr), VALUE_IS_MATE_IN);
}

TEST_F(SearchTest, MateIn7) {
    parseFen(pBoard, "2rnqb2/1b1n1Nk1/p2PB1Pp/1p6/4Q3/8/PP4PP/R4RK1 w - - 1 40");
    pInstr->depth = 7;
    pInstr->depthSet = true;
    pInstr->timeSet = false;

    EXPECT_GE(search(pBoard, pSearch, pInstr), VALUE_IS_MATE_IN);
}

TEST_F(SearchTest, Fisher_4) {
    parseFen(pBoard, "5r2/2Q4p/3P2p1/1k3p2/qp5P/2r2P2/2P3P1/1K1RR3 w - - 1 40");
    pInstr->depth = 4;
    pInstr->depthSet = true;
    pInstr->timeSet = false;

    EXPECT_GE(search(pBoard, pSearch, pInstr), VALUE_IS_MATE_IN);
}

TEST_F(SearchTest, Fisher_3) {
    parseFen(pBoard, "3r1k2/1q1P4/5b2/p3p2p/1p6/1B3P2/PPPQ4/1K1R4 w - - 1 40");
    pInstr->depth = 6;
    pInstr->depthSet = true;
    pInstr->timeSet = false;

    EXPECT_GE(search(pBoard, pSearch, pInstr), VALUE_IS_MATE_IN);
}

TEST_F(SearchTest, Fisher_5) {
    parseFen(pBoard, "1r4r1/q2b1p1k/p4P1p/1pn1P1p1/3N4/5B2/PPP2R1Q/4R1K1 w - - 1 40");
    pInstr->depth = 9;
    pInstr->depthSet = true;
    pInstr->timeSet = false;

    EXPECT_GE(search(pBoard, pSearch, pInstr), VALUE_IS_MATE_IN);
}

TEST_F(SearchTest, Endgame_KQvKP_1) {
    parseFen(pBoard, "8/2Q5/K7/8/8/8/4kp2/8 w - - 0 1");
    pInstr->depth = 10;
    pInstr->depthSet = true;
    pInstr->timeSet = false;

    EXPECT_LE(abs(search(pBoard, pSearch, pInstr)), 100);
}

TEST_F(SearchTest, Endgame_KQvKP_2) {
    parseFen(pBoard, "8/2Q5/K7/8/8/8/4k1p1/8 w - - 0 1");
    pInstr->depth = 10;
    pInstr->depthSet = true;
    pInstr->timeSet = false;

    EXPECT_GE(abs(search(pBoard, pSearch, pInstr)), 100);
}

TEST_F(SearchTest, Endgame_KQvKP_3) {
    parseFen(pBoard, "8/2Q5/K7/7k/8/5p2/8/8 w - - 0 1");
    pInstr->depth = 10;
    pInstr->depthSet = true;
    pInstr->timeSet = false;

    EXPECT_GE(abs(search(pBoard, pSearch, pInstr)), 100);
}

TEST_F(SearchTest, Endgame_KQvKP_4) {
    parseFen(pBoard, "8/8/K7/8/8/8/2Q1pk2/8 w - - 0 1");
    pInstr->depth = 10;
    pInstr->depthSet = true;
    pInstr->timeSet = false;

    EXPECT_GE(abs(search(pBoard, pSearch, pInstr)), 100);
}

TEST_F(SearchTest, Endgame_KQvKP_5) {
    parseFen(pBoard, "8/8/K7/8/8/8/1Qp1k3/8 b - - 0 1");
    pInstr->depth = 10;
    pInstr->depthSet = true;
    pInstr->timeSet = false;

    EXPECT_LE(abs(search(pBoard, pSearch, pInstr)), 100);
}

TEST_F(SearchTest, Endgame_KQvKP_6) {
    parseFen(pBoard, "8/2k2K1P/8/8/8/8/1q6/8 b - - 0 1");
    pInstr->depth = 15;
    pInstr->depthSet = true;
    pInstr->timeSet = false;

    EXPECT_GE(abs(search(pBoard, pSearch, pInstr)), 100);
}



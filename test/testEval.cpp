#include "testFixtures.h"

using namespace EvalTest;

namespace chai {

TEST_F(EvalTest, Backward_Pawns) {
    bitboard_t sol, ans;

	parseFen(pBoard, "4k3/2p3p1/1p1pp3/8/6p1/pP2Pp2/P6P/4K3 w - - 0 1");
    ans = wBackwardPawns(pBoard);
    sol = (1ULL << H2);
	EXPECT_EQ(ans, sol);

    parseFen(pBoard, "4k3/2p3pp/1p1p4/8/6p1/pP2Pp2/P6P/4K3 w - - 0 1");
    ans = wBackwardPawns(pBoard);
    sol = 0ULL;
	EXPECT_EQ(ans, sol);
    
    parseFen(pBoard, "4k3/2p3pp/3pp3/8/1p4p1/1P2Pp2/P6P/4K3 w - - 0 1");
    ans = wBackwardPawns(pBoard);
    sol = (1ULL << A2);
	EXPECT_EQ(ans, sol);

    parseFen(pBoard, "4k3/2p3pp/3pp3/8/1p4p1/4Pp2/PP5P/4K3 w - - 0 1");
    ans = wBackwardPawns(pBoard);
    sol = 0ULL;
	EXPECT_EQ(ans, sol);
}

TEST_F(EvalTest, PawnIslands) {
    int sol, ans;

	parseFen(pBoard, "4k3/2p3pp/3pp3/8/1p4p1/4Pp2/PP5P/4K3 w - - 0 1");
    ans = pawnIslands(pBoard, chai::WHITE);
    sol = 3;
	EXPECT_EQ(ans, sol);

	parseFen(pBoard, "4k3/2p3pp/3pp1P1/8/1p4p1/4Pp2/1PPP3P/4K3 w - - 0 1");
    ans = pawnIslands(pBoard, chai::WHITE);
    sol = 2;
	EXPECT_EQ(ans, sol);

	parseFen(pBoard, "4k3/2p3pp/3pp1P1/8/1p4p1/5pP1/1P1P3P/4K3 w - - 0 1");
    ans = pawnIslands(pBoard, chai::WHITE);
    sol = 3;
	EXPECT_EQ(ans, sol);

	parseFen(pBoard, "4k3/2p3pp/3pp1P1/8/1p4p1/5pP1/PPPPPP1P/4K3 w - - 0 1");
    ans = pawnIslands(pBoard, chai::WHITE);
    sol = 1;
	EXPECT_EQ(ans, sol);
}

TEST_F(EvalTest, PassedPawns) {
    int ans, sol;

    parseFen(pBoard, "4k3/pppppppp/8/8/4P3/5P2/P5PP/4K3 w - - 0 1");
    ans = passedPawns(pBoard, chai::WHITE);
    sol = 0;
	EXPECT_EQ(ans, sol);
    ans = passedPawns(pBoard, chai::BLACK);
    sol = 5;
	EXPECT_EQ(ans, sol);

    parseFen(pBoard, "4k3/pp3ppp/8/3P4/4P3/4pP2/PP1p2PP/4K3 w - - 0 1");
    ans = passedPawns(pBoard, chai::WHITE);
    sol = 35;
	EXPECT_EQ(ans, sol);
    ans = passedPawns(pBoard, chai::BLACK);
    sol = 185;
	EXPECT_EQ(ans, sol);
}

TEST_F(EvalTest, HiddenPassedPawns) {
    int ans, sol;

    parseFen(pBoard, "4k3/8/p7/P7/1P6/8/8/4K3 w - - 0 1");
    ans = hiddenPassedPawns(pBoard, chai::WHITE);
    sol = 1;
	EXPECT_EQ(ans, sol);

    parseFen(pBoard, "4k3/1p6/p7/P7/1P6/8/8/4K3 w - - 0 1");
    ans = hiddenPassedPawns(pBoard, chai::WHITE);
    sol = 0;
	EXPECT_EQ(ans, sol);

    parseFen(pBoard, "4k3/1p6/p7/P7/1P6/8/8/4K3 w - - 0 1");
    ans = hiddenPassedPawns(pBoard, chai::WHITE);
    sol = 0;
	EXPECT_EQ(ans, sol);

    parseFen(pBoard, "4k3/4pppp/p2p4/P7/1P6/8/8/4K3 w - - 0 1");
    ans = hiddenPassedPawns(pBoard, chai::WHITE);
    sol = 1;
	EXPECT_EQ(ans, sol);

    parseFen(pBoard, "4k3/4p3/p7/P3P3/1P1P1P2/8/8/4K3 w - - 0 1");
    ans = hiddenPassedPawns(pBoard, chai::WHITE);
    sol = 2;
	EXPECT_EQ(ans, sol);

    parseFen(pBoard, "4k3/2p1p1p1/p6p/P3P3/1P1P1P2/8/8/4K3 w - - 0 1");
    ans = hiddenPassedPawns(pBoard, chai::WHITE);
    sol = 0;
	EXPECT_EQ(ans, sol);
}

TEST_F(EvalTest, PawnStructure) {
    value_t pawnEval;

	parseFen(pBoard, "4k3/7p/4p3/8/1p4p1/4Pp2/PP5P/4K3 w - - 0 1");
    pawnEval = evaluatePawns(pBoard);
	EXPECT_TRUE(pawnEval < 0);

	parseFen(pBoard, "4k3/p6p/P3p3/1P6/8/4Pp2/5P1P/4K3 w - - 0 1");
    pawnEval = evaluatePawns(pBoard);
	EXPECT_TRUE(pawnEval > 0);

	parseFen(pBoard, "4k3/p1p1pp2/p1p3p1/8/3P4/4P3/PP3PPP/4K3 w - - 0 1");
    pawnEval = evaluatePawns(pBoard);
	EXPECT_TRUE(pawnEval > 0);
}

TEST_F(EvalTest, KingPawnEval) {
    int count;

	parseFen(pBoard, "3k4/2p1p3/8/8/8/8/5PPP/6K1 w - - 0 1");
    count = openFilesNearKing(pBoard, G1, WHITE);
	EXPECT_EQ(count, 0);
    count = openFilesNearKing(pBoard, D8, BLACK);
	EXPECT_EQ(count, 1);

	parseFen(pBoard, "4k3/2p1p3/8/8/8/8/5P2/6K1 w - - 0 1");
    count = openFilesNearKing(pBoard, G1, WHITE);
	EXPECT_EQ(count, 2);
    count = openFilesNearKing(pBoard, E8, BLACK);
	EXPECT_EQ(count, 2);
}

} // chai

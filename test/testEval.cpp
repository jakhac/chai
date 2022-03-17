#include "testFixtures.h"

using namespace EvalTest;


TEST_F(EvalTest, Backward_Pawns) {
    Bitboard sol, ans;

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
    ans = pawnIslands<WHITE>(pBoard);
    sol = 3;
    EXPECT_EQ(ans, sol);

    parseFen(pBoard, "4k3/2p3pp/3pp1P1/8/1p4p1/4Pp2/1PPP3P/4K3 w - - 0 1");
    ans = pawnIslands<WHITE>(pBoard);
    sol = 2;
    EXPECT_EQ(ans, sol);

    parseFen(pBoard, "4k3/2p3pp/3pp1P1/8/1p4p1/5pP1/1P1P3P/4K3 w - - 0 1");
    ans = pawnIslands<WHITE>(pBoard);
    sol = 3;
    EXPECT_EQ(ans, sol);

    parseFen(pBoard, "4k3/2p3pp/3pp1P1/8/1p4p1/5pP1/PPPPPP1P/4K3 w - - 0 1");
    ans = pawnIslands<WHITE>(pBoard);
    sol = 1;
    EXPECT_EQ(ans, sol);
}

TEST_F(EvalTest, PassedPawns) {
    int ans, sol;

    parseFen(pBoard, "4k3/pppppppp/8/8/4P3/5P2/P5PP/4K3 w - - 0 1");
    ans = passedPawns<WHITE>(pBoard);
    sol = 0;
    EXPECT_EQ(ans, sol);
    ans = passedPawns<BLACK>(pBoard);
    sol = 5;
    EXPECT_EQ(ans, sol);

    parseFen(pBoard, "4k3/pp3ppp/8/3P4/4P3/4pP2/PP1p2PP/4K3 w - - 0 1");
    ans = passedPawns<WHITE>(pBoard);
    sol = 35;
    EXPECT_EQ(ans, sol);
    ans = passedPawns<BLACK>(pBoard);
    sol = 185;
    EXPECT_EQ(ans, sol);
}

TEST_F(EvalTest, HiddenPassedPawns) {
    int ans, sol;

    parseFen(pBoard, "4k3/8/p7/P7/1P6/8/8/4K3 w - - 0 1");
    ans = hiddenPassedPawns<WHITE>(pBoard);
    sol = 1;
    EXPECT_EQ(ans, sol);

    parseFen(pBoard, "4k3/1p6/p7/P7/1P6/8/8/4K3 w - - 0 1");
    ans = hiddenPassedPawns<WHITE>(pBoard);
    sol = 0;
    EXPECT_EQ(ans, sol);

    parseFen(pBoard, "4k3/1p6/p7/P7/1P6/8/8/4K3 w - - 0 1");
    ans = hiddenPassedPawns<WHITE>(pBoard);
    sol = 0;
    EXPECT_EQ(ans, sol);

    parseFen(pBoard, "4k3/4pppp/p2p4/P7/1P6/8/8/4K3 w - - 0 1");
    ans = hiddenPassedPawns<WHITE>(pBoard);
    sol = 1;
    EXPECT_EQ(ans, sol);

    parseFen(pBoard, "4k3/4p3/p7/P3P3/1P1P1P2/8/8/4K3 w - - 0 1");
    ans = hiddenPassedPawns<WHITE>(pBoard);
    sol = 2;
    EXPECT_EQ(ans, sol);

    parseFen(pBoard, "4k3/2p1p1p1/p6p/P3P3/1P1P1P2/8/8/4K3 w - - 0 1");
    ans = hiddenPassedPawns<WHITE>(pBoard);
    sol = 0;
    EXPECT_EQ(ans, sol);
}

TEST_F(EvalTest, PawnStructure) {
    Value pawnEval;

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

TEST_F(EvalTest, Passers) {
    int count;

    parseFen(pBoard, "4k3/2p3pp/3PpP2/8/1Pp5/Pp6/3P4/4K3 w - - 0 1");
    count = popCount(getPassers(pBoard, WHITE));
    EXPECT_EQ(count, 1);
    count = popCount(getPassers(pBoard, BLACK));
    EXPECT_EQ(count, 2);

    bool flag;
    parseFen(pBoard, "1r2k3/2p2Ppp/3Pp3/8/1Pp5/Pp6/3P4/R3K1R1 w - - 0 1");
    Bitboard wRooks = getPieces(pBoard, ROOK, WHITE);
    Bitboard bRooks = getPieces(pBoard, ROOK, BLACK);
    Bitboard wPasser = getPassers(pBoard, WHITE);
    Bitboard bPasser = getPassers(pBoard, BLACK);

    flag = rookControlsPasser(wRooks, wPasser, false);
    EXPECT_EQ(flag, true);

    flag = rookControlsPasser(wRooks, bPasser, false);
    EXPECT_EQ(flag, true);

    flag = rookControlsPasser(bRooks, wPasser, true);
    EXPECT_EQ(flag, false);

    flag = rookControlsPasser(bRooks, bPasser, true);
    EXPECT_EQ(flag, true);

    EXPECT_EQ(rooksConnected(pBoard, A1, G1), false);
    parseFen(pBoard, "1r2k3/2p2Ppp/3Pp3/1r6/1Pp5/Pp6/3PK3/R5R1 w - - 0 1");
    EXPECT_EQ(rooksConnected(pBoard, A1, G1), true);
    EXPECT_EQ(rooksConnected(pBoard, B8, B5), true);
}

TEST_F(EvalTest, DiscoveredAttacks) {
    Bitboard res;

    // DiscoveredAttacks
    parseFen(pBoard, "3rk2b/8/3ppp2/8/5Q2/8/1PPP4/4K3 w - - 0 1");
    res = 0ULL;
    EXPECT_EQ(res, getDiscoveredAttacks(pBoard, F4, WHITE));

    parseFen(pBoard, "1b2kr1b/8/3ppp2/8/5Q2/8/1PPP4/4K3 w - - 0 1");
    res = (1ULL << B8) | (1ULL << F8);
    EXPECT_EQ(res, getDiscoveredAttacks(pBoard, F4, WHITE));

    parseFen(pBoard, "1b2kr1b/5n2/3ppp2/8/5Q2/8/1PPP4/4K3 w - - 0 1");
    res = (1ULL << B8);
    EXPECT_EQ(res, getDiscoveredAttacks(pBoard, F4, WHITE));

    // Knight checks
    parseFen(pBoard, "4k3/5ppp/4p3/1N1P4/1nP4n/8/PP3PP1/4K3 w - - 0 1");
    pBoard->attackedSquares[WHITE] = attackerSet(pBoard, WHITE);
    pBoard->attackedSquares[BLACK] = attackerSet(pBoard, BLACK);
    EXPECT_EQ(3, undefendedChecksFrom<KNIGHT>(pBoard, E1, BLACK));

    parseFen(pBoard, "4k3/5ppp/4p3/1N1P4/1nP4n/8/PP1Q1PP1/4K3 w - - 0 1");
    pBoard->attackedSquares[WHITE] = attackerSet(pBoard, WHITE);
    pBoard->attackedSquares[BLACK] = attackerSet(pBoard, BLACK);
    EXPECT_EQ(1, undefendedChecksFrom<KNIGHT>(pBoard, E1, BLACK));

    // Bishop checks
    parseFen(pBoard, "4k3/5ppp/3bp3/1N1P4/1nP4n/5P2/PP1Q2P1/4K3 w - - 0 1");
    pBoard->attackedSquares[WHITE] = attackerSet(pBoard, WHITE);
    pBoard->attackedSquares[BLACK] = attackerSet(pBoard, BLACK);
    EXPECT_EQ(1, undefendedChecksFrom<BISHOP>(pBoard, E1, BLACK));

    parseFen(pBoard, "4k3/5ppp/3bp3/1N1P4/1nP4n/5P2/PP1Q2P1/4KN2 w - - 0 1");
    pBoard->attackedSquares[WHITE] = attackerSet(pBoard, WHITE);
    pBoard->attackedSquares[BLACK] = attackerSet(pBoard, BLACK);
    EXPECT_EQ(0, undefendedChecksFrom<BISHOP>(pBoard, E1, BLACK));

    // Rook checks
    parseFen(pBoard, "r3k3/5ppp/3bp3/1N1P4/1nP4n/5P2/1P1Q2P1/4KN2 w - - 0 1");
    pBoard->attackedSquares[WHITE] = attackerSet(pBoard, WHITE);
    pBoard->attackedSquares[BLACK] = attackerSet(pBoard, BLACK);
    EXPECT_EQ(1, undefendedChecksFrom<ROOK>(pBoard, E1, BLACK));

    parseFen(pBoard, "r3k3/5ppp/3bp3/1N1P4/1nP4n/2Q2P2/6P1/4KN2 w - - 0 1");
    pBoard->attackedSquares[WHITE] = attackerSet(pBoard, WHITE);
    pBoard->attackedSquares[BLACK] = attackerSet(pBoard, BLACK);
    EXPECT_EQ(0, undefendedChecksFrom<ROOK>(pBoard, E1, BLACK));
    
}


#include "testFixtures.h"

namespace PositionalTest {

TEST_F(PositionalTest, CheckingMoves) {
    Move move;


    // Checking moves
    parseFen(pBoard, "r2q1rk1/pb1nppbp/1p1p1np1/1Bp3B1/3PP1N1/1NP5/PP2QPPP/R3K2R w KQ - 4 10");
    move = serializeMove(G4, F6, NORMAL_MOVE, Piece::NO_PIECE);
    EXPECT_TRUE(checkingMove(pBoard, move));

    parseFen(pBoard, "r2q1rk1/pb1nppbp/1p1p1np1/1Bp3B1/3PP1N1/1NP4Q/PP3PPP/R3K2R w KQ - 4 10");
    move = serializeMove(H3, H7, NORMAL_MOVE, Piece::NO_PIECE);
    EXPECT_TRUE(checkingMove(pBoard, move));

    parseFen(pBoard, "r2q1r1k/pb1npp1p/1ppp1Npb/1B5n/3QP3/1NP5/PP3PPP/R3K2R w KQ - 4 10");
    move = serializeMove(F6, G4, NORMAL_MOVE, Piece::NO_PIECE);
    EXPECT_TRUE(checkingMove(pBoard, move));

    parseFen(pBoard, "8/5k2/8/2pP4/8/Q7/8/R3K2R w KQ - 0 1"); 
    move = serializeMove(E1, G1, CASTLE_MOVE, Piece::NO_PIECE);
    EXPECT_TRUE(checkingMove(pBoard, move));

    parseFen(pBoard, "5k2/8/8/2pP4/8/Q7/8/4K3 w - c6 0 1"); 
    move = serializeMove(D5, C6, EP_MOVE, Piece::NO_PIECE);
    EXPECT_TRUE(checkingMove(pBoard, move));


    // Non-checking moves
    parseFen(pBoard, "8/5k2/8/2pP4/8/Q7/8/4K3 w - c6 0 1");
    move = serializeMove(D5, C6, EP_MOVE, Piece::NO_PIECE);
    EXPECT_FALSE(checkingMove(pBoard, move));

    parseFen(pBoard, "8/5k2/8/2pP4/8/Q7/8/4K3 w - c6 0 1");
    move = serializeMove(E1, D1, NORMAL_MOVE, Piece::NO_PIECE);
    EXPECT_FALSE(checkingMove(pBoard, move));

}

TEST_F(PositionalTest, DiscoveredAttacks) {
    Bitboard discoveredAtks = 0ULL;


    // Discovered attacks
    parseFen(pBoard, "r2q1r1k/pb1npp1p/1ppp1Npb/7n/3BP3/2P5/NP3PPP/R3K2R w KQ - 4 10");
    discoveredAtks = getDiscoveredAttacks(pBoard, H8, BLACK);
    EXPECT_TRUE(discoveredAtks & (1ULL << D4));

    parseFen(pBoard, "krbq1r2/3npp1p/1ppp1Npb/7n/3BP3/2P5/NP3PPP/R3K2R w KQ - 4 10");
    discoveredAtks = getDiscoveredAttacks(pBoard, A8, BLACK);
    EXPECT_TRUE(discoveredAtks & (1ULL << A1));

    parseFen(pBoard, "8/8/R1P1k3/8/8/8/8/4K3 w - - 0 1");
    discoveredAtks = getDiscoveredAttacks(pBoard, E6, BLACK);
    EXPECT_TRUE(discoveredAtks & (1ULL << A6));


    // No discovered attacks possible
    parseFen(pBoard, "r2q1rk1/pb1nppbp/1ppp2p1/1B5n/4P1N1/1NP1B3/PP3PPP/R2QK2R w KQ - 4 10");
    discoveredAtks = getDiscoveredAttacks(pBoard, E6, BLACK);
    EXPECT_FALSE(discoveredAtks);
    discoveredAtks = getDiscoveredAttacks(pBoard, A1, BLACK);
    EXPECT_FALSE(discoveredAtks);
    discoveredAtks = getDiscoveredAttacks(pBoard, C7, BLACK);
    EXPECT_FALSE(discoveredAtks);

}

TEST_F(PositionalTest, Pinner) {
    Bitboard pinner = 0ULL;
    Bitboard pinned = 0ULL;


    parseFen(pBoard, "r2q3k/pb1np2p/1ppp1rpb/1B5n/3QP1N1/1NP1B3/PP3PPP/R3K2R w KQ - 4 10");
    pinner = getPinner(pBoard, H8, BLACK);
    pinned = getPinned(pBoard, H8, BLACK);
    EXPECT_TRUE(pinner & (1ULL << D4));
    EXPECT_TRUE(pinned & (1ULL << F6));


    parseFen(pBoard, "r2q4/pb1Qp1kp/1ppp1rpb/1B5n/3BP1N1/1NP5/PP3PPP/R3K2R w KQ - 4 10");
    pinner = getPinner(pBoard, G7, BLACK);
    pinned = getPinned(pBoard, G7, BLACK);
    EXPECT_TRUE(pinner & (1ULL << D4));
    EXPECT_TRUE(pinner & (1ULL << D7));
    EXPECT_TRUE(pinned & (1ULL << F6));
    EXPECT_TRUE(pinned & (1ULL << E7));

}


} // namespace PositionalTest
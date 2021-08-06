#include "testFixtures.h"

using namespace SeeTest;

TEST_F(SeeTest, PawnCap) {
	parseFen(pBoard, "4R3/2r3p1/5bk1/1p1r3p/p2PR1P1/P1BK1P2/1P6/8 b - - 0 1");
	int move = serializeMove(H5, G4, NORMAL_MOVE, Pieces::NO_PIECE);
	EXPECT_EQ(see(pBoard, move), 0);
	EXPECT_EQ(see_ge(pBoard, move, 0), 1);
}

TEST_F(SeeTest, PawnCap2) {
	parseFen(pBoard, "4R3/2r3p1/5bk1/1p1r1p1p/p2PR1P1/P1BK1P2/1P6/8 b - - 0 1");
	int move = serializeMove(H5, G4, NORMAL_MOVE, Pieces::NO_PIECE);
	EXPECT_EQ(see(pBoard, move), 0);
	EXPECT_EQ(see_ge(pBoard, move, 0), 1);
}

TEST_F(SeeTest, RookCap) {
	parseFen(pBoard, "2r2r1k/6bp/p7/2q2p1Q/3PpP2/1B6/P5PP/2RR3K b - - 0 1");
	int move = serializeMove(C5, C1, NORMAL_MOVE, Pieces::NO_PIECE);
	EXPECT_EQ(see(pBoard, move), 2 * pieceValues[chai::ROOK] - pieceValues[chai::QUEEN]);
	EXPECT_EQ(see_ge(pBoard, move, 2 * SEEPieceValues[chai::ROOK] - SEEPieceValues[chai::QUEEN]), 1);
}

TEST_F(SeeTest, EnPas) {
	parseFen(pBoard, "3q2nk/pb1r1p2/np6/3P2Pp/2p1P3/2R4B/PQ3P1P/3R2K1 w - h6 0 1");
	int move = serializeMove(G5, H6, EP_MOVE, Pieces::NO_PIECE);
	//EXPECT_EQ(see(pBoard, move), 0);
	EXPECT_EQ(see_ge(pBoard, move, 0), 1);
}

TEST_F(SeeTest, StackedRooksQueen) {
	parseFen(pBoard, "4q3/1p1pr1k1/1B2rp2/6p1/p3PP2/P3R1P1/1P2R1K1/4Q3 b - - 0 1");
	int move = serializeMove(E6, E4, NORMAL_MOVE, Pieces::NO_PIECE);
	EXPECT_EQ(see(pBoard, move), pieceValues[chai::PAWN] - pieceValues[chai::ROOK]);
	EXPECT_EQ(see_ge(pBoard, move, SEEPieceValues[chai::PAWN] - SEEPieceValues[chai::ROOK]), 1);
}

TEST_F(SeeTest, StackedPieces) {
	parseFen(pBoard, "4q3/1p1pr1kb/1B2rp2/6p1/p3PP2/P3R1P1/1P2R1K1/4Q3 b - - 0 1");
	int move = serializeMove(H7, E4, NORMAL_MOVE, Pieces::NO_PIECE);
	EXPECT_EQ(see(pBoard, move), pieceValues[chai::PAWN]);
	EXPECT_EQ(see_ge(pBoard, move, pieceValues[chai::PAWN]), 1);
}
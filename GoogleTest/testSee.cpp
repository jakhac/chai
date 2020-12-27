#include "pch.h"
#include "perft.h"

namespace {
	class SeeTest : public ::testing::Test {

	protected:
		Board* pBoard;

		virtual void SetUp() {
			pBoard = new Board();
			pBoard->initHashKeys();

			initClearSetMask();
			initSquareToRankFile();
			initAttackerMasks();

			initRookMasks();
			initRookMagicTable();
			initBishopMasks();
			initBishopMagicTable();

			initObstructed();
			initLine();
		}

		virtual void TearDown() {
			delete pBoard;
		}

	};

	TEST_F(SeeTest, PawnCap) {
		pBoard->parseFen("4R3/2r3p1/5bk1/1p1r3p/p2PR1P1/P1BK1P2/1P6/8 b - - 0 1");
		int move = serializeMove(H5, G4, P, EMPTY, EMPTY);
		EXPECT_EQ(see(pBoard, move), 0);
	}

	TEST_F(SeeTest, PawnCap2) {
		pBoard->parseFen("4R3/2r3p1/5bk1/1p1r1p1p/p2PR1P1/P1BK1P2/1P6/8 b - - 0 1");
		int move = serializeMove(H5, G4, P, EMPTY, EMPTY);
		EXPECT_EQ(see(pBoard, move), 0);
	}

	TEST_F(SeeTest, RookCap) {
		pBoard->parseFen("2r2r1k/6bp/p7/2q2p1Q/3PpP2/1B6/P5PP/2RR3K b - - 0 1");
		int move = serializeMove(C5, C1, R, EMPTY, EMPTY);
		EXPECT_EQ(see(pBoard, move), 2 * pieceScores[ROOK] - pieceScores[QUEEN]);
	}

	TEST_F(SeeTest, EnPas) {
		pBoard->parseFen("3q2nk/pb1r1p2/np6/3P2Pp/2p1P3/2R4B/PQ3P1P/3R2K1 w - h6 0 1");
		int move = serializeMove(G5, H6, EMPTY, EMPTY, MFLAG_EP);
		EXPECT_EQ(see(pBoard, move), 0);
	}

	TEST_F(SeeTest, StackedRooksQueen) {
		pBoard->parseFen("4q3/1p1pr1k1/1B2rp2/6p1/p3PP2/P3R1P1/1P2R1K1/4Q3 b - - 0 1");
		int move = serializeMove(E6, E4, P, EMPTY, EMPTY);
		EXPECT_EQ(see(pBoard, move), pieceScores[PAWN] - pieceScores[ROOK]);
	}

	TEST_F(SeeTest, StackedPieces) {
		pBoard->parseFen("4q3/1p1pr1kb/1B2rp2/6p1/p3PP2/P3R1P1/1P2R1K1/4Q3 b - - 0 1");
		int move = serializeMove(H7, E4, P, EMPTY, EMPTY);
		EXPECT_EQ(see(pBoard, move), pieceScores[PAWN]);
	}


} // namespace evaluation
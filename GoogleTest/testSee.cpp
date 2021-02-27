#include "pch.h"
#include "perft.h"

namespace {
	class SeeTest : public ::testing::Test {

	protected:
		board_t* pBoard;

		virtual void SetUp() {
			pBoard = new board_t();
			initHashKeys(pBoard);

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
		parseFen(pBoard, "4R3/2r3p1/5bk1/1p1r3p/p2PR1P1/P1BK1P2/1P6/8 b - - 0 1");
		int move = serializeMove(H5, G4, Piece::P, Piece::EMPTY, Piece::EMPTY);
		EXPECT_EQ(see(pBoard, move), 0);
	}

	TEST_F(SeeTest, PawnCap2) {
		parseFen(pBoard, "4R3/2r3p1/5bk1/1p1r1p1p/p2PR1P1/P1BK1P2/1P6/8 b - - 0 1");
		int move = serializeMove(H5, G4, Piece::P, Piece::EMPTY, Piece::EMPTY);
		EXPECT_EQ(see(pBoard, move), 0);
	}

	TEST_F(SeeTest, RookCap) {
		parseFen(pBoard, "2r2r1k/6bp/p7/2q2p1Q/3PpP2/1B6/P5PP/2RR3K b - - 0 1");
		int move = serializeMove(C5, C1, Piece::R, Piece::EMPTY, Piece::EMPTY);
		EXPECT_EQ(see(pBoard, move), 2 * pieceScores[Piece::ROOK] - pieceScores[Piece::QUEEN]);
	}

	TEST_F(SeeTest, EnPas) {
		parseFen(pBoard, "3q2nk/pb1r1p2/np6/3P2Pp/2p1P3/2R4B/PQ3P1P/3R2K1 w - h6 0 1");
		int move = serializeMove(G5, H6, Piece::EMPTY, Piece::EMPTY, MFLAG_EP);
		EXPECT_EQ(see(pBoard, move), 0);
	}

	TEST_F(SeeTest, StackedRooksQueen) {
		parseFen(pBoard, "4q3/1p1pr1k1/1B2rp2/6p1/p3PP2/P3R1P1/1P2R1K1/4Q3 b - - 0 1");
		int move = serializeMove(E6, E4, Piece::P, Piece::EMPTY, Piece::EMPTY);
		EXPECT_EQ(see(pBoard, move), pieceScores[Piece::PAWN] - pieceScores[Piece::ROOK]);
	}

	TEST_F(SeeTest, StackedPieces) {
		parseFen(pBoard, "4q3/1p1pr1kb/1B2rp2/6p1/p3PP2/P3R1P1/1P2R1K1/4Q3 b - - 0 1");
		int move = serializeMove(H7, E4, Piece::P, Piece::EMPTY, Piece::EMPTY);
		EXPECT_EQ(see(pBoard, move), pieceScores[Piece::PAWN]);
	}


} // namespace evaluation
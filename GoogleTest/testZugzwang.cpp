#include "pch.h"
#include "search.h"

namespace {
	class Zugzwang : public ::testing::Test {

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

	TEST_F(Zugzwang, KingVSKing) {
		parseFen(pBoard, "2k5/8/8/8/8/8/8/2K5 w - - 0 1");
		EXPECT_EQ(zugzwang(pBoard), 1);
	}

	TEST_F(Zugzwang, NoPawns) {
		parseFen(pBoard, "2k1n3/8/1q1r1r2/1R6/B7/R7/4N3/2K5 w - - 0 1");
		EXPECT_EQ(zugzwang(pBoard), 1);
	}

	TEST_F(Zugzwang, PawnOnly) {
		parseFen(pBoard, "2k5/8/1p1ppp2/pP6/8/P4P2/4P3/2K5 w - - 0 1");
		EXPECT_EQ(zugzwang(pBoard), 1);
	}

	TEST_F(Zugzwang, PawnKnight1) {
		parseFen(pBoard, "2k5/5n2/1p1ppp2/pP1N4/8/P4P2/4PN2/2K5 w - - 0 1");
		EXPECT_EQ(zugzwang(pBoard), 1);
	}

	TEST_F(Zugzwang, PawnKnight2) {
		parseFen(pBoard, "2k5/5n2/1p1ppp2/pP1N4/8/P4P2/4PN2/2K5 w - - 0 1");
		EXPECT_EQ(zugzwang(pBoard), 1);
	}

	TEST_F(Zugzwang, PawnBlocks1) {
		parseFen(pBoard, "2k5/5n2/1p1p4/pP6/5Q2/1P2RP2/4PN2/2K5 w - - 0 1");
		EXPECT_EQ(zugzwang(pBoard), 1);
	}

	TEST_F(Zugzwang, PawnPushPossible1) {
		parseFen(pBoard, "2k5/5n2/1p1p4/pP1N4/5Q2/1P3P2/4PN2/2K5 w - - 0 1");
		EXPECT_EQ(zugzwang(pBoard), 0);
	}

	TEST_F(Zugzwang, PawnPushPossible2) {
		parseFen(pBoard, "2k5/5n2/1p1p4/pP1N4/P4Q2/2P2P2/4PN2/2K5 w - - 0 1");
		EXPECT_EQ(zugzwang(pBoard), 0);
	}

	TEST_F(Zugzwang, NoPawnPushPossible) {
		parseFen(pBoard, "2k5/5n2/1p1p4/pP1N4/P4Q2/2P2P2/4PN2/2K5 b - - 0 1");
		EXPECT_EQ(zugzwang(pBoard), 1);
	}

	TEST_F(Zugzwang, ValidPos1) {
		parseFen(pBoard, "2k5/5n2/1p1ppp2/pP1N4/5Q2/P4P2/4PN2/2K5 w - - 0 1");
		EXPECT_EQ(zugzwang(pBoard), 0);
	}

} // namespace zw test
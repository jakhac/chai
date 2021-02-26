#include "pch.h"
#include "perft.h"

namespace {
	class PerftTest : public ::testing::Test {

	protected:
		board_t* pBoard;
		Perft* pPerft;

		virtual void SetUp() {
			pBoard = new board_t();
			pPerft = new Perft();
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
			delete pPerft;
		}

	};

	TEST_F(PerftTest, START_POS) {
		parseFen(pBoard, STARTING_FEN);
		EXPECT_EQ(pPerft->perftRoot(pBoard, 4), 197281);
	}

	TEST_F(PerftTest, MID_FEN) {
		parseFen(pBoard, MID_FEN);
		EXPECT_EQ(pPerft->perftRoot(pBoard, 3), 97862);
	}

	TEST_F(PerftTest, END_FEN_1) {
		parseFen(pBoard, END_FEN_1);
		EXPECT_EQ(pPerft->perftRoot(pBoard, 4), 89363);
	}

	TEST_F(PerftTest, END_FEN_2) {
		parseFen(pBoard, END_FEN_2);
		EXPECT_EQ(pPerft->perftRoot(pBoard, 4), 54703);
	}

	TEST_F(PerftTest, END_FEN_3) {
		parseFen(pBoard, END_FEN_3);
		EXPECT_EQ(pPerft->perftRoot(pBoard, 4), 67197);
	}

	//TEST_F(PerftTest, START_POS_EGAL) {
	//	parseFen(pBoard, STARTING_FEN);
	//	EXPECT_EQ(pPerft->perftLegalRoot(pBoard, 4), 197281);
	//}

	//TEST_F(PerftTest, MID_FEN_LEGAL) {
	//	parseFen(pBoard, MID_FEN);
	//	EXPECT_EQ(pPerft->perftLegalRoot(pBoard, 3), 97862);
	//}

	//TEST_F(PerftTest, END_POS_3_LEGAL) {
	//	parseFen(pBoard, END_FEN_3);
	//	EXPECT_EQ(pPerft->perftLegalRoot(pBoard, 4), 67197);
	//}

} // namespace perft


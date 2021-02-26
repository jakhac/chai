#include "pch.h"
#include "uci.h"

namespace {
	class ChecksTest : public ::testing::Test {

	protected:
		board_t* pBoard;
		search_t* pSearch;

		virtual void SetUp() {
			pBoard = new board_t();
			pSearch = new search_t();

			init(pBoard);
		}

		virtual void TearDown() {
			delete pBoard;
			delete pSearch;
		}

	};

	//// Mating positions
	//TEST_F(ChecksTest, DiscoveredChecks) {
	//	parseFen(pBoard, "rnbq1b1r/pppp1ppp/5k2/8/B3Pp2/2N2Q2/PPP2PPP/R1B1K1NR b KQ e3 0 4");

	//	move_t enPasDisCheck = serializeMove(F4, E3, EMPTY, EMPTY, MFLAG_EP);
	//	move_t captureDisCheck = serializeMove(F4, G3, Q, EMPTY, EMPTY);
	//	move_t legalMove = serializeMove(B7, B6, EMPTY, EMPTY, EMPTY);

	//	EXPECT_EQ(pBoard->leavesKingInCheck(pBoard, enPasDisCheck), true);
	//	EXPECT_EQ(pBoard->leavesKingInCheck(pBoard, captureDisCheck), true);
	//	EXPECT_EQ(pBoard->leavesKingInCheck(pBoard, legalMove), false);
	//}

	//TEST_F(ChecksTest, PawnPushDisChecks) {
	//	parseFen(pBoard, "rnbqkb1r/pppp1ppp/8/7Q/B3Pp2/2N5/PPP2PPP/R1B1K1NR b KQkq - 0 4");
	//	move_t pawnPushCheck1 = serializeMove(D7, D6, EMPTY, EMPTY, EMPTY);
	//	move_t pawnPushCheck2 = serializeMove(F7, F5, EMPTY, EMPTY, MFLAG_PS);
	//	move_t knightJump = serializeMove(B8, C6, EMPTY, EMPTY, EMPTY);

	//	EXPECT_EQ(pBoard->leavesKingInCheck(pBoard, pawnPushCheck1), true);
	//	EXPECT_EQ(pBoard->leavesKingInCheck(pBoard, pawnPushCheck2), true);
	//	EXPECT_EQ(pBoard->leavesKingInCheck(pBoard, knightJump), false);
	//}

	//TEST_F(ChecksTest, EnPasDiscoversCheck) {
	//	parseFen(pBoard, "rnbq1b1r/pppp1ppp/8/8/B1k1Pp1Q/2N5/PPP2PPP/R1B1K1NR b KQ e3 0 4");
	//	move_t enPasDisCheck1 = serializeMove(F4, E3, EMPTY, EMPTY, MFLAG_EP);
	//	EXPECT_EQ(pBoard->leavesKingInCheck(pBoard, enPasDisCheck1), true);

	//	parseFen(pBoard, "rnbq1b1r/pppp1ppp/3k4/8/B3Pp1Q/2N5/PPP2PPB/R3K1NR b KQ e3 0 4");
	//	move_t enPasDisCheck2 = serializeMove(F4, E3, EMPTY, EMPTY, MFLAG_EP);
	//	EXPECT_EQ(pBoard->leavesKingInCheck(pBoard, enPasDisCheck2), true);

	//	parseFen(pBoard, "rnbq1b1r/pppp1ppp/2k5/8/4Pp1Q/2N5/PPP2PBB/R3K1NR b KQ e3 0 4");
	//	move_t enPasDisCheck3 = serializeMove(F4, E3, EMPTY, EMPTY, MFLAG_EP);
	//	EXPECT_EQ(pBoard->leavesKingInCheck(pBoard, enPasDisCheck3), true);

	//	parseFen(pBoard, "rnbq1b1r/pppp1ppp/3k4/8/4Pp1Q/2N5/PPP2PBB/R3K1NR b KQ e3 0 4");
	//	move_t enPasDisCheck4 = serializeMove(F4, E3, EMPTY, EMPTY, MFLAG_EP);
	//	EXPECT_EQ(pBoard->leavesKingInCheck(pBoard, enPasDisCheck4), true);
	//}

	//TEST_F(ChecksTest, KingMovesInCheck) {
	//	parseFen(pBoard, "rnbq1b1r/pppp1ppp/3k4/8/B3Pp1Q/2N5/PPP2PPB/R3K1NR b KQ - 0 4");
	//	move_t kingMovesCheck1 = serializeMove(D6, C6, EMPTY, EMPTY, EMPTY);
	//	move_t kingMovesCheck2 = serializeMove(D6, D5, EMPTY, EMPTY, EMPTY);
	//	move_t legalKingMove = serializeMove(D6, C5, EMPTY, EMPTY, EMPTY);

	//	EXPECT_EQ(pBoard->leavesKingInCheck(pBoard, kingMovesCheck1), true);
	//	EXPECT_EQ(pBoard->leavesKingInCheck(pBoard, kingMovesCheck2), true);
	//	EXPECT_EQ(pBoard->leavesKingInCheck(pBoard, legalKingMove), false);
	//}
}
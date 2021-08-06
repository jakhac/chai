#include "testFixtures.h"

using namespace PerftTest;

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


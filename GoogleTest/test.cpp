#include "pch.h"

#include "board.h"

TEST(TestCaseName, TestName) {

	Board b;
	initClearSetMask();
	initSquareToRankFile();
	b.initHashKeys();
	initAttackerMasks();
	b.parseFen(STARTING_FEN);

  EXPECT_EQ(b.generateZobristKey(), b.zobristKey);
}

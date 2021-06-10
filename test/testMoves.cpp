#include "pch.h"
#include "uci.h"

namespace {
	class MoveTest : public ::testing::Test {

	protected:
		move_t m;

		virtual void SetUp() {

		}

		virtual void TearDown() {

		}

	};

	//TEST_F(MoveTest, CaptureFlag) {
	//	m = serializeMove(0, 0, Piece::N, 0, 0);
	//	EXPECT_EQ((MCHECK_CAP & m) > 0, 1);
	//}


}
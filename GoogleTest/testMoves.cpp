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

	TEST_F(MoveTest, CaptureFlag) {
		m = serializeMove(0, 0, Piece::N, 0, 0);
		EXPECT_EQ((MCHECK_CAP & m) > 0, 1);

		m = serializeMove(0, 0, Piece::q, 0, 0);
		EXPECT_EQ((MCHECK_CAP & m) > 0, 1);

		m = serializeMove(0, 0, 0, 0, MFLAG_EP);
		EXPECT_EQ((MCHECK_CAP & m) > 0, 0);

		m = serializeMove(0, 0, Piece::b, 0, 0);
		EXPECT_EQ((MCHECK_CAP & m) > 0, 1);
	}

	TEST_F(MoveTest, CastleFlag) {
		m = serializeMove(0, 5, 0, 0, MFLAG_CAS);
		EXPECT_EQ((MCHECK_CAS & m) > 0, 1);

		m = serializeMove(0, 5, 0, 0, MFLAG_CAS | MFLAG_PS);
		EXPECT_EQ((MCHECK_CAS & m) > 0, 1);

		m = serializeMove(0, 5, 0, 0, MCHECK_CAP);
		EXPECT_EQ((MCHECK_CAS & m) > 0, 0);
	}

	TEST_F(MoveTest, PromFlag) {
		m = serializeMove(0, 5, 0, Piece::N, 0);
		EXPECT_EQ((MCHECK_PROM & m) > 0, 1);

		m = serializeMove(0, 5, 0, Piece::q, 0);
		EXPECT_EQ((MCHECK_PROM & m) > 0, 1);

		m = serializeMove(0, 5, 0, 0, 0);
		EXPECT_EQ((MCHECK_PROM & m) > 0, 0);
	}

}
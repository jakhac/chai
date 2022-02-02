
#include "../src/uci.h"
#include "../src/perft.h"
#include "../src/bitboard.h"

#include "gtest/gtest.h"


namespace SearchTest {
	class SearchTest : public ::testing::Test {

	protected:
		board_t* pBoard;
		stats_t* pSearch;
		instr_t* pInstr;
	
	public:
		virtual void SetUp() {
			pBoard = new board_t();
			pSearch = new stats_t();
			pInstr = new instr_t();
			
			MoveOrder::init();
			Mask::init();
			Search::init();
			TT::init();
			Threads::initPool();
		}

		virtual void TearDown() {
			TT::freeHashTables();
			Threads::deletePool();
			delete pBoard;
			delete pSearch;
			delete pInstr;
		}

	};
}

namespace PerftTest {
	class PerftTest : public ::testing::Test {

	protected:
		board_t* pBoard;
		Perft* pPerft;

		virtual void SetUp() {
			pBoard = new board_t();
			pPerft = new Perft();

			MoveOrder::init();
			Mask::init();
			Search::init();
		}

		virtual void TearDown() {
			delete pBoard;
			delete pPerft;
		}

	};
}

namespace SeeTest {
	class SeeTest : public ::testing::Test {

	protected:
		board_t* pBoard;

		virtual void SetUp() {
			pBoard = new board_t();

			MoveOrder::init();
			Mask::init();
			Search::init();
		}

		virtual void TearDown() {
			delete pBoard;
		}

	};
}

namespace EvalTest {
	class EvalTest : public ::testing::Test {

	protected:
		board_t* pBoard;

		virtual void SetUp() {
			pBoard = new board_t();

			MoveOrder::init();
			Mask::init();
			Search::init();
		}

		virtual void TearDown() {
			delete pBoard;
		}

	};
}

namespace PositionalTest {
	class PositionalTest : public ::testing::Test {

	protected:
		board_t* pBoard;

		virtual void SetUp() {
			pBoard = new board_t();

			MoveOrder::init();
			Mask::init();
			Search::init();
		}

		virtual void TearDown() {
			delete pBoard;
		}

	};
}

#include "gtest/gtest.h"

#include "../src/uci.h"
#include "../src/perft.h"


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
			
			init();
			initHashTables();
			initThreadPool();
		}

		virtual void TearDown() {
			freeHashTables();
			deleteThreadPool();
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

			init();
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

			init();
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

			init();
		}

		virtual void TearDown() {
			delete pBoard;
		}

	};
}
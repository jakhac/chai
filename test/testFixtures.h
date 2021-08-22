#include "gtest/gtest.h"

#include "../chai/uci.h"
#include "../chai/perft.h"


namespace SearchTest {
	class SearchTest : public ::testing::Test {

	protected:
		board_t* pBoard;
		search_t* pSearch;
	
	public:
		virtual void SetUp() {
			pBoard = new board_t();
			pSearch = new search_t();
			
			init();
			initHashTables();
			initThreadPool();
		}

		virtual void TearDown() {
			freeHashTables();
			deleteThreadPool();
			delete pBoard;
			delete pSearch;
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
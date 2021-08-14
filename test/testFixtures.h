#include "gtest/gtest.h"

#include "../chai/uci.h"
#include "../chai/perft.h"


namespace SearchTest {
	class SearchTest : public ::testing::Test {

	protected:
		board_t* pBoard;
		search_t* pSearch;
	
	public:
		SearchTest() {
			pBoard = new board_t();
			pSearch = new search_t();
			
			init();
			if (!resizeHashTables(DEFAULT_TT_SIZE)) {
				cout << "Error in memory allocation for TT." << endl;
				exit(1);
			}
		}

		~SearchTest() override { 
			freeHashTables();
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

namespace Zugzwang {
	class Zugzwang : public ::testing::Test {

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
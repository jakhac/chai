#include "pch.h"
#include "perft.h"

namespace {
    class PerftTest : public ::testing::Test {

    protected:
        Board* pBoard;
        Perft* pPerft;

        virtual void SetUp() {
            pBoard = new Board();
            pPerft = new Perft();
            pBoard->initHashKeys();

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
        pBoard->parseFen(STARTING_FEN);
        EXPECT_EQ(pPerft->perftRoot(pBoard, 4), 197281);
    }

    TEST_F(PerftTest, MID_POS) {
        pBoard->parseFen(MID_FEN);
        EXPECT_EQ(pPerft->perftRoot(pBoard, 3), 97862);
    }

    TEST_F(PerftTest, END_POS_1) {
        pBoard->parseFen(END_FEN_1);
        EXPECT_EQ(pPerft->perftRoot(pBoard, 4), 89363);
    }

    TEST_F(PerftTest, END_POS_2) {
        pBoard->parseFen(END_FEN_2);
        EXPECT_EQ(pPerft->perftRoot(pBoard, 4), 54703);
    }

    TEST_F(PerftTest, END_POS_3) {
        pBoard->parseFen(END_FEN_3);
        EXPECT_EQ(pPerft->perftRoot(pBoard, 4), 67197);
    }

}
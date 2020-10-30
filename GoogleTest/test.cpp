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
} // namespace perft

namespace {
    class EvalTest: public ::testing::Test {

    protected:
        Board* pBoard;

        virtual void SetUp() {
            pBoard = new Board();
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
        }

    };

    TEST_F(EvalTest, PawnCap) {
        pBoard->parseFen("4R3/2r3p1/5bk1/1p1r3p/p2PR1P1/P1BK1P2/1P6/8 b - - 0 1");
        int move = MOVE(H5, G4, P, EMPTY, EMPTY);
        EXPECT_EQ(see(pBoard, move), 0);
    }

    TEST_F(EvalTest, PawnCap2) {
        pBoard->parseFen("4R3/2r3p1/5bk1/1p1r1p1p/p2PR1P1/P1BK1P2/1P6/8 b - - 0 1");
        int move = MOVE(H5, G4, P, EMPTY, EMPTY);
        EXPECT_EQ(see(pBoard, move), 0);
    }

    TEST_F(EvalTest, RookCap) {
        pBoard->parseFen("2r2r1k/6bp/p7/2q2p1Q/3PpP2/1B6/P5PP/2RR3K b - - 0 1");
        int move = MOVE(C5, C1, R, EMPTY, EMPTY);
        EXPECT_EQ(see(pBoard, move), 2*pieceScores[ROOK] - pieceScores[QUEEN]);
    }

    TEST_F(EvalTest, EnPas) {
        pBoard->parseFen("3q2nk/pb1r1p2/np6/3P2Pp/2p1P3/2R4B/PQ3P1P/3R2K1 w - h6 0 1");
        int move = MOVE(G5, H6, EMPTY, EMPTY, MFLAGEP);
        EXPECT_EQ(see(pBoard, move), 0);
    }

    TEST_F(EvalTest, StackedRooksQueen) {
        pBoard->parseFen("4q3/1p1pr1k1/1B2rp2/6p1/p3PP2/P3R1P1/1P2R1K1/4Q3 b - - 0 1");
        int move = MOVE(E6, E4, P, EMPTY, EMPTY);
        EXPECT_EQ(see(pBoard, move), pieceScores[PAWN] - pieceScores[ROOK]);
    }

    TEST_F(EvalTest, StackedPieces) {
        pBoard->parseFen("4q3/1p1pr1kb/1B2rp2/6p1/p3PP2/P3R1P1/1P2R1K1/4Q3 b - - 0 1");
        int move = MOVE(H7, E4, P, EMPTY, EMPTY);
        EXPECT_EQ(see(pBoard, move), pieceScores[PAWN]);
    }


} // namespace evaluation
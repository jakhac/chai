#pragma once

#include "stdlib.h"
#include <string>
#include <cassert>
#include <iostream>
#include <fstream>
#include <chrono> // time measurement

#include "types.h"

using namespace std;

// #define TESTING
// #define ASSERT
// #define INFO

/**
 * Write logging information into log.txt file.
 *
 * @param  logMsg Message to write into log file.
 */
inline void logDebug(string logMsg, string file, string line) {
#if defined(_MSC_VER)
	time_t now = time(0);
	tm gmtm[1];
	char buffer[26]{};
	gmtime_s(gmtm, &now);
	asctime_s(buffer, gmtm);
	ofstream ofs("./assertLog.txt", std::ios_base::app);
	string streamMsg = "At " + string(buffer) + " Error in:" + file + " at line " + line + "\n\n";
	cout << streamMsg;
	ofs << streamMsg;
	ofs.close();
#endif
}

#ifndef ASSERT
#define Assert(n)
#else
#define Assert(n) \
if(!(n)) { \
printf("%s - Failed",#n); \
printf("On %s ",__DATE__); \
printf("At %s ",__TIME__); \
printf("In File %s ",__FILE__); \
printf("At Line %d\n",__LINE__); \
logDebug("Failed Assert in File", __FILE__, to_string(__LINE__)); \
exit(1); \
}
#endif // ASSERT

const int NUM_SQUARES = 64;
const int MAX_POSITION_MOVES = 256;
const int DEFAULT_EP_SQ = 65;
const move_t NO_MOVE = 0;
const move_t NULL_MOVE = 4095; // C1-C1 used as nullmove (impossible move, never generated)

const value_t INF = 32000;
const value_t MATE_VALUE = 31000;
const value_t ISMATE = MATE_VALUE - 1000;
const value_t NO_VALUE = INF + 1;

const bitboard_t RANK_1_HEX = 0xFF;
const bitboard_t RANK_2_HEX = 0xFF00;
const bitboard_t RANK_3_HEX = 0xFF0000;
const bitboard_t RANK_4_HEX = 0xFF000000;
const bitboard_t RANK_5_HEX = 0xFF00000000;
const bitboard_t RANK_6_HEX = 0xFF0000000000;
const bitboard_t RANK_7_HEX = 0xFF000000000000;
const bitboard_t RANK_8_HEX = 0xFF00000000000000;

const bitboard_t FILE_A_HEX = 0x0101010101010101;
const bitboard_t FILE_B_HEX = 0x0202020202020202;
const bitboard_t FILE_C_HEX = 0x0404040404040404;
const bitboard_t FILE_D_HEX = 0x0808080808080808;
const bitboard_t FILE_E_HEX = 0x1010101010101010;
const bitboard_t FILE_F_HEX = 0x2020202020202020;
const bitboard_t FILE_G_HEX = 0x4040404040404040;
const bitboard_t FILE_H_HEX = 0x8080808080808080;

const bitboard_t CENTER_SQUARES = (1ULL << D4) | (1ULL << D5) | (1ULL << E4) | (1ULL << E5);

const bitboard_t FILE_LIST[8] = {
	FILE_A_HEX, FILE_B_HEX, FILE_C_HEX, FILE_D_HEX,
	FILE_E_HEX, FILE_F_HEX, FILE_G_HEX, FILE_H_HEX
};

const bitboard_t RANK_LIST[8] = {
	RANK_1_HEX, RANK_2_HEX, RANK_3_HEX, RANK_4_HEX,
	RANK_5_HEX, RANK_6_HEX, RANK_7_HEX, RANK_8_HEX
};

const bitboard_t BLACK_SQUARES = 0xAA55AA55AA55AA55;
const bitboard_t WHITE_SQUARES = 0x55AA55AA55AA55AA;

/* test fens to check parse function */
const string STARTING_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
const string MID_FEN = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
const string END_FEN_1 = "8/PPP4k/8/8/8/8/4Kppp/8 w - - 0 1";
const string END_FEN_2 = "8/3K4/2p5/p2b2r1/5k2/8/8/1q6 b - - 1 67";
const string END_FEN_3 = "8/7p/p5pb/4k3/P1pPn3/8/P5PP/1rB2RK1 b - d3 0 28";
const string BUG_FEN = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
const string PIN_FEN = "r3k3/p1pp1p2/bn2pnp1/3PN3/Nb2r3/5Q1p/PPPBBPPP/R3K2R w KQq - 0 1";
const string EP_CHECK_EVA = "r3k2r/pp1n1ppp/8/2pP1b2/2PK1NqP/1Q2P3/P5P1/2B2B1R w - c6 0 1";

// index is true if the piece is knight / king / bishop / rook
const int pieceKnight[13] = { 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 };
const int pieceKing[13] = { 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1 };
const int pieceRookQueen[13] = { 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0 };
const int pieceRook[13] = { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0 };
const int pieceBishopQueen[13] = { 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0 };

// victim scores used to calculate mvv lva score
const int victimScore[13] = { 0, 100, 200, 300, 400, 500, 600, 100, 200, 300, 400, 500, 600 };

const int pieceValues[13] = { 0, 100, 325, 325, 550, 1000, 0, 100, 325, 325, 550, 1000, 0 };
const int piecePawn[13] = { 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 };

// color for given index
const int pieceCol[13] = { BOTH, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK };

// Converts piece 1..12 to 1..6
const int pieceType[13] = { 0, 1, 2, 3, 4, 5, 6, 1, 2, 3, 4, 5, 6 };

// color to string
const string colorString[2] = { "BLACK", "WHITE" };

// variables to parse fen into board variables and print in console
const string pieceChar = ".PNBRQKpnbrqk";
const string sideChar = "wb-";
const string rankChar = "12345678";
const string fileChar = "abcdefgh";

const bitboard_t rand64();
const bitboard_t randomFewBits();
const int file_rank_2_sq(int f, int r);









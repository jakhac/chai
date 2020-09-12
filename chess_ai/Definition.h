#pragma once

#include "stdlib.h"
#include <string>

using namespace std;

#define DEBUG

#ifndef DEBUG
#define ASSERT(n)
#else
#define ASSERT(n) \
if(!(n)) { \
printf("%s - Failed",#n); \
printf("On %s ",__DATE__); \
printf("At %s ",__TIME__); \
printf("In File %s ",__FILE__); \
printf("At Line %d\n",__LINE__); \
exit(1);}
#endif

typedef unsigned long long U64;

const int NUM_SQUARES = 120;
const int MAX_GAME_MOVES = 2048;
const int MAX_POSITION_MOVES = 256;

enum PIECE_VALUES { EMPTY, P, N, B, R, Q, K, p, n, b, r, q, k };
enum FILES { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NONE };
enum RANKS { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE };
enum COLORS { WHITE, BLACK, BOTH };

enum CASTLING_RIGHTS { K_CASTLE= 1, Q_CASTLE= 2, k_CASTLE = 4, q_CASTLE = 8};

enum SQUARES {
    A1 = 21, B1, C1, D1, E1, F1, G1, H1,
    A2 = 31, B2, C2, D2, E2, F2, G2, H2,
    A3 = 41, B3, C3, D3, E3, F3, G3, H3,
    A4 = 51, B4, C4, D4, E4, F4, G4, H4,
    A5 = 61, B5, C5, D5, E5, F5, G5, H5,
    A6 = 71, B6, C6, D6, E6, F6, G6, H6,
    A7 = 81, B7, C7, D7, E7, F7, G7, H7,
    A8 = 91, B8, C8, D8, E8, F8, G8, H8, NO_SQ, OFFBOARD
};

// TODO convert into .h-.cpp-class
struct UNDO_MOVE {
    int move;
    int castle;
    int enPas;
    int fiftyMove;
    U64 posKey;
};

const string STARTING_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1";
const string BUG_FEN = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
const string QUEEN_FEN = "8/3q1p2/8/5P2/4Q3/8/8/8 w - - 0 1";
const string PAWN_FEN_W = "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1";
const string PAWN_FEN_B = "rnbqkbnr/p1p1p3/3p3p/1p1p4/2P1Pp2/8/PP1P1PpP/RNBQKB1R b KQkq e3 0 1";
const string KNIGHT_KINGS = "5k2/1n6/4n3/6N1/8/3N4/8/5K2 w - - 0 1";
const string ROOKS = "6k1/8/5r2/8/1nR5/5N2/8/6K1 b - - 0 1";
const string QUEENS = "6k1/8/4nq2/8/1nQ5/5N2/1N6/6K1 b - - 0 1";
const string BISHOPS = "6k1/1b6/4n3/8/1n4B1/1B3N2/1N6/2b3K1 b - - 0 1";
const string CASTLE = "r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1";
const string CASTLE2 = "3rk2r/8/8/8/8/8/6p1/R3K2R b KQkq - 0 1";
const string MOVES_48 = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b KQkq - 0 1";

// index is true if the piece is knight / king / bishop / rook
const int pieceKnight[13] = { 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 };
const int pieceKing[13] = { 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1 };
const int pieceRookQueen[13] = { 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0 };
const int pieceBishopQueen[13] = { 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0 };

const int isBQ(int p);
const int isRQ(int p);
const int isN(int p);
const int isK(int p);

/* index true if piece is big / maj / min /wb and value */
const int pieceBig[13] = { 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1 };
const int pieceMaj[13] = { 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1 };
const int pieceMin[13] = { 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0 };
const int pieceVal[13] = { 0, 100, 325, 325, 550, 1000, 50000, 100, 325, 325, 550, 1000, 50000 };
const int piecePawn[13] = { 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 };

/* color for given index */
const int pieceCol[13] = { BOTH, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK };

/* contains piece slides for indexed piece */
const int pieceSlides[13] = { 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0 };

/* variables to parse fen into board variables */
const string pieceChar = ".PNBRQKpnbrqk";
const string sideChar = "wb-";
const string rankChar = "12345678";
const string fileChar = "abcdefgh";

/* Contains directions for pieces for move generation and attacks */
const int nDir[8] = { -8, -19, -21, -12, 8, 19, 21, 12 };
const int rDir[4] = { -1, -10, 1, 10 };
const int bDir[4] = { -9, -11, 11, 9 };
const int kDir[8] = { -1, -10, 1, 10, -9, -11, 11, 9 };

// bitwise and operation with from / to square results in 1111==KQkq representation
const int castlePerm[120] = {
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 13, 15, 15, 15, 12, 15, 15, 14, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15,  7, 15, 15, 15,  3, 15, 15, 11, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15
};

const U64 rand64();
const int file_rank_2_sq(int f, int r);

/* Macros for int Move data */
#define FROMSQ(m) ((m) & 0x7F)
#define TOSQ(m) (((m)>>7) & 0x7F)
#define CAPTURED(m) (((m)>>14) & 0xF)
#define PROMOTED(m) (((m)>>20) & 0xF)

#define MFLAGEP 0x40000
#define MFLAGPS 0x80000
#define MFLAGCA 0x1000000

#define MFLAGCAP 0x7C000
#define MFLAGPROM 0xF00000

#define NOMOVE 0

#define MOVE(f,t,ca,pro,fl) ( (f) | ((t) << 7) | ( (ca) << 14 ) | ( (pro) << 20 ) | (fl))

//#define HASH_PCE(pce,sq) (pos->posKey ^= (PieceKeys[(pce)][(sq)]))
//#define HASH_CA (pos->posKey ^= (CastleKeys[(pos->castlePerm)]))
//#define HASH_SIDE (pos->posKey ^= (SideKey))
//#define HASH_EP (pos->posKey ^= (PieceKeys[EMPTY][(pos->enPas)]))

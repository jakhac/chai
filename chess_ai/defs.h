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

const string STARTING_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
const string BUG_FEN = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";

const string QUEEN_FEN = "8/3q1p2/8/5P2/4Q3/8/8/8 w - - 0 1";

extern int pieceCol[13];


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


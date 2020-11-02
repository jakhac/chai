#pragma once

typedef unsigned long long U64;

struct MOVE_S {
    int moveCounter = 0;
    int moveList[256];
    int moveScore[256];

    U64 attackedSquares = 0ULL;
};

struct UNDO_S {
    int move;
    int castle;
    int enPas;
    int fiftyMove;
    U64 zobKey;
    U64 pawnKey;
};

struct TT_ENTRY_S {
    U64 zobKey;
    int move;
    int score;
    uint8_t flag;
    uint8_t depth;
};

struct TT_S {
    TT_ENTRY_S* table = NULL;
    int entries = 0;

    // measure successful probes
    int probed;
    int hit;
    int valueHit;

    // measure collided keys
    int collided;
    int overwrite;
    int stored;

};

struct PAWN_ENTRY_S {
    U64 zobristPawnKey;
    int eval;
};

struct PAWN_TABLE_S {
    PAWN_ENTRY_S* table = NULL;
    int entries;

    // measure collided keys
    int collided;
    int stored;

    // measure successful probes
    int probed;
    int hit;
};

struct SEARCH_S {
    int startTime;
    int stopTime;
    int depth;
    int movesLeft;

    bool infinite;
    bool depthSet;
    bool timeSet;

    long nodes;

    bool quit = false;
    bool stopped = false;

    // fail high heuristics
    float fh;
    float fhf;

    // pv hits
    int pvHits = 0;
};

enum PIECE_VALUES { EMPTY, P, N, B, R, Q, K, p, n, b, r, q, k };
enum PIECES_TYPES { NO_PIECE, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING };
enum FILES { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NONE };
enum RANKS { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE };
enum COLORS { BLACK, WHITE, BOTH };

enum CASTLING_RIGHTS { K_CASTLE = 1, Q_CASTLE = 2, k_CASTLE = 4, q_CASTLE = 8 };
enum GAME_STATE { START, MID, END };

enum { NONE, TT_ALPHA, TT_BETA, TT_SCORE };

enum SQUARES {
    A8 = 56, B8, C8, D8, E8, F8, G8, H8, NO_SQ, OFFBOARD,
    A7 = 48, B7, C7, D7, E7, F7, G7, H7,
    A6 = 40, B6, C6, D6, E6, F6, G6, H6,
    A5 = 32, B5, C5, D5, E5, F5, G5, H5,
    A4 = 24, B4, C4, D4, E4, F4, G4, H4,
    A3 = 16, B3, C3, D3, E3, F3, G3, H3,
    A2 = 8, B2, C2, D2, E2, F2, G2, H2,
    A1 = 0, B1, C1, D1, E1, F1, G1, H1
};

enum {
    NORTH,
    SOUTH,
    EAST,
    WEST,
    NORTH_EAST,
    NORTH_WEST,
    SOUTH_EAST,
    SOUTH_WEST
};
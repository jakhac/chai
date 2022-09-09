#pragma once

#include <stdint.h>
#include <vector>


#define MAX_DEPTH 64
#define MAX_GAME_MOVES 512
#define BUCKETS 3

#define NUM_SQUARES 64
#define MAX_POSITION_MOVES 256
#define DEFAULT_EP_SQ 0

#define a64 alignas(64)



typedef unsigned long long Bitboard;
typedef unsigned long long Key;

typedef uint16_t Move;

typedef int16_t Value;
typedef int32_t Tuple;

typedef int8_t Weight;
typedef int8_t Clipped;


enum Color {
    BLACK,
    WHITE,
    BW
};

constexpr Color operator!(Color c) {
    return Color(c ^ 1);
}

enum {
    NNUE_ROTATE = 1,
    NNUE_FLIP   = 2
};

enum {
    EMPTY = 0,
    COMPUTED = 1
};

// Namespace to avoid ambiguity with 'b' naming convention for boards
namespace Piece {

    enum {
        NO_PIECE = 0,
        P = 1, N, B, R, Q, K,
        p = 7, n, b, r, q, k
    };

} // namespace Piece

enum PieceType {
    NO_PTYPE, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING
};

const PieceType pieceType[13] = { 
    NO_PTYPE,
    PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING,
    PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING
};


enum FILES {
    FILE_A, FILE_B, FILE_C, FILE_D,
    FILE_E, FILE_F, FILE_G, FILE_H, 
    FILE_NONE
};

enum RANKS {
    RANK_1, RANK_2, RANK_3, RANK_4,
    RANK_5, RANK_6, RANK_7, RANK_8, 
    RANK_NONE
};

enum SQUARES {
    A8 = 56, B8, C8, D8, E8, F8, G8, H8, NO_SQ,
    A7 = 48, B7, C7, D7, E7, F7, G7, H7,
    A6 = 40, B6, C6, D6, E6, F6, G6, H6,
    A5 = 32, B5, C5, D5, E5, F5, G5, H5,
    A4 = 24, B4, C4, D4, E4, F4, G4, H4,
    A3 = 16, B3, C3, D3, E3, F3, G3, H3,
    A2 =  8, B2, C2, D2, E2, F2, G2, H2,
    A1 =  0, B1, C1, D1, E1, F1, G1, H1
};


struct MoveList {
    int cnt = 0;
    a64 int moves[MAX_POSITION_MOVES]{};
    a64 int scores[MAX_POSITION_MOVES]{};

    Bitboard attackedSquares = 0ULL;
};

struct Undo {
    Move move;
    int castle;
    int enPas;
    uint8_t fiftyMove;
    uint8_t cap;
    Key zobKey;
    Key pawnKey;

    Value psqtOpening;
    Value psqtEndgame;
    Value material;
};

struct SearchStack {
    bool isCheck;
    int staticEval;
    Move currentMove;

    Move* pvLine;
};

struct TTEntry {
    uint16_t key = 0; // upper 16bit to determine bucket

    Value value = 0;
    Value staticEval = 0;
    Move move = 0;

    uint8_t flag = 0;
    int8_t depth = 0;

};

struct Bucket {
    TTEntry bucketEntries[BUCKETS];
    char padding[2];
};

struct TTable {
    a64 Bucket* bucketList = nullptr;

    // total number of buckets (entries-stacks)
    int buckets = 0;

    // measure successful probes
    int probed;
    int hit;
    int valueHit;

    // measure collided keys
    int collided;
    int overwrite;
    int stored;
};

struct PTEntry {
    Key zobristPawnKey;
    int16_t eval;
};

struct PTable {
    PTEntry* table = nullptr;
    int entries;

    // measure collided keys
    int collided;
    int stored;

    // measure successful probes
    int probed;
    int hit;
};

struct Accum {
    a64 int16_t accumulation[2][256];
    bool compState[2] = { EMPTY, EMPTY }; 
};

struct Dirty {
    bool isKingMove;

    int changedPieces;
    int piece[3];
    int from[3];
    int to[3];
};

struct Features {
    // Keep track of all feature-indices to add and to indices to delete
    std::vector<std::size_t> addIndex;
    std::vector<std::size_t> delIndex;
};

struct Board {
    Color stm;

    // Current en passant square. DEFAULT_EP_SQ, if not set.
    int enPas;

    // Ply Counter.
    int ply;

    // Ply Counter for undoHistory array.
    int undoPly;

    // Fifty-move rule counter. Resets after captures or pawn moves.
    int fiftyMove;

    // Count half moves. Increment when push or pushnullptr, decrement when pop.
    int halfMoves;

    // CastlePermission stored as number between 0 and 15 (4 bits for each side and color).
    int castlePermission;

    // Unique zobrist key.
    Key zobristKey;

    // Unique zobrist pawn key.
    Key zobristPawnKey;

    // Store pieces for given color.
    Bitboard color[2];

    // Store pieces for given type.
    Bitboard pieces[7];

    // Store occupied squares.
    Bitboard occupied;

    // Stores the currently attacked squares by WHITE/BLACK.
    Bitboard attackedSquares[2];

    // Stack stores pushed moves as Undo objects.
    a64 Undo undoHistory[MAX_GAME_MOVES];

    // PSQT values and material for calculation on-the-fly
    Value psqtOpening = 0;
    Value psqtEndgame = 0;
    Value material    = 0;


    a64 Accum accum[MAX_DEPTH];
    a64 Dirty dp[MAX_DEPTH];
};

// Holds various pruning and counting statistics during search.
struct Stats {
    int futileCnt;
    int futileFH;

    int tbHit;

    int depthReached;

    // fail high heuristics
    float fh;
    float fhf;

    int pvHits = 0;
};

// Contains instructions about time management parsed from
// UCI protocol command.
struct Instructions {
    int startTime;
    int allocatedTime;
    int depth;
    int timeLeft;

    // bool infinite;
    bool depthSet;
    bool timeSet;

    bool quit = false;
    bool stopped = false;
};



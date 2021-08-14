#pragma once

#define MAX_DEPTH 64
#define MAX_GAME_MOVES 512
#define BUCKETS 3

#define NUM_SQUARES 64
#define MAX_POSITION_MOVES 256
#define DEFAULT_EP_SQ 0

/**
 * Unsigned 64-Bit integer represents bitboard isntance.
 */
typedef unsigned long long bitboard_t;

/**
 * Unsigned 64-Bit integer Hash-Keys both hashtables.
 */
typedef unsigned long long key_t;

/**
 * Move type. All moves are unsigned.
 */
typedef uint16_t move_t;

/**
 * Value type. Used for all evalutions.
 */
typedef int16_t value_t;

/**
 * Store moves, scores and number entries in moveList. Used in move generation.
 */
struct moveList_t {
	int cnt = 0;
	int moves[MAX_POSITION_MOVES]{}; // MAX POSITION MOVES
	int scores[MAX_POSITION_MOVES]{}; // MAX POSITION MOVES

	bitboard_t attackedSquares = 0ULL;
};

/**
 * Stores board state to reverse pushed moves.
 */
struct undo_t {
	move_t move;
	int castle;
	int enPas;
	uint8_t fiftyMove;
	uint8_t cap;
	key_t zobKey;
	key_t pawnKey;
};

struct searchStack_t {
	bool isCheck;
	int staticEval;
	move_t currentMove;

	move_t* pvLine;
};

/**
 * Transposition table entry.
 */
struct ttable_entry_t {
	uint16_t key = 0; // upper 16bit to determine bucket

	value_t value = 0;
	value_t staticEval = 0;
	move_t move = 0;

	uint8_t flag = 0;
	int8_t depth = 0;

};

struct bucket_t {
	ttable_entry_t bucketEntries[BUCKETS];
	char padding[2];
};

/**
 * Transposition table instance. Contains stat-variables and pointer to entries.
 */
struct ttable_t {
	bucket_t* bucketList = NULL;

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

/**
 * Pawn Table entry.
 */
struct pawntable_entry_t {
	key_t zobristPawnKey;
	int16_t eval;
};

/**
 * Pawn table instance. Contains stat-variables and pointer to entry.
 */
struct pawntable_t {
	pawntable_entry_t* table = NULL;
	int entries;

	// measure collided keys
	int collided;
	int stored;

	// measure successful probes
	int probed;
	int hit;
};

struct board_t {
	// Current side, 0 for black and 1 for white. Use enums for debug purpose.
	bool stm;

	// Current en passant square. 0, if not set.
	int enPas;

	// Ply Counter.
	int ply;

	// Ply Counter for undoHistory array.
	int undoPly;

	// Fifty-move rule counter. Resets after captures or pawn moves.
	int fiftyMove;

	// Count half moves. Increment when push or pushNull, decrement when pop.
	int halfMoves;

	// CastlePermission stored as number between 0 and 15 (4 bits for each side and color).
	int castlePermission;

	// Unique zobrist key.
	key_t zobristKey;

	// Unique zobrist pawn key.
	key_t zobristPawnKey;

	// Store pieces for given color.
	bitboard_t color[2];

	// Store pieces for given type.
	bitboard_t pieces[7];

	// Store occupied squares.
	bitboard_t occupied;

	// Stores the currently attacked squares by side.
	bitboard_t attackedSquares[2];

	// Stack stores pushed moves as Undo objects.
	undo_t undoHistory[MAX_GAME_MOVES];
};

/**
 * Store search parameters set by UCI commands.
 */
struct search_t {
	int startTime;
	int stopTime;
	int depth;
	int movesLeft;

	bool infinite;
	bool depthSet;
	bool timeSet;

	long nodes;
	long qnodes;

	bool quit = false;
	bool stopped = false;

	int futileCnt;
	int futileFH;

	int tbHit;

	// fail high heuristics
	float fh;
	float fhf;

	// pv hits
	int pvHits = 0;
};

// struct threadState_t {
// 	int id;

// 	// Thread status
// 	bool searching;

// 	// Always false. If set to true, this thread terminates.
// 	// bool terminateThread = false;

// 	// Each thread has its own board to push/pop moves
// 	// and store positional state variables
// 	board_t b;

// 	// Own search_t as thread searches may vary from main search
// 	search_t s;

// 	// Search heuristics for each thread
// 	move_t killer[2][MAX_GAME_MOVES];
// 	move_t mateKiller[MAX_GAME_MOVES];
// 	move_t counterHeuristic[64][64][2];
// 	int histHeuristic[2][64][64];
// 	int histMax = 0;

// 	// Search parameters
// 	value_t bestScore;
// 	move_t bestMove;
// 	move_t pvLine[MAX_DEPTH + 1];
// 	searchStack_t ss[MAX_DEPTH + 1];

// 	int selDepth;


// 	// bool stopThread;
// 	int nodeCount;


// };

namespace Pieces {
	typedef enum PIECE {
		NO_PIECE = 0,
		P = 1, N, B, R, Q, K,
		p = 7, n, b, r, q, k
	} piece_t;
}

namespace chai {
	typedef enum COLORS {
		BLACK, WHITE, BOTH
	} color_t;

	typedef enum nodeType_t {
		PV, NoPV
	} nodeType_t;

	typedef enum PIECES_TYPES {
		NO_TYPE, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING
	} pieceType_t;
}

enum FILES {
	FILE_A, FILE_B, FILE_C, FILE_D,
	FILE_E, FILE_F, FILE_G, FILE_H, FILE_NONE
};

enum RANKS {
	RANK_1, RANK_2, RANK_3, RANK_4,
	RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE
};

enum CASTLING_RIGHTS {
	K_CASTLE = 1, Q_CASTLE = 2, k_CASTLE = 4, q_CASTLE = 8
};

typedef enum TT_FLAG {
	TT_NONE = 0,
	TT_ALPHA = 1,
	TT_BETA = 1 << 1,
	TT_VALUE = 1 << 2,
	TT_EVAL = 1 << 3
} bound_t;

enum MOVE_FLAG {
	NORMAL_MOVE = 0 << 12,
	CASTLE_MOVE = 1 << 12,
	EP_MOVE = 2 << 12,
	PROM_MOVE = 3 << 12,

	PROM_TO_KNIGHT = 0 << 14,
	PROM_TO_BISHOP = 1 << 14,
	PROM_TO_ROOK = 2 << 14,
	PROM_TO_QUEEN = 3 << 14,
};

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

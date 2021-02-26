#pragma once

#include <cstdint>
#include "stdlib.h"

#define MAX_DEPTH 64
#define MAX_GAME_MOVES 512

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
typedef uint32_t move_t;

/**
 * Store moves, scores and number entries in moveList. Used in move generation.
 */
struct moveList_t {
	int cnt = 0;
	int moves[256]; // MAX POSITION MOVES
	int scores[256]; // MAX POSITION MOVES

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
	key_t zobKey;
	key_t pawnKey;
};

/**
 * Transposition table entry.
 */
struct ttable_entry_t {
	uint8_t flag = 0;
	uint8_t depth = 0;
	int16_t score = 0;
	move_t move = 0;
	key_t zobKey = 0x0;
};

/**
 * Transposition table instance. Contains stat-variables and pointer to entries.
 */
struct ttable_t {
	ttable_entry_t* table = NULL;
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
	int side;

	// Current en passant square. 0, if not set.
	int enPas;

	// Ply Counter.
	int ply;

	// Ply Counter for undoHistory array.
	int undoPly;

	// Fifty-move rule counter. Resets after captures.
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

	// Castle keys. // TODO use fixed keys
	key_t castleKeys[16];

	// Stack stores pushed moves as Undo objects.
	undo_t undoHistory[MAX_GAME_MOVES];

	// Transposition table.
	ttable_t tt[1];

	// Pawn hash table.
	pawntable_t pawnTable[1];

	// Stores the pv line.
	move_t pvArray[MAX_DEPTH];
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

	// fail high heuristics
	float fh;
	float fhf;

	// pv hits
	int pvHits = 0;
};

/**
 * Store PV-Line.
 */
struct pv_line_t {
	uint8_t len;
	move_t line[64 * 64]; // MAX DEPTH
};

typedef enum PIECE_VALUES {
	EMPTY, P, N, B, R, Q, K, p, n, b, r, q, k
} piece_t;

enum PIECES_TYPES {
	NO_PIECE, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING
};

enum FILES {
	FILE_A, FILE_B, FILE_C, FILE_D,
	FILE_E, FILE_F, FILE_G, FILE_H, FILE_NONE
};

enum RANKS {
	RANK_1, RANK_2, RANK_3, RANK_4,
	RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE
};

typedef enum COLORS {
	BLACK, WHITE, BOTH
} color_t;

enum CASTLING_RIGHTS {
	K_CASTLE = 1, Q_CASTLE = 2, k_CASTLE = 4, q_CASTLE = 8
};

enum TT_FLAG {
	TT_NONE, TT_ALPHA, TT_BETA, TT_SCORE
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
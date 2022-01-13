#pragma once

#include <algorithm>
#include <sstream>
#include <immintrin.h>

#include "incbin.h"
#include "../board.h"
#include "simd.h"
#include "nnue.h"


#define NNUEFILEVERSIONROTATE       0x7AF32F16u
#define NNUEFILEVERSIONFLIP         0x7AF32F17u
#define NNUENETLAYERHASH            0xCC03DAE4u
#define NNUECLIPPEDRELUHASH         0x538D24C7u
#define NNUEFEATUREHASH             (0x5D69D5B9u ^ 1)
#define NNUEINPUTSLICEHASH          0xEC42E90Du

extern int netType;
extern int orientOperator;
extern bool canUseNNUE;

// Indices to index features
enum {
    PS_WPAWN    =  1,
    PS_BPAWN    =  1 * 64 + 1,
    PS_WKNIGHT  =  2 * 64 + 1,
    PS_BKNIGHT  =  3 * 64 + 1,
    PS_WBISHOP  =  4 * 64 + 1,
    PS_BBISHOP  =  5 * 64 + 1,
    PS_WROOK    =  6 * 64 + 1,
    PS_BROOK    =  7 * 64 + 1,
    PS_WQUEEN   =  8 * 64 + 1,
    PS_BQUEEN   =  9 * 64 + 1,
    PS_END      = 10 * 64 + 1
};

const uint32_t PieceToIndex[2][13] = {
    // Black's perspective
    { 0, PS_BPAWN, PS_BKNIGHT, PS_BBISHOP, PS_BROOK, PS_BQUEEN, 0, PS_WPAWN, PS_WKNIGHT, PS_WBISHOP, PS_WROOK, PS_WQUEEN, 0},

    // White's perspective
    { 0, PS_WPAWN, PS_WKNIGHT, PS_WBISHOP, PS_WROOK, PS_WQUEEN, 0, PS_BPAWN, PS_BKNIGHT, PS_BBISHOP, PS_BROOK, PS_BQUEEN, 0 },
};

int getHalfKPIndex(int sq, int piece, color_t color, int kIdx);

void refreshAccumulator(board_t* b, color_t color);

void accumulateFeatures(board_t* b, color_t color);

void assertActiveFeatures(board_t* b, color_t color, accum_t* accDstTest);

void setActiveFeatures(board_t* b, color_t color, features_t* features);

void updateAccumulator(board_t* b, color_t color, int reusePly);

value_t propagate(board_t* b);

value_t evaluateNNUE(board_t* b);


// Initialize a local .nnue in compilation.
void initIncNet();

/**
 * @brief Read NNUE parameters from the given stream.
 * @return True if init successful, else false
 */
bool initNet(std::istream& ss);
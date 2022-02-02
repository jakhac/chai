#pragma once

#include "incbin.h"
#include "../board.h"
#include "simd.h"
#include "layers.h"



// Either rotation or flip. Defined while parsing a network
extern int orientOperator;

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


template<color_t color>
int getHalfKPIndex(int sq, int piece, int kIdx);

template<color_t color>
void refreshAccumulator(board_t* b);

template<color_t color>
void accumulateFeatures(board_t* b);

template<color_t color>
void setActiveFeatures(board_t* b, features_t* features);

template<color_t color>
void updateAccumulator(board_t* b, int reusePly);

void updateTransformer(board_t* b, clipped_t* output);


namespace NNUE {

value_t propagate(board_t* b);

} // namespace NNUE
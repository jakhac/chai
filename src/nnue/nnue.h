#pragma once

#include "../defs.h"
#include "simd.h"


const int clipShift = 6;

// Feature Layer:
// HalfKP has 2 * 41024 binary features and forwards up to 32-2 (exclude king) active
// features to the first hidden layer HD1 with 512 inputs. Both colors use identical
// weights, thus we only use half of the inputs.
const size_t FEAT_INPUT_SIZE    = 64 * 641;
const size_t FEAT_OUT_SIZE_HALF = 256;
const size_t FEAT_OUT_SIZE      = FEAT_OUT_SIZE_HALF * 2;
const size_t FEAT_NUM_WEIGHTS   = FEAT_OUT_SIZE_HALF * FEAT_INPUT_SIZE;

const size_t INSLICE_OUT_SIZE   = 512;

// Hidden Layer 1
const size_t HD1_IN_SIZE        = 512;
const size_t HD1_OUT_SIZE       = 32;

// Rectified Linear Unit 1
const size_t RELU1_SIZE         = 32;

// Hidden Layer 2
const size_t HD2_IN_SIZE        = 32;
const size_t HD2_OUT_SIZE       = 32;

// Rectified Linear Unit 2
const size_t RELU2_SIZE         = 32;

// Hidden Layer 3 (Out Layer)
const size_t HD3_IN_SIZE     = 32;
const size_t HD3_OUT_SIZE       = 1;

// Feature transformer network
extern int16_t feat_weights[FEAT_INPUT_SIZE * FEAT_OUT_SIZE_HALF];
extern int16_t feat_biases[FEAT_OUT_SIZE_HALF];

// Hidden layer 1
extern weight_t hd1_weights[HD1_OUT_SIZE * HD1_IN_SIZE];
extern int32_t hd1_biases[HD1_OUT_SIZE];

// Hidden layer 2
extern weight_t hd2_weights[HD2_IN_SIZE * HD2_OUT_SIZE];
extern int32_t hd2_biases[HD2_IN_SIZE];

// Hidden layer 3 (out layer)
extern weight_t out_weights[HD3_OUT_SIZE * HD3_IN_SIZE];
extern int32_t out_biases[HD3_OUT_SIZE];


// Cheat sheet
// ps - vectors contain floats (ps stands for packed single-precision)
// pd - vectors contain doubles (pd stands for packed double-precision)
// epi8/epi16/epi32/epi64 - vectors contain 8-bit/16-bit/32-bit/64-bit signed integers
// epu8/epu16/epu32/epu64 - vectors contain 8-bit/16-bit/32-bit/64-bit unsigned integers
// si128/si256 - unspecified 128-bit vector or 256-bit vector
// m128/m128i/m128d/m256/m256i/m256d : identifies input vector types when they're different than the type of the returned vector


// Type: __m128i
// typedef __m128i vec_t;

#define vec_load(a) (*(a))
#define vec_store(a,b) *(a)=(b)
#define vec_add_16(a,b) _mm_add_epi16(a,b)
#define vec_sub_16(a,b) _mm_sub_epi16(a,b)
#define SSE2_NUM_REGS = 16;


void hiddenLayer(clipped_t* in, int32_t* out, layerData_t* layer);

void hiddenLayer1(clipped_t* in, int32_t* out);
void hiddenLayer2(clipped_t* in, int32_t* out);

void clpReluLayer(int32_t* in, clipped_t* out, size_t dim);

void outLayer(clipped_t* in, int32_t* out, size_t inDim);
#pragma once

#include "../defs.h"
#include "simd.h"


// Feature Layer:
// HalfKP has 2 * 41024 binary features and forwards up to 32-2 (exclude king) active
// features to the first hidden layer HD1 with 512 inputs. Both colors use identical
// weights, thus we only use half of the inputs.
const size_t FEAT_INPUT_SIZE    = 64 * 641;
const size_t FEAT_OUT_SIZE_HALF = 256;
const size_t FEAT_OUT_SIZE      = FEAT_OUT_SIZE_HALF * 2;
const size_t FEAT_NUM_WEIGHTS   = FEAT_OUT_SIZE_HALF * FEAT_INPUT_SIZE;

const size_t INSLICE_OUT_SIZE   = 512;

// Architecture of the neural network
const size_t HD1_IN_SIZE  = 512;
const size_t HD1_OUT_SIZE = 32;

const size_t RELU1_SIZE   = 32;

const size_t HD2_IN_SIZE  = 32;
const size_t HD2_OUT_SIZE = 32;

const size_t RELU2_SIZE   = 32;

const size_t HD3_IN_SIZE  = 32;
const size_t HD3_OUT_SIZE = 1;


const int clipShift = 6;


// Parameters of neural network
extern int16_t feat_weights[FEAT_INPUT_SIZE * FEAT_OUT_SIZE_HALF];
extern int16_t feat_biases[FEAT_OUT_SIZE_HALF];

extern weight_t hd1_weights[HD1_OUT_SIZE * HD1_IN_SIZE];
extern int32_t  hd1_biases[HD1_OUT_SIZE];

extern weight_t hd2_weights[HD2_IN_SIZE * HD2_OUT_SIZE];
extern int32_t  hd2_biases[HD2_IN_SIZE];

extern weight_t out_weights[HD3_OUT_SIZE * HD3_IN_SIZE];
extern int32_t  out_biases[HD3_OUT_SIZE];


void hiddenLayer1(clipped_t* in, int32_t* out);
void hiddenLayer2(clipped_t* in, int32_t* out);

void clpReluLayer(int32_t* in, clipped_t* out, size_t dim);

void outLayer(clipped_t* in, int32_t* out);
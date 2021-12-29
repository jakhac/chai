#pragma once

#include "../board.h"
#include "incbin.h"


#define NNUEFILEVERSIONROTATE       0x7AF32F16u
#define NNUEFILEVERSIONFLIP         0x7AF32F17u
#define NNUENETLAYERHASH            0xCC03DAE4u
#define NNUECLIPPEDRELUHASH         0x538D24C7u
#define NNUEFEATUREHASH             (0x5D69D5B9u ^ true)
#define NNUEINPUTSLICEHASH          0xEC42E90Du

#define ALIGN64 alignas(64)


#define INSIZE 41024
#define L1SIZE 24

#define FT_I_SIZE       (64 * 641)
#define FT_O_SIZE_HALF  (256)
#define FT_O_SIZE       (FT_O_SIZE_HALF * 2)
#define FT_NUM_WEIGHTS  (FT_I_SIZE * FT_O_SIZE_HALF)


typedef int16_t weight_t;
typedef int8_t clipped_t;

enum {
    NNUE_ROTATE = 1,
    NNUE_FLIP   = 2
};


const int Feature_outdims = FT_O_SIZE;

const int InputSlice_outputdims = 512;

const int Hidden_1_inputdims = 512;
const int Hidden_1_outdims = 32;

const int ReLu_1_dims = 32;

const int Hidden_2_inputdims = 32;
const int Hidden_2_outdims = 32;

const int ReLu_2_dims = 32;

const int OutLayer_inputdims = 32;
const int OutLayer_outdims = 1;



void parseNet(std::string file);
#include "nnue.h"

const int NnueClippingShift = 6;

// Feature transformer network
a64 int16_t feat_weights[FEAT_INPUT_SIZE * FEAT_OUT_SIZE_HALF];
a64 int16_t feat_biases[FEAT_OUT_SIZE_HALF];

// Hidden Layer 1
a64 weight_t hd1_weights[HD1_OUT_SIZE * HD1_IN_SIZE];
a64 int32_t  hd1_biases[HD1_OUT_SIZE];

// Hidden Layer 2
a64 weight_t hd2_weights[HD2_IN_SIZE * HD2_OUT_SIZE];
a64 int32_t  hd2_biases[HD2_IN_SIZE];

// Hidden Layer 3 (out layer)
a64 weight_t out_weights[HD3_OUT_SIZE * HD3_INPUT_SIZE];
a64 int32_t  out_biases[HD3_OUT_SIZE];


void hiddenLayer(clipped_t* in, int32_t* out, layerData_t* layer) {


#ifdef USE_AVX2

    // TODO

#elif defined(USE_SSSE3)

    auto vOnes = _mm_set1_epi16(1);
    auto vIn   = reinterpret_cast<__m128i*>(in);
    auto vOut  = reinterpret_cast<__m128i*>(out);
    auto vBias = reinterpret_cast<__m128i*>(layer->bias);

    __m128i s0, s1, s2, s3;
    __m128i p1, p2;

    Assert(layer->outDims % 8 == 0);
    Assert(layer->inDims == 32 || layer->inDims % 128 == 0);
    
    for (size_t i = 0; i < layer->outDims / 4; i++) {

        auto vWeights = reinterpret_cast<__m128i*>(&layer->weight[4 * i * layer->inDims]);
        s0 = s1 = s2 = s3 = _mm_setzero_si128();

        for (size_t j = 0; j < layer->inDims / 32; j++) {
            
            p1 = _mm_maddubs_epi16(vIn[2 * j    ], vWeights[0 * layer->inDims / 16 + 2 * j    ]);
            p2 = _mm_maddubs_epi16(vIn[2 * j + 1], vWeights[0 * layer->inDims / 16 + 2 * j + 1]);
            s0 = _mm_add_epi32(s0, _mm_madd_epi16(_mm_add_epi16(p1, p2), vOnes));

            p1 = _mm_maddubs_epi16(vIn[2 * j    ], vWeights[1 * layer->inDims / 16 + 2 * j    ]);
            p2 = _mm_maddubs_epi16(vIn[2 * j + 1], vWeights[1 * layer->inDims / 16 + 2 * j + 1]);
            s1 = _mm_add_epi32(s1, _mm_madd_epi16(_mm_add_epi16(p1, p2), vOnes));

            p1 = _mm_maddubs_epi16(vIn[2 * j    ], vWeights[2 * layer->inDims / 16 + 2 * j    ]);
            p2 = _mm_maddubs_epi16(vIn[2 * j + 1], vWeights[2 * layer->inDims / 16 + 2 * j + 1]);
            s2 = _mm_add_epi32(s2, _mm_madd_epi16(_mm_add_epi16(p1, p2), vOnes));

            p1 = _mm_maddubs_epi16(vIn[2 * j    ], vWeights[3 * layer->inDims / 16 + 2 * j    ]);
            p2 = _mm_maddubs_epi16(vIn[2 * j + 1], vWeights[3 * layer->inDims / 16 + 2 * j + 1]);
            s3 = _mm_add_epi32(s3, _mm_madd_epi16(_mm_add_epi16(p1, p2), vOnes));
        }

        s0 = _mm_hadd_epi32(s0, s1);
        s2 = _mm_hadd_epi32(s2, s3);
        s0 = _mm_hadd_epi32(s0, s2);

        vOut[i] = _mm_add_epi32(s0, vBias[i]); 

    }

#else

    for (size_t i = 0; i < layer->outDims; i++) {
        int offset = i * layer->inDims;
        int32_t sum = layer->bias[i];
        
        for (size_t j = 0; j < layer->inDims; j++)
            sum += layer->weight[offset + j] * in[j];

        out[i] = sum;
    }

#endif

}

void clReluLayer(int32_t* in, clipped_t* out, size_t dim) {
    Assert(RELU1_SIZE == RELU2_SIZE);

#ifdef USE_AVX2

    // TODO

#elif defined(USE_SSSE3)

    const int numChunks = dim / 16;

    __m128i temp, words0, words1;
    __m128i k0x80s = _mm_set1_epi8(-128);

    auto vIn  = reinterpret_cast<__m128i*>(in);
    auto vOut = reinterpret_cast<__m128i*>(out);

    for (int i = 0; i < numChunks; i++) {

        temp   = _mm_packs_epi32(vIn[i * 4 + 0], vIn[i * 4 + 1]);
        words0 = _mm_srai_epi16(temp, NnueClippingShift);

        temp   = _mm_packs_epi32(vIn[i * 4 + 2], vIn[i * 4 + 3]);
        words1 = _mm_srai_epi16(temp, NnueClippingShift);

        temp    = _mm_packs_epi16(words0, words1);
        vOut[i] = _mm_subs_epi8(_mm_adds_epi8(temp, k0x80s), k0x80s);

    }    

#else

    for (size_t i = 0; i < dim; i++)
        out[i] = std::max(0, std::min(127, in[i] >> NnueClippingShift));

#endif
}

void outLayer(clipped_t* in, int32_t* out, size_t inDim) {

#ifdef USE_AVX2

    // TODO

#elif defined(USE_SSSE3)

    Assert(inDim == 32);

    __m128i* vIn     = reinterpret_cast<__m128i*>(in);
    __m128i* vWeight = reinterpret_cast<__m128i*>(out_weights);
    __m128i  vOnes   = _mm_set1_epi16(1);

    __m128i p0  = _mm_madd_epi16(_mm_maddubs_epi16(vIn[0], vWeight[0]), vOnes);
    __m128i p1  = _mm_madd_epi16(_mm_maddubs_epi16(vIn[1], vWeight[1]), vOnes);
    __m128i sum = _mm_add_epi32(p0, p1);

    sum = _mm_add_epi32(sum, _mm_shuffle_epi32(sum, 0xb));
    sum = _mm_add_epi32(sum, _mm_shuffle_epi32(sum, 0x1));

    *out = _mm_cvtsi128_si32(sum) + out_biases[0];

#else

    *out = out_biases[0];
    for (size_t j = 0; j < inDim; j++) {
        *out += out_weights[j] * in[j];
    }

#endif
}

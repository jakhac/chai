#include "nnue.h"

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
a64 weight_t out_weights[HD3_OUT_SIZE * HD3_IN_SIZE];
a64 int32_t  out_biases[HD3_OUT_SIZE];


void hiddenLayer1(clipped_t* in, int32_t* out) {

#if defined(USE_AVX2)

    __m256i* vIn   = reinterpret_cast<__m256i*>(in);
    __m128i* vOut  = reinterpret_cast<__m128i*>(out);
    __m128i* vBias = reinterpret_cast<__m128i*>(hd1_biases);

    __m256i s0, s1, s2, s3;
    __m256i p1, p2;

    const __m256i kOnes = _mm256_set1_epi16(1);

    for (unsigned int i = 0; i < HD1_OUT_SIZE / 4; i++) {

        __m256i* w = reinterpret_cast<__m256i*>(&hd1_weights[4 * i * HD1_IN_SIZE]);
        s0 = s1 = s2 = s3 = _mm256_setzero_si256();

        for (unsigned int j = 0; j < HD1_IN_SIZE / 64; j++) {
            p1 = _mm256_maddubs_epi16(vIn[2 * j    ], w[0 * HD1_IN_SIZE / 32 + 2 * j    ]);
            p2 = _mm256_maddubs_epi16(vIn[2 * j + 1], w[0 * HD1_IN_SIZE / 32 + 2 * j + 1]);
            s0 = _mm256_add_epi32(s0, _mm256_madd_epi16(_mm256_add_epi16(p1, p2), kOnes));

            p1 = _mm256_maddubs_epi16(vIn[2 * j    ], w[1 * HD1_IN_SIZE / 32 + 2 * j    ]);
            p2 = _mm256_maddubs_epi16(vIn[2 * j + 1], w[1 * HD1_IN_SIZE / 32 + 2 * j + 1]);
            s1 = _mm256_add_epi32(s1, _mm256_madd_epi16(_mm256_add_epi16(p1, p2), kOnes));

            p1 = _mm256_maddubs_epi16(vIn[2 * j    ], w[2 * HD1_IN_SIZE / 32 + 2 * j    ]);
            p2 = _mm256_maddubs_epi16(vIn[2 * j + 1], w[2 * HD1_IN_SIZE / 32 + 2 * j + 1]);
            s2 = _mm256_add_epi32(s2, _mm256_madd_epi16(_mm256_add_epi16(p1, p2), kOnes));

            p1 = _mm256_maddubs_epi16(vIn[2 * j    ], w[3 * HD1_IN_SIZE / 32 + 2 * j    ]);
            p2 = _mm256_maddubs_epi16(vIn[2 * j + 1], w[3 * HD1_IN_SIZE / 32 + 2 * j + 1]);
            s3 = _mm256_add_epi32(s3, _mm256_madd_epi16(_mm256_add_epi16(p1, p2), kOnes));
        }

        s0 = _mm256_hadd_epi32(s0, s1);
        s2 = _mm256_hadd_epi32(s2, s3);
        s0 = _mm256_hadd_epi32(s0, s2);

        __m128i sum128 = _mm_add_epi32(_mm256_castsi256_si128(s0),_mm256_extracti128_si256(s0, 1));
            
        vOut[i] = _mm_add_epi32(sum128, vBias[i]);
    }

#elif defined(USE_SSSE3)

    auto vOnes = _mm_set1_epi16(1);
    auto vIn   = reinterpret_cast<__m128i*>(in);
    auto vOut  = reinterpret_cast<__m128i*>(out);
    auto vBias = reinterpret_cast<__m128i*>(hd1_biases);

    __m128i s0, s1, s2, s3;
    __m128i p1, p2;

    Assert(HD1_OUT_SIZE % 8 == 0);
    Assert(HD1_IN_SIZE == 32 || HD1_IN_SIZE % 128 == 0);
    
    for (size_t i = 0; i < HD1_OUT_SIZE / 4; i++) {

        auto vWeights = reinterpret_cast<__m128i*>(&hd1_weights[4 * i * HD1_IN_SIZE]);
        s0 = s1 = s2 = s3 = _mm_setzero_si128();

        for (size_t j = 0; j < HD1_IN_SIZE / 32; j++) {
            
            p1 = _mm_maddubs_epi16(vIn[2 * j    ], vWeights[0 * HD1_IN_SIZE / 16 + 2 * j    ]);
            p2 = _mm_maddubs_epi16(vIn[2 * j + 1], vWeights[0 * HD1_IN_SIZE / 16 + 2 * j + 1]);
            s0 = _mm_add_epi32(s0, _mm_madd_epi16(_mm_add_epi16(p1, p2), vOnes));

            p1 = _mm_maddubs_epi16(vIn[2 * j    ], vWeights[1 * HD1_IN_SIZE / 16 + 2 * j    ]);
            p2 = _mm_maddubs_epi16(vIn[2 * j + 1], vWeights[1 * HD1_IN_SIZE / 16 + 2 * j + 1]);
            s1 = _mm_add_epi32(s1, _mm_madd_epi16(_mm_add_epi16(p1, p2), vOnes));

            p1 = _mm_maddubs_epi16(vIn[2 * j    ], vWeights[2 * HD1_IN_SIZE / 16 + 2 * j    ]);
            p2 = _mm_maddubs_epi16(vIn[2 * j + 1], vWeights[2 * HD1_IN_SIZE / 16 + 2 * j + 1]);
            s2 = _mm_add_epi32(s2, _mm_madd_epi16(_mm_add_epi16(p1, p2), vOnes));

            p1 = _mm_maddubs_epi16(vIn[2 * j    ], vWeights[3 * HD1_IN_SIZE / 16 + 2 * j    ]);
            p2 = _mm_maddubs_epi16(vIn[2 * j + 1], vWeights[3 * HD1_IN_SIZE / 16 + 2 * j + 1]);
            s3 = _mm_add_epi32(s3, _mm_madd_epi16(_mm_add_epi16(p1, p2), vOnes));
        }

        s0 = _mm_hadd_epi32(s0, s1);
        s2 = _mm_hadd_epi32(s2, s3);
        s0 = _mm_hadd_epi32(s0, s2);

        vOut[i] = _mm_add_epi32(s0, vBias[i]); 

    }

#else

    for (size_t i = 0; i < HD1_OUT_SIZE; i++) {
        int offset = i * HD1_IN_SIZE;
        int32_t sum = hd1_biases[i];
        
        for (size_t j = 0; j < HD1_IN_SIZE; j++)
            sum += hd1_weights[offset + j] * in[j];

        out[i] = sum;
    }

#endif

}

void hiddenLayer2(clipped_t* in, int32_t* out) {

#if defined(USE_AVX2)

    __m128i* vIn   = reinterpret_cast<__m128i*>(in);
    __m256i* vOut  = reinterpret_cast<__m256i*>(out);
    __m256i* vBias = reinterpret_cast<__m256i*>(hd2_biases);

    __m256i s0, s1, s2, s3, p;

    __m256i in0 = _mm256_broadcastsi128_si256(vIn[0]);
    __m256i in1 = _mm256_broadcastsi128_si256(vIn[1]);
    const __m256i kOnes = _mm256_set1_epi16(1);

    for (unsigned int i = 0; i < HD2_OUT_SIZE / 8; i++) {
        __m256i* w = reinterpret_cast<__m256i*>(&hd2_weights[8 * i * 32]);

        s0 = _mm256_maddubs_epi16(in0, w[0]); // first half of rows 0,4
        s0 = _mm256_madd_epi16(s0, kOnes);
        p  = _mm256_maddubs_epi16(in1, w[1]); // second half of rows 0,4
        p  = _mm256_madd_epi16(p, kOnes);
        s0 = _mm256_add_epi32(s0, p);

        s1 = _mm256_maddubs_epi16(in0, w[2]); // first half of rows 1,5
        s1 = _mm256_madd_epi16(s1, kOnes);
        p  = _mm256_maddubs_epi16(in1, w[3]); // second half of rows 1,5
        p  = _mm256_madd_epi16(p, kOnes);
        s1 = _mm256_add_epi32(s1, p);

        s2 = _mm256_maddubs_epi16(in0, w[4]); // first half of rows 2,6
        s2 = _mm256_madd_epi16(s2, kOnes);
        p  = _mm256_maddubs_epi16(in1, w[5]); // second half of rows 2,6
        p  = _mm256_madd_epi16(p, kOnes);
        s2 = _mm256_add_epi32(s2, p);

        s3 = _mm256_maddubs_epi16(in0, w[6]); // first half of rows 3,7
        s3 = _mm256_madd_epi16(s3, kOnes);
        p  = _mm256_maddubs_epi16(in1, w[7]); // second half of rows 3,7
        p  = _mm256_madd_epi16(p, kOnes);
        s3 = _mm256_add_epi32(s3, p);

        s0 = _mm256_hadd_epi32(s0, s1);
        s2 = _mm256_hadd_epi32(s2, s3);
        s0 = _mm256_hadd_epi32(s0, s2);

        vOut[i] = _mm256_add_epi32(s0, vBias[i]);
    }

#elif defined(USE_SSSE3)

    auto vOnes = _mm_set1_epi16(1);
    auto vIn   = reinterpret_cast<__m128i*>(in);
    auto vOut  = reinterpret_cast<__m128i*>(out);
    auto vBias = reinterpret_cast<__m128i*>(hd2_biases);

    __m128i s0, s1, s2, s3;
    __m128i p1, p2;

    Assert(HD2_OUT_SIZE % 8 == 0);
    Assert(HD2_IN_SIZE == 32 || HD2_IN_SIZE % 128 == 0);
    
    for (size_t i = 0; i < HD2_OUT_SIZE / 4; i++) {

        auto vWeights = reinterpret_cast<__m128i*>(&hd2_weights[4 * i * HD2_IN_SIZE]);
        s0 = s1 = s2 = s3 = _mm_setzero_si128();

        for (size_t j = 0; j < HD2_IN_SIZE / 32; j++) {
            
            p1 = _mm_maddubs_epi16(vIn[2 * j    ], vWeights[0 * HD2_IN_SIZE / 16 + 2 * j    ]);
            p2 = _mm_maddubs_epi16(vIn[2 * j + 1], vWeights[0 * HD2_IN_SIZE / 16 + 2 * j + 1]);
            s0 = _mm_add_epi32(s0, _mm_madd_epi16(_mm_add_epi16(p1, p2), vOnes));

            p1 = _mm_maddubs_epi16(vIn[2 * j    ], vWeights[1 * HD2_IN_SIZE / 16 + 2 * j    ]);
            p2 = _mm_maddubs_epi16(vIn[2 * j + 1], vWeights[1 * HD2_IN_SIZE / 16 + 2 * j + 1]);
            s1 = _mm_add_epi32(s1, _mm_madd_epi16(_mm_add_epi16(p1, p2), vOnes));

            p1 = _mm_maddubs_epi16(vIn[2 * j    ], vWeights[2 * HD2_IN_SIZE / 16 + 2 * j    ]);
            p2 = _mm_maddubs_epi16(vIn[2 * j + 1], vWeights[2 * HD2_IN_SIZE / 16 + 2 * j + 1]);
            s2 = _mm_add_epi32(s2, _mm_madd_epi16(_mm_add_epi16(p1, p2), vOnes));

            p1 = _mm_maddubs_epi16(vIn[2 * j    ], vWeights[3 * HD2_IN_SIZE / 16 + 2 * j    ]);
            p2 = _mm_maddubs_epi16(vIn[2 * j + 1], vWeights[3 * HD2_IN_SIZE / 16 + 2 * j + 1]);
            s3 = _mm_add_epi32(s3, _mm_madd_epi16(_mm_add_epi16(p1, p2), vOnes));
        }

        s0 = _mm_hadd_epi32(s0, s1);
        s2 = _mm_hadd_epi32(s2, s3);
        s0 = _mm_hadd_epi32(s0, s2);

        vOut[i] = _mm_add_epi32(s0, vBias[i]); 

    }

#else

    for (size_t i = 0; i < HD1_OUT_SIZE; i++) {
        int offset = i * HD1_IN_SIZE;
        int32_t sum = hd1_biases[i];
        
        for (size_t j = 0; j < HD1_IN_SIZE; j++)
            sum += hd1_weights[offset + j] * in[j];

        out[i] = sum;
    }

#endif

}

void hiddenLayer(clipped_t* in, int32_t* out, layerData_t* layer) {

#ifdef USE_AVX2

    if (layer->inDims > 32) {
        __m128i* outVec = (__m128i*)out;
        __m128i* biasVec = (__m128i*)layer->bias;
        __m256i* inVec = (__m256i*)in;
        for (unsigned int i = 0; i < layer->outDims / 4; i++) {
            __m256i* w = (__m256i*) & layer->weight[4 * i * layer->inDims];
            __m256i s0, s1, s2, s3;
            s0 = s1 = s2 = s3 = _mm256_setzero_si256();
            const __m256i kOnes = _mm256_set1_epi16(1);
            __m256i p1, p2;
            for (unsigned int j = 0; j < layer->inDims / 64; j++) {
                p1 = _mm256_maddubs_epi16(inVec[2 * j], w[0 * layer->inDims / 32 + 2 * j]);
                p2 = _mm256_maddubs_epi16(inVec[2 * j + 1], w[0 * layer->inDims / 32 + 2 * j + 1]);
                s0 = _mm256_add_epi32(s0, _mm256_madd_epi16(_mm256_add_epi16(p1, p2), kOnes));
                p1 = _mm256_maddubs_epi16(inVec[2 * j], w[1 * layer->inDims / 32 + 2 * j]);
                p2 = _mm256_maddubs_epi16(inVec[2 * j + 1], w[1 * layer->inDims / 32 + 2 * j + 1]);
                s1 = _mm256_add_epi32(s1, _mm256_madd_epi16(_mm256_add_epi16(p1, p2), kOnes));
                p1 = _mm256_maddubs_epi16(inVec[2 * j], w[2 * layer->inDims / 32 + 2 * j]);
                p2 = _mm256_maddubs_epi16(inVec[2 * j + 1], w[2 * layer->inDims / 32 + 2 * j + 1]);
                s2 = _mm256_add_epi32(s2, _mm256_madd_epi16(_mm256_add_epi16(p1, p2), kOnes));
                p1 = _mm256_maddubs_epi16(inVec[2 * j], w[3 * layer->inDims / 32 + 2 * j]);
                p2 = _mm256_maddubs_epi16(inVec[2 * j + 1], w[3 * layer->inDims / 32 + 2 * j + 1]);
                s3 = _mm256_add_epi32(s3, _mm256_madd_epi16(_mm256_add_epi16(p1, p2), kOnes));
            }
            s0 = _mm256_hadd_epi32(s0, s1);
            s2 = _mm256_hadd_epi32(s2, s3);
            s0 = _mm256_hadd_epi32(s0, s2);
            __m128i sum128 = _mm_add_epi32(_mm256_castsi256_si128(s0),
                _mm256_extracti128_si256(s0, 1));
            outVec[i] = _mm_add_epi32(sum128, biasVec[i]);
        }
    }
    else { // 32x32 multiplication
        __m256i* outVec = (__m256i*)out;
        __m256i* biasVec = (__m256i*)layer->bias;
        __m128i* inVec = (__m128i*)in;
        __m256i in0 = _mm256_broadcastsi128_si256(inVec[0]);
        __m256i in1 = _mm256_broadcastsi128_si256(inVec[1]);
        const __m256i kOnes = _mm256_set1_epi16(1);
        __m256i s0, s1, s2, s3, p;
        for (unsigned int i = 0; i < layer->outDims / 8; i++) {
            __m256i* w = (__m256i*) & layer->weight[8 * i * 32];
            s0 = _mm256_maddubs_epi16(in0, w[0]); // first half of rows 0,4
            s0 = _mm256_madd_epi16(s0, kOnes);
            p = _mm256_maddubs_epi16(in1, w[1]); // second half of rows 0,4
            p = _mm256_madd_epi16(p, kOnes);
            s0 = _mm256_add_epi32(s0, p);
            s1 = _mm256_maddubs_epi16(in0, w[2]); // first half of rows 1,5
            s1 = _mm256_madd_epi16(s1, kOnes);
            p = _mm256_maddubs_epi16(in1, w[3]); // second half of rows 1,5
            p = _mm256_madd_epi16(p, kOnes);
            s1 = _mm256_add_epi32(s1, p);
            s2 = _mm256_maddubs_epi16(in0, w[4]); // first half of rows 2,6
            s2 = _mm256_madd_epi16(s2, kOnes);
            p = _mm256_maddubs_epi16(in1, w[5]); // second half of rows 2,6
            p = _mm256_madd_epi16(p, kOnes);
            s2 = _mm256_add_epi32(s2, p);
            s3 = _mm256_maddubs_epi16(in0, w[6]); // first half of rows 3,7
            s3 = _mm256_madd_epi16(s3, kOnes);
            p = _mm256_maddubs_epi16(in1, w[7]); // second half of rows 3,7
            p = _mm256_madd_epi16(p, kOnes);
            s3 = _mm256_add_epi32(s3, p);
            s0 = _mm256_hadd_epi32(s0, s1);
            s2 = _mm256_hadd_epi32(s2, s3);
            s0 = _mm256_hadd_epi32(s0, s2);
            outVec[i] = _mm256_add_epi32(s0, biasVec[i]);
        }
    }

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

void clpReluLayer(int32_t* in, clipped_t* out, size_t dim) {
    Assert(RELU1_SIZE == RELU2_SIZE);

#if defined(USE_AVX2)

    const size_t numChunks = dim / 32;
    const vec_t kZero = _mm256_setzero_si256();

    vec_t* vin = reinterpret_cast<vec_t*>(in);
    vec_t* vout = reinterpret_cast<vec_t*>(out);

    for (unsigned int i = 0; i < numChunks; i++) {
        
        vec_t words0 = _mm256_srai_epi16(_mm256_packs_epi32(
            vin[i * 4 + 0], vin[i * 4 + 1]), clipShift
        );

        vec_t words1 = _mm256_srai_epi16(_mm256_packs_epi32(
            vin[i * 4 + 2], vin[i * 4 + 3]), clipShift
        );

       vout[i] = _mm256_max_epi8(_mm256_packs_epi16(words0, words1), kZero);
    }

#elif defined(USE_SSSE3)

    const int numChunks = dim / 16;

    vec_t temp, words0, words1;
    vec_t k0x80s = _mm_set1_epi8(-128);

    auto vIn  = reinterpret_cast<vec_t*>(in);
    auto vOut = reinterpret_cast<vec_t*>(out);

    for (int i = 0; i < numChunks; i++) {

        temp   = _mm_packs_epi32(vIn[i * 4 + 0], vIn[i * 4 + 1]);
        words0 = _mm_srai_epi16(temp, clipShift);

        temp   = _mm_packs_epi32(vIn[i * 4 + 2], vIn[i * 4 + 3]);
        words1 = _mm_srai_epi16(temp, clipShift);

        temp    = _mm_packs_epi16(words0, words1);
        vOut[i] = _mm_subs_epi8(_mm_adds_epi8(temp, k0x80s), k0x80s);

    }    

#else

    for (size_t i = 0; i < dim; i++)
        out[i] = std::max(0, std::min(127, in[i] >> clipShift));

#endif
}

void outLayer(clipped_t* in, int32_t* out, size_t inDim) {

    Assert(inDim == 32);

#if defined(USE_AVX2)

    vec_t* iv  = reinterpret_cast<vec_t*>(in);
    vec_t* row = reinterpret_cast<vec_t*>(out_weights);
    
    vec_t prod; __m128i sum;

    prod = _mm256_maddubs_epi16(iv[0], row[0]);
    prod = _mm256_madd_epi16(prod, _mm256_set1_epi16(1));
    sum  = _mm_add_epi32(_mm256_castsi256_si128(prod), _mm256_extracti128_si256(prod, 1));
    sum  = _mm_add_epi32(sum, _mm_shuffle_epi32(sum, 0x1b));

    *out = _mm_cvtsi128_si32(sum) + _mm_extract_epi32(sum, 1) + out_biases[0];

#elif defined(USE_SSSE3)

    vec_t* vIn     = reinterpret_cast<vec_t*>(in);
    vec_t* vWeight = reinterpret_cast<vec_t*>(out_weights);
    vec_t  vOnes   = _mm_set1_epi16(1);

    vec_t p0  = _mm_madd_epi16(_mm_maddubs_epi16(vIn[0], vWeight[0]), vOnes);
    vec_t p1  = _mm_madd_epi16(_mm_maddubs_epi16(vIn[1], vWeight[1]), vOnes);
    vec_t sum = _mm_add_epi32(p0, p1);

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

#include "transformer.h"

INCBIN(IncWeights, "../nets/nn-fb50f1a2b1-20210705.nnue");

// Define externs
int netType = 0x0;
int orientOperator;

void assertActiveFeatures(board_t* b, color_t color, accum_t* accDstTest) {
    int16_t* bias = feat_biases;
    accum_t assertAcc;
    features_t features;

    setActiveFeatures(b, color, &features);

    for (size_t i = 0; i < FEAT_OUT_SIZE_HALF; i++)
        assertAcc.accumulation[color][i] = *(bias++);

    for (auto feature : features.addIndex) {
        int offset = FEAT_OUT_SIZE_HALF * feature;

        for (size_t j = 0; j < FEAT_OUT_SIZE_HALF; j++)
            assertAcc.accumulation[color][j] += feat_weights[offset + j];
    }

    for (size_t j = 0; j < FEAT_OUT_SIZE_HALF; j++) {
        Assert(assertAcc.accumulation[color][j] == accDstTest->accumulation[color][j]);
    }

}

template <typename T>
std::string __m128i_toString(const __m128i var) {
    std::stringstream sstr;
    T values[16/sizeof(T)];
    memcpy(values,&var,sizeof(values));
    if (sizeof(T) == 1) {
        for (unsigned int i = 0; i < sizeof(__m128i); i++) {
            sstr << (int) values[i] << " ";
        }
    } else {
        for (unsigned int i = 0; i < sizeof(__m128i) / sizeof(T); i++) {
            sstr << values[i] << " ";
        }
    }
    return sstr.str();
}

uint32_t getOutLayerHash() {

    uint32_t hash;
    
    hash =  NNUEINPUTSLICEHASH ^ INSLICE_OUT_SIZE;
    hash = (NNUENETLAYERHASH + HD1_OUT_SIZE) ^ (hash >> 1) ^ (hash << 31);
    hash =  NNUECLIPPEDRELUHASH + hash;
    hash = (NNUENETLAYERHASH + HD2_OUT_SIZE) ^ (hash >> 1) ^ (hash << 31);
    hash =  NNUECLIPPEDRELUHASH + hash;

    return (NNUENETLAYERHASH + HD3_OUT_SIZE) ^ (hash >> 1) ^ (hash << 31);
}

int orientSq(color_t color, int sq) {
    return (color == chai::BLACK) ? sq ^ orientOperator : sq;
}

int getHalfKPIndex(int sq, int piece, color_t color, int kIdx) {

    int relSq = orientSq(color, sq);
    Assert(pieceValid(piece));

    return relSq + (PieceToIndex[color][piece]) + kIdx;
}

void setActiveFeatures(board_t* b, color_t color, features_t* features) {

    bitboard_t pieces = b->occupied & ~b->pieces[chai::KING];
    int kSq = getKingSquare(b, color);
    int kIdx = PS_END * orientSq(color, kSq);

    while (pieces) {
        int sq = popLSB(&pieces);
        features->addIndex.push_back(getHalfKPIndex(sq, pieceAt(b, sq), color, kIdx));
    }
}

void gatherDirtyFeatures(board_t* b, dirty_t* dp, features_t* features, color_t c) {

    int kSq = getKingSquare(b, c);
    int kIdx = PS_END * orientSq(c, kSq);

    for (int i = 0; i < dp->changedPieces; i++) {

        if (pieceKing[dp->piece[i]])
            continue;

        if (dp->from[i] != NO_SQ)
            features->delIndex.push_back(getHalfKPIndex(dp->from[i], dp->piece[i], c, kIdx));
        
        if (dp->to[i] != NO_SQ)
            features->addIndex.push_back(getHalfKPIndex(dp->to[i], dp->piece[i], c, kIdx));

    }
}

void refreshAccumulator(board_t* b, color_t color) {

#ifdef AVX2

    // TODO

#elif USE_SSSE3

    __m128i registers[NUM_REGS];
    auto tempBias = reinterpret_cast<__m128i*>(&feat_biases);

    accum_t* acc = &b->accum[b->ply];
    features_t features;
    setActiveFeatures(b, color, &features);


    // We have to accumulate respective weights for all 256-neurons in the first layer.
    // This is done stepwise, the first 128 (or complete 256 for AVX2) and second 128
    // neurons successively.
    for (size_t i = 0; i < FEAT_OUT_SIZE_HALF / TILE_HEIGHT; i++) {

        // Copy biases into the registers. Each register contains (SIMD_WIDTH / 16) elements.
        for (size_t k = 0; k < NUM_REGS; k++)
            registers[k] = *(tempBias++);

        // For each feature we take the first (second) 128-weights and add the to the respective
        // first (second) 128 neurons.
        for (auto feature : features.addIndex) {
            int offset = FEAT_OUT_SIZE_HALF * feature + i * TILE_HEIGHT;

            auto col = reinterpret_cast<const __m128i*>(&feat_weights[offset]);
            for (int j = 0; j < NUM_REGS; j++)
                registers[j] = vadd_16(registers[j], col[j]);

        }

        // Finally, we can write the accumulated weights to the accumulator
        auto accTile = reinterpret_cast<__m128i*>(&acc->accumulation[color][i * TILE_HEIGHT]);
        for (unsigned int j = 0; j < NUM_REGS; j++)
            accTile[j] = registers[j];

    }

    acc->compState[color] = COMPUTED;

#else

    int16_t* bias = feat_biases;
    accum_t* acc = &b->accum[b->ply];
    features_t features;

    setActiveFeatures(b, color, &features);

    for (size_t i = 0; i < FEAT_OUT_SIZE_HALF; i++)
        acc->accumulation[color][i] = *(bias++);

    // Accumulate weights
    for (auto feature : features.addIndex) {
        int offset = FEAT_OUT_SIZE_HALF * feature;

        // For a fixed active input feature, add the j-th weight to the 
        // total weight of the j-th neuron
        for (size_t j = 0; j < FEAT_OUT_SIZE_HALF; j++)
            acc->accumulation[color][j] += feat_weights[offset + j];
    }

    acc->compState[color] = COMPUTED;

#endif 

}

void accumulateFeatures(board_t* b, color_t color) {

    int refreshCost = popCount(b->occupied) - 2;
    int reusePly = b->ply;

    // Look for a similar position as long as previous move did not change a king square
    // and the position is not computed
    while (reusePly > 0 && !b->accum[reusePly].compState[color] == COMPUTED) {
        
        refreshCost -= b->dp->changedPieces + 1;
        if (   b->dp[reusePly].isKingMove
            || refreshCost < 0) {

                // Break if update costs too high or the king moved
                break;
            }

        reusePly--;
    }

    // If reusePly is at least 0 and the accumulator at this ply is computed, we can
    // reuse this state instead of a full refresh
    if (   reusePly >= 0
        && b->accum[reusePly].compState[color] == COMPUTED) {
            // cout << "Update sufficient" << endl;
            updateAccumulator(b, color, reusePly);
    } else {
        // cout << "Refresh necessary" << endl;
        refreshAccumulator(b, color);
    }

}

void adjustWeights(accum_t* accDst, accum_t* accSrc, features_t* features, color_t c) {

#ifdef USE_AVX2

    // TODO

#elif USE_SSSE3

    __m128i registers[NUM_REGS];

    for (size_t i = 0; i < FEAT_OUT_SIZE_HALF / TILE_HEIGHT; i++) {

        auto to   = (__m128i*)&accDst->accumulation[c][i * TILE_HEIGHT];
        auto from = (__m128i*)&accSrc->accumulation[c][i * TILE_HEIGHT];

        // Copy current src-accumulator values in 128-bit chunks into our registers
        for (int j = 0; j < NUM_REGS; j++)
            registers[j] = from[j];


        // Delete weight from removed pieces
        for (auto feature : features->delIndex) {
            size_t offset = FEAT_OUT_SIZE_HALF * feature + i * TILE_HEIGHT;

            auto col = reinterpret_cast<__m128i*>(&feat_weights[offset]);
            for (int j = 0; j < NUM_REGS; j++)
                registers[j] = vsub_16(registers[j], col[j]);
        }

        // Add weight from added pieces
        for (auto feature : features->addIndex) {
            size_t offset = FEAT_OUT_SIZE_HALF * feature + i * TILE_HEIGHT;

            auto col = reinterpret_cast<__m128i*>(&feat_weights[offset]);
            for (int j = 0; j < NUM_REGS; j++)
                registers[j] = vadd_16(registers[j], col[j]);
        }

        for (int j = 0; j < NUM_REGS; j++)
            to[j] = registers[j];

    }

#else

    // Copy last accum state that is reused
    for (size_t i = 0; i < FEAT_OUT_SIZE_HALF; i++) {
        accDst->accumulation[c][i] = accSrc->accumulation[c][i];
    }

    // Delete weight from removed pieces
    for (auto feature : features->delIndex) {
        int offset = FEAT_OUT_SIZE_HALF * feature;

        for (size_t j = 0; j < FEAT_OUT_SIZE_HALF; j++)
            accDst->accumulation[c][j] -= feat_weights[offset + j];
    }

    // Add weight from added pieces
    for (auto feature : features->addIndex) {
        int offset = FEAT_OUT_SIZE_HALF * feature;

        for (size_t j = 0; j < FEAT_OUT_SIZE_HALF; j++)
            accDst->accumulation[c][j] += feat_weights[offset + j];
    }

#endif

}

void updateAccumulator(board_t* b, color_t color, int reusePly) {

    // For the following code we have the invariant: reusePly < b-ply
    if (reusePly == b->ply)
        return;

    // Initialize two feature sets. We either update the prveious ply and ignore the second set
    // or utilize a ply further away and gather all additionally
    features_t features[2];

    // Append changed pieces from (reusePly + 1) to the first feature list
    dirty_t* dp = &b->dp[reusePly + 1];
    gatherDirtyFeatures(b, dp, &features[0], color);

    // Append changed pieces for further plies mslast+2 <= ply to indexList1
    for (int p = reusePly + 2; p <= b->ply; p++) {
        dp = &b->dp[p];
        gatherDirtyFeatures(b, dp, &features[1], color);
    }

    // Update computation state as we recalculate accumulators at both plies
    b->accum[reusePly + 1].compState[color] = COMPUTED;
    b->accum[b->ply      ].compState[color] = COMPUTED;

    adjustWeights(&b->accum[reusePly + 1], &b->accum[reusePly], &features[0], color);

    if (reusePly + 1 != b->ply)
        adjustWeights(&b->accum[b->ply], &b->accum[reusePly + 1], &features[1], color);

}


void updateTransformer(board_t* b, clipped_t* output) {

    accumulateFeatures(b, chai::WHITE);
    accumulateFeatures(b, chai::BLACK);

    auto& acc = b->accum[b->ply].accumulation; // TODO copy or pointer?

    std::vector<int> pList = { b->stm, b->stm^1 };
    for (size_t i = 0; i < pList.size(); i++) {

        size_t offset = FEAT_OUT_SIZE_HALF * i;

#ifdef USE_AVX2

    // TODO

#elif USE_SSSE3

        // We can process SIMD_WIDTH-bits within one instruction; so we only
        // loop #chunks times to clip each sum.
        size_t chunks = (FEAT_OUT_SIZE_HALF * 16) / SIMD_WIDTH;
        auto   out    = reinterpret_cast<__m128i*>(&output[offset]);

        for (size_t j = 0; j < chunks/2; j++) {
            auto idx = reinterpret_cast<__m128i*>(acc[pList[i]]);
            _mm_store_si128(&out[j], vclp_8(idx[j * 2], idx[j * 2 + 1]));
        }

#else

    // Native implementation
    for (size_t j = 0; j < FEAT_OUT_SIZE_HALF; j++) {
        int16_t sum = acc[pList[i]][j];
        output[offset + j] = std::max<int16_t>(0, std::min<int16_t>(127, sum));
    }

#endif

    }
}

value_t propagate(board_t* b) {

    layerData_t layer1 = { hd1_weights, hd1_biases, HD1_IN_SIZE, HD1_OUT_SIZE };
    layerData_t layer2 = { hd2_weights, hd2_biases, HD2_IN_SIZE, HD2_OUT_SIZE };

    a64 clipped_t buffer_8[512];
    a64 int32_t buffer_32[512];
    a64 int32_t outvalue;

    updateTransformer(b, buffer_8);

    hiddenLayer(buffer_8, buffer_32, &layer1);
    clReluLayer(buffer_32, buffer_8, RELU1_SIZE);

    hiddenLayer(buffer_8, buffer_32, &layer2);
    clReluLayer(buffer_32, buffer_8, RELU2_SIZE);

    outLayer(buffer_8, &outvalue, HD3_INPUT_SIZE);

    cout << "\nOut layer value " << outvalue << endl;
    // TODO reset custom threads to 1 or param
    return outvalue;
}

value_t evaluateNNUE(board_t* b) {
    return (propagate(b) * 64) / 1024;
}

char* readNetInfo(char* data, uint32_t* v, uint32_t* fHash, uint32_t* size, uint32_t* ftHash) {
    
    a64 uint32_t* data32 = (uint32_t*)data;

    *v = *(data32++);
    if (*v == NNUEFILEVERSIONROTATE) {
        netType = NNUE_ROTATE;
        orientOperator = 0x3f;
    } else if (*v == NNUEFILEVERSIONFLIP) {
        netType = NNUE_FLIP;
        orientOperator = 0x3c;
    }
    
    *fHash = *(data32++);
    *size = *(data32++);

    data = (char*)data32;
    // std::string arch((char*)data32, (char*)data32 + *size);
    // cout << "NNUE Arch: " << arch << endl;
    data += *size;

    data32 = (uint32_t*)data;
    *ftHash = *(data32++);

    return (char*)data32;
}

char* readNetData(uint32_t* data32) {

    // Hidden Layer 1
    for (size_t i = 0; i < HD1_OUT_SIZE; i++) 
        hd1_biases[i] = *(data32++);

    char* data = (char*)data32;
    for (size_t i = 0; i < HD1_OUT_SIZE*HD1_IN_SIZE; i++)
        hd1_weights[i] = *(data++);


    // Hidden Layer 2
    data32 = (uint32_t*)data;
    for (size_t i = 0; i < HD2_OUT_SIZE; i++)
        hd2_biases[i] = *(data32++);

    data = (char*)data32;
    for (size_t i = 0; i < HD2_OUT_SIZE*HD2_IN_SIZE; i++)
        hd2_weights[i] = *(data++);


    // Outlayer
    data32 = (uint32_t*)data;
    for (size_t i = 0; i < HD3_OUT_SIZE; i++)
        out_biases[i] = *(data32++);

    data = (char*)data32;
    for (size_t i = 0; i < HD3_OUT_SIZE*HD3_INPUT_SIZE; i++)
        out_weights[i] = *(data++);


    return data;
}

void initIncNet() {

    // .nnue file is included in binary; we can acces data using <g> pointers
    char* data = const_cast<char*>(reinterpret_cast<const char*>(gIncWeightsData));
    char* end  = (char*)data + gIncWeightsSize;

    uint32_t ftHash   = NNUEFEATUREHASH ^ FEAT_OUT_SIZE;
    uint32_t netHash  = getOutLayerHash();
    uint32_t fileHash = (ftHash ^ netHash);

    uint32_t version, size;
    uint32_t readFeatHash, readFilehash, readNetHash;
    data = readNetInfo(data, &version, &readFilehash, &size, &readFeatHash);

    if (   readFeatHash != ftHash
        || readFilehash != fileHash) {
        cerr << "info string Error: NNUE parsed incorrect hash value." << endl;
        exit(1);
    }

    // Read weights for the feature layer
    uint16_t* data16 = (uint16_t*)data;
    for (size_t i = 0; i < FEAT_OUT_SIZE_HALF; i++)
        feat_biases[i] = *(data16++);

    for (size_t i = 0; i < FEAT_NUM_WEIGHTS; i++)
        feat_weights[i] = *(data16++);


    uint32_t* data32 = (uint32_t*)data16;
    readNetHash = *(data32++);
    if (readNetHash != netHash) {
        cerr << "info string Error: NNUE parsed incorrect hash value." << endl;
        exit(1);
    }

    // Read remaining net weights and biases
    data = readNetData(data32);

    if (data != end) {
        cout << "info string Error: NNUE parsing incomplete." << endl;
        exit(1);
    }

    cout << "info string NNUE parsing completed." << endl;
}


void parseNet(board_t* b) {

    initIncNet();

    propagate(b);
    
}

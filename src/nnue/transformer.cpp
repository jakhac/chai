#include "transformer.h"


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

#if defined(USE_AVX2) || defined(USE_SSSE3)

    vec_t registers[NUM_REGS];
    auto tempBias = reinterpret_cast<vec_t*>(&feat_biases);

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

            auto col = reinterpret_cast<const vec_t*>(&feat_weights[offset]);
            for (int j = 0; j < NUM_REGS; j++)
                registers[j] = vadd_16(registers[j], col[j]);

        }

        // Finally, we can write the accumulated weights to the accumulator
        auto accTile = reinterpret_cast<vec_t*>(&acc->accumulation[color][i * TILE_HEIGHT]);
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

#if defined(USE_AVX2) || defined(USE_SSSE3)

    vec_t registers[NUM_REGS];

    for (size_t i = 0; i < FEAT_OUT_SIZE_HALF / TILE_HEIGHT; i++) {

        auto to   = (vec_t*)&accDst->accumulation[c][i * TILE_HEIGHT];
        auto from = (vec_t*)&accSrc->accumulation[c][i * TILE_HEIGHT];

        // Copy current src-accumulator values in 128/256-bit chunks into our registers
        for (int j = 0; j < NUM_REGS; j++)
            registers[j] = from[j];


        // Delete weight from removed pieces
        for (auto feature : features->delIndex) {
            size_t offset = FEAT_OUT_SIZE_HALF * feature + i * TILE_HEIGHT;

            auto col = reinterpret_cast<vec_t*>(&feat_weights[offset]);
            for (int j = 0; j < NUM_REGS; j++)
                registers[j] = vsub_16(registers[j], col[j]);
        }

        // Add weight from added pieces
        for (auto feature : features->addIndex) {
            size_t offset = FEAT_OUT_SIZE_HALF * feature + i * TILE_HEIGHT;

            auto col = reinterpret_cast<vec_t*>(&feat_weights[offset]);
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

    auto& acc = b->accum[b->ply].accumulation;

    std::vector<int> pList = { b->stm, b->stm^1 };
    for (size_t i = 0; i < pList.size(); i++) {

        size_t offset = FEAT_OUT_SIZE_HALF * i;

#if defined(USE_AVX2) || defined(USE_SSSE3)

        // We can process SIMD_WIDTH-bits within one instruction; so we only
        // loop #chunks times to clip each sum.
        size_t chunks = (FEAT_OUT_SIZE_HALF * 16) / SIMD_WIDTH;
        auto   out    = reinterpret_cast<vec_t*>(&output[offset]);

        for (size_t j = 0; j < chunks/2; j++) {
            auto idx = reinterpret_cast<vec_t*>(acc[pList[i]]);
            vstore(&out[j], vclp_8(idx[j * 2], idx[j * 2 + 1]));
        }

#else

        for (size_t j = 0; j < FEAT_OUT_SIZE_HALF; j++) {
            int16_t sum = acc[pList[i]][j];
            output[offset + j] = std::max<int16_t>(0, std::min<int16_t>(127, sum));
        }

#endif

    }
}

#include "nnue.h"

#ifdef CUSTOM_EVALFILE
INCBIN(IncWeights, CUSTOM_EVALFILE);
#endif // CUSTOM_EVALFILE
// INCBIN(IncWeights, "../nets/nn-fb50f1a2b1-20210705.nnue");


int netType = 0x0;
int orientOperator;
bool canUseNNUE = false;


value_t evaluateNNUE(board_t* b) {
    value_t eval = (propagate(b) * 64) / 1024;
    return std::clamp(eval, VALUE_LOSS, VALUE_WIN);
}

value_t propagate(board_t* b) {

    a64 clipped_t buffer_8[512];
    a64 int32_t buffer_32[512];

    updateTransformer(b, buffer_8);

    hiddenLayer1(buffer_8, buffer_32);
    clpReluLayer(buffer_32, buffer_8, RELU1_SIZE);

    hiddenLayer2(buffer_8, buffer_32);
    clpReluLayer(buffer_32, buffer_8, RELU2_SIZE);

    outLayer(buffer_8, buffer_32);

    cout << "\nOut layer value " << buffer_32[0] << endl;
    return buffer_32[0];
}

void initIncNet() {

#if defined(CUSTOM_EVALFILE)

    char* data = const_cast<char*>(reinterpret_cast<const char*>(gIncWeightsData));
    const int binSize = gIncWeightsSize;

    std::istringstream ss;
    ss.rdbuf()->pubsetbuf(data, binSize);

    initNet(ss);

#endif

}

bool initNet(std::istream& ss) {

    uint32_t ftHash   = NNUEFEATUREHASH ^ FEAT_OUT_SIZE;
    uint32_t netHash  = getOutLayerHash();
    uint32_t fileHash = (ftHash ^ netHash);

    uint32_t version, size;
    uint32_t readFeatHash, readFileHash, readNetHash;

    readFromStream<uint32_t>(ss, &version, 1);
    if (version == NNUEFILEVERSIONROTATE) {
        netType = NNUE_ROTATE;
        orientOperator = 0x3f;
    } else if (version == NNUEFILEVERSIONFLIP) {
        netType = NNUE_FLIP;
        orientOperator = 0x3c;
    } else {
        return false;
    }
    
    // Read several hashes and meta data from net
    readFromStream<uint32_t>(ss, &readFileHash, 1);
    readFromStream<uint32_t>(ss, &size, 1);
    std::string arch;
    arch.resize(size);
    readFromStream<char>(ss, &arch[0], size);
    readFromStream<uint32_t>(ss, &readFeatHash, 1);

    // Input layer weights and biases
    readFromStream<int16_t>(ss, feat_biases, FEAT_OUT_SIZE_HALF);
    readFromStream<int16_t>(ss, feat_weights, FEAT_NUM_WEIGHTS);

    readFromStream<uint32_t>(ss, &readNetHash, 1);

    // Verify all hashes read from stream
    if (   readFeatHash != ftHash
        || readNetHash  != netHash
        || readFileHash != fileHash) {
        return false;
    }

    readNetData(ss);
    canUseNNUE = true;

    return true;
}

void readNetData(std::istream& ss) {

    readFromStream<int32_t>(ss, hd1_biases, HD1_OUT_SIZE);
    for (size_t i = 0; i < HD1_OUT_SIZE * HD1_IN_SIZE; i++)
        readFromStream<weight_t>(ss, &hd1_weights[shuffle(i, HD1_IN_SIZE, false)], 1);

    readFromStream<int32_t>(ss, hd2_biases, HD2_OUT_SIZE);
    for (size_t i = 0; i < HD2_OUT_SIZE * HD2_IN_SIZE; i++)
        readFromStream<weight_t>(ss, &hd2_weights[shuffle(i, HD2_IN_SIZE, false)], 1);

    readFromStream<int32_t>(ss, out_biases, HD3_OUT_SIZE);
    for (size_t i = 0; i < HD3_OUT_SIZE * HD3_IN_SIZE; i++)
        readFromStream<weight_t>(ss, &out_weights[shuffle(i, HD3_IN_SIZE, true)], 1);
}

template<typename T>
void readFromStream(std::istream& ss, T* buffer, size_t cnt) {
    
    ss.read(reinterpret_cast<char*>(buffer), sizeof(T) * cnt);

    if (isBigEndian())
        endswap(buffer);
        
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

int bit_shuffle(int v, int left, int right, int mask) {
    int w = v & mask;
    w = (w << left) | (w >> right);
    return (v & ~mask) | (w & mask);
}

int shuffle(int idx, int dims, bool outlayer) {

#ifdef USE_AVX2

    if (dims > 32)
        idx = bit_shuffle(idx, 1, 1, 0x18);
    else if (dims == 32) {
        idx = bit_shuffle(idx, 2, 1, 0x1c);
        if (!outlayer)
            idx = bit_shuffle(idx, 1, 3, 0xf0);
    }
    return idx;

#else 

    return idx;

#endif // USE_AVX2

}

bool isBigEndian(void) {
    union {
        uint32_t i;
        char c[4];
    } bint = {0x01020304};

    return bint.c[0] == 1; 
}

template <typename T>
void endswap(T* buffer) {
    unsigned char *memp = reinterpret_cast<unsigned char*>(buffer);
    std::reverse(memp, memp + sizeof(T));
}
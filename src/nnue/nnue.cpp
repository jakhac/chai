#include "./nnue.h"

INCBIN(IncWeights, "./nnue/nn-fb50f1a2b1-20210705.nnue");

// Feature transformer network
int16_t in_weights[FT_I_SIZE * FT_O_SIZE_HALF];
int16_t in_biases[FT_I_SIZE];

// Hidden layer 1
weight_t hd1_weights[Hidden_1_outdims * Hidden_1_inputdims];
int32_t hd1_biases[Hidden_1_outdims];

// Hidden layer 2
weight_t hd2_weights[Hidden_2_inputdims * Hidden_2_outdims];
int32_t hd2_biases[Hidden_2_inputdims];

// Out layer
weight_t out_weights[OutLayer_outdims * OutLayer_inputdims];
int32_t out_biases[OutLayer_outdims];



#define NNUEREAD(s, t, l) (memcpy(t, (*s), l), (*s) = (*s) + (l))

uint32_t getFtTransformerHash() {
    return NNUEFEATUREHASH ^ Feature_outdims;
}

uint32_t getInputSliceHash() {
    return NNUEINPUTSLICEHASH ^ InputSlice_outputdims;
}

uint32_t getHiddenLayer_1_Hash() {
    // return (NNUENETLAYERHASH + outputdims) ^ (previous->GetHash() >> 1) ^ (previous->GetHash() << 31);
    uint32_t prevHash = getInputSliceHash();
    return (NNUENETLAYERHASH + Hidden_1_outdims) ^ (prevHash >> 1) ^ (prevHash << 31);
}

uint32_t getReLu_1_Hash() {
    // return NNUECLIPPEDRELUHASH + previous->GetHash();
    return NNUECLIPPEDRELUHASH + getHiddenLayer_1_Hash();
}

uint32_t getHiddenLayer_2_Hash() {
    uint32_t prevHash = getReLu_1_Hash();
    return (NNUENETLAYERHASH + Hidden_2_outdims) ^ (prevHash >> 1) ^ (prevHash << 31);
}

uint32_t getReLu_2_Hash() {
    return NNUECLIPPEDRELUHASH + getHiddenLayer_2_Hash();
}

uint32_t getOutLayerHash() {
    uint32_t prevHash = getReLu_2_Hash();
    return (NNUENETLAYERHASH + OutLayer_outdims) ^ (prevHash >> 1) ^ (prevHash << 31);
}


void readFeatureTransformer() {

    char* data = (char*)gIncWeightsData;
    char* end = data + gIncWeightsSize;

    uint32_t fthash = getFtTransformerHash();
    uint32_t nethash = getOutLayerHash();
    uint32_t filehash = (fthash ^ nethash);

    uint32_t version, hash, size;

    memcpy(&version, data, sizeof(uint32_t));
    data += sizeof(uint32_t);

    memcpy(&hash, data, sizeof(uint32_t));
    data += sizeof(uint32_t);

    memcpy(&size, data, sizeof(uint32_t));
    data += sizeof(uint32_t);

    std::string sarchitecture((char*)data, (char*)data + size);
    data += size;
    cout << "arc " << sarchitecture << endl;

    int nt;
    if (version == NNUEFILEVERSIONROTATE)
        nt = NNUE_ROTATE;
    else if (version == NNUEFILEVERSIONFLIP)
        nt = NNUE_FLIP;
    else {
        cout << "wrong version" << endl;
        return;
    }


    if (hash != filehash) {
        cout << "wrong hash " << endl;        
        return;
    }

    // Start reading the NN now

    memcpy(&hash, data, sizeof(uint32_t));
    data += sizeof(uint32_t);
    if (hash != fthash) {
        cout << "wrong ft hash" << endl;
        exit(EXIT_FAILURE);
    }


    uint16_t* data16 = (uint16_t*)data;
    std::copy(data16, data16 + FT_O_SIZE_HALF, in_biases);
    data16 += FT_O_SIZE_HALF;

    std::copy(data16, data16 + FT_NUM_WEIGHTS, in_weights);
    data16 += FT_NUM_WEIGHTS;


    data = (char*)data16;
    memcpy(&hash, data, sizeof(uint32_t));
    data += sizeof(uint32_t);


    if (hash != nethash) {
        cout << "wrong net hash" << endl;
        exit(EXIT_FAILURE);
    }




    // HD1 BIAS //
    uint32_t* data32 = (uint32_t*)data;
    std::copy(data32, data32 + Hidden_1_outdims, hd1_biases);
    data32 += Hidden_1_outdims;

    // HD1 WEIGHT //
    size_t hd1_weight_size = Hidden_1_outdims * Hidden_1_inputdims;

    char* buffer1 = (char*)calloc(hd1_weight_size, sizeof(char));
    char* w = buffer1;
    
    data = (char*)data32;
    std::copy(data, data + hd1_weight_size, buffer1);
    data += hd1_weight_size;

    for (unsigned int r = 0; r < Hidden_1_outdims; r++) {
        for (unsigned int c = 0; c < Hidden_1_inputdims; c++) {
            unsigned int idx = r * Hidden_1_inputdims + c;
            hd1_weights[idx] = *w++;
        }
    }

    // HD2 BIAS //
    data32 = (uint32_t*)data;
    std::copy(data32, data32 + Hidden_2_outdims, hd2_biases);
    data32 += Hidden_2_outdims;

    // HD2 WEIGHT //
    size_t hd2_weight_size = Hidden_2_outdims * Hidden_2_inputdims;
    char* buffer2 = (char*)calloc(hd2_weight_size, sizeof(char));
    w = buffer2;
    
    data = (char*)data32;
    std::copy(data, data + hd2_weight_size, buffer2);
    data += hd2_weight_size;

    for (unsigned int r = 0; r < Hidden_1_outdims; r++) {
        for (unsigned int c = 0; c < Hidden_2_inputdims; c++) {
            unsigned int idx = r * Hidden_2_inputdims + c;
            hd2_weights[idx] = *w++;
        }
    }


    // OUT BIAS //
    data32 = (uint32_t*)data;
    std::copy(data32, data32 + OutLayer_outdims, out_biases);
    data32 += OutLayer_outdims;

    // OUT WEIGHT //
    size_t out_weight_size = OutLayer_outdims * OutLayer_inputdims;
    char* outbuffer = (char*)calloc(out_weight_size, sizeof(char));
    w = outbuffer;
    
    data = (char*)data32;
    std::copy(data, data + out_weight_size, outbuffer);
    data += out_weight_size;

    for (unsigned int r = 0; r < OutLayer_outdims; r++) {
        for (unsigned int c = 0; c < OutLayer_inputdims; c++) {
            unsigned int idx = r * OutLayer_inputdims + c;
            out_weights[idx] = *w++;
        }
    }

    if (data == end) {
        cout << "hit EOF" << endl;
    } else {
        cout << "error eof "<< endl;
    }

    cout << "Parsing successful" << endl;
}


void parseNet(std::string file) {

    readFeatureTransformer();

    // if (   fread(in_biases, sizeof(int16_t), KPSIZE, sin) != (size_t) KPSIZE)
        // || fread(in_weights, sizeof(int16_t), INSIZE * KPSIZE, fin) != (size_t) INSIZE * KPSIZE)
        // printf("info string Unable to read NNUE file\n"), exit(EXIT_FAILURE);

}

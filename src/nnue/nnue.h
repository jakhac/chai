#pragma once

#include <sstream>
#include <algorithm>

#include "layers.h"
#include "transformer.h"


#define NNUEFILEVERSIONROTATE       0x7AF32F16u
#define NNUEFILEVERSIONFLIP         0x7AF32F17u
#define NNUENETLAYERHASH            0xCC03DAE4u
#define NNUECLIPPEDRELUHASH         0x538D24C7u
#define NNUEFEATUREHASH             (0x5D69D5B9u ^ 1)
#define NNUEINPUTSLICEHASH          0xEC42E90Du


extern int netType;
extern bool canUseNNUE;


namespace NNUE {

/**
 * @brief Initialize a local .nnue from the ../net folder.
 * (no-op if no CUSTOM_EVALFILE is defined)
 * 
 */
void initIncNet();

/**
 * @brief Read NNUE parameters from the given stream.
 * @return True if init successful, else false
 */
bool initNet(std::istream& ss);

/**
 * @brief Read NNUE parameters from the given input stream.
 */
void readNetData(std::istream& ss);

/**
 * @brief Read cnt elements of type T from the stream into the buffer.
 */
template<typename T>
void readFromStream(std::istream& ss, T* buffer, size_t cnt);

/**
 * @brief Propagate active features wrt the given board through neural network and
 * return the value of outlayer neuron.
 */
Value propagate(Board* b);

/**
 * @brief Return the NNUE evaluation for the given board.
 */
Value evaluateNNUE(Board* b);


// Misc functions
uint32_t getOutLayerHash();
int bit_shuffle(int v, int left, int right, int mask);
int shuffle(int idx, int dims, bool outlayer);
bool isBigEndian(void);

template <typename T>
void endswap(T* buffer);

}

#pragma once

#include <sstream>
#include <algorithm>

#include "layers.h"
#include "transformer.h"


extern int netType;
extern bool canUseNNUE;


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
value_t propagate(board_t* b);

/**
 * @brief Return the NNUE evaluation for the given board.
 */
value_t evaluateNNUE(board_t* b);


// Misc functions
uint32_t getOutLayerHash();
int bit_shuffle(int v, int left, int right, int mask);
int shuffle(int idx, int dims, bool outlayer);
bool isBigEndian(void);

template <typename T>
void endswap(T* buffer);

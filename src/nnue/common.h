#pragma once

#include <stdint.h>
#include <immintrin.h>
// #include <emmintrin.h>
// #include <xmmintrin.h>

#if defined(USE_SSSE3)
const int NUM_REGS    =  16;
const int SIMD_WIDTH  = 128;

// Number of biases/weights processable given #REGS and SIMD-width
const int TILE_HEIGHT = (NUM_REGS * SIMD_WIDTH / 16); 

#define vadd_16(a,b)    _mm_add_epi16(a,b)
#define vsub_16(a,b)    _mm_sub_epi16(a,b)

#ifdef USE_SSSE3
#define vclp_8(a,b)     _mm_packs_epi16(_mm_max_epi16(a,_mm_setzero_si128()),_mm_max_epi16(b,_mm_setzero_si128()))
#else
#define vclp_16(a)      _mm_min_epi16(_mm_max_epi16(a,_mm_setzero_si128()),_mm_set1_epi16(127))
#endif

#endif

#pragma once

#include <stdint.h>

#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <x86intrin.h>
#endif

#if defined(USE_AVX2)

typedef __m256i vec_t;

const int NUM_REGS    = 16;
const int SIMD_WIDTH  = 256;
const int TILE_HEIGHT = (NUM_REGS * SIMD_WIDTH / 16); 

#define vadd_16(a,b)    _mm256_add_epi16(a,b)
#define vsub_16(a,b)    _mm256_sub_epi16(a,b)
#define vclp_8(a,b)     _mm256_max_epi8(_mm256_packs_epi16(a,b),_mm256_setzero_si256())
#define vstore(a, b)    _mm256_store_si256(a, b)

#elif defined(USE_SSSE3)

typedef __m128i vec_t;

const int NUM_REGS    =  16;
const int SIMD_WIDTH  = 128;
const int TILE_HEIGHT = (NUM_REGS * SIMD_WIDTH / 16); 

#define vadd_16(a,b)    _mm_add_epi16(a,b)
#define vsub_16(a,b)    _mm_sub_epi16(a,b)
#define vclp_8(a,b)     _mm_packs_epi16(_mm_max_epi16(a,_mm_setzero_si128()),_mm_max_epi16(b,_mm_setzero_si128()))
#define vstore(a, b)    _mm_store_si128(a, b)

#endif



#pragma once

#include "stdlib.h"
#include <string>
#include <cassert>
#include <iostream>
#include <fstream>
#include <chrono> // time measurement
#include <tuple>
#include <sstream> // std::ostringstream


#include "types.h"

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::fixed;


#define STRINGIFY(x) #x
#define TOSTRING(x) std::string(STRINGIFY(x))
#define AT __FILE__ ":" TOSTRING(__LINE__)

#ifndef ASSERT
#define Assert(n)
#define Assert3(n, x, y)
#else
#define Assert(n) \
if(!(n)) { \
    std::string errMsg = "Failed assert \"" + TOSTRING(n) + "\"\n" \
        + "in " + TOSTRING(__FILE__) + ":" + TOSTRING(__LINE__) + "\n" \
        + "at " + getTimeAndDate() +"\n\n"; \
    cout << errMsg; \
    logDebug(errMsg); \
    exit(1); \
}
#define Assert3(n, x, y) \
if(!(n)) { \
    std::string errMsg = "Failed assert \"" + TOSTRING(n) + "\"\n" \
        + TOSTRING(x) + "=" + x + " " + TOSTRING(y) + "=" + y + "\n" \
        + "in " + TOSTRING(__FILE__) + ":" + TOSTRING(__LINE__) + "\n" \
        + "at " + getTime() + "\n\n"; \
    cerr << errMsg; \
    logDebug(errMsg); \
    exit(1); \
}
#endif // ASSERT


static std::string ver_string(int a, int b, int c) {
  std::ostringstream ss;
  ss << a << '.' << b << '.' << c;
  return ss.str();
}


const std::string info_ASSERT = 
#ifdef ASSERT
    "1";
#else
    "0";
#endif // ASSERT

const std::string info_CXX =
#ifdef __clang__
   "clang++ " + ver_string(__clang_major__, __clang_minor__, __clang_patchlevel__);
#else
   "g++ " + ver_string(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#endif

const std::string info_SIMD =
#if defined(USE_AVX2)
    "AVX2";
#elif defined(USE_SSSE3)
    "SSSE3";
#else
    "NONE";
#endif


extern void logDebug(std::string errMsg);
extern std::string getTime();
extern std::string getTimeAndDate();

const std::string STARTING_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
const std::string MID_FEN      = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
const std::string END_FEN_1    = "8/PPP4k/8/8/8/8/4Kppp/8 w - - 0 1";
const std::string END_FEN_2    = "8/3K4/2p5/p2b2r1/5k2/8/8/1q6 b - - 1 67";
const std::string END_FEN_3    = "8/7p/p5pb/4k3/P1pPn3/8/P5PP/1rB2RK1 b - d3 0 28";
const std::string BUG_FEN      = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";

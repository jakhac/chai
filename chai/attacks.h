#pragma once

#include "board.h"
#include "magics.h"

#ifndef ATTACKS_H
#define ATTACKS_H

// store number of bits required for that square in rook magic table index
const int rookIndexBits[64] = {
    12, 11, 11, 11, 11, 11, 11, 12,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    12, 11, 11, 11, 11, 11, 11, 12
};

// store number of bits required for that square in bishop magic table index
const int bishopIndexBits[64] = {
    6, 5, 5, 5, 5, 5, 5, 6,
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 5, 5, 5, 5, 5, 5, 6
};

/// <summary>Magic numbers for bishop squares</summary>
const U64 bishopMagic[64] = {
    U64(0x0002020202020200), U64(0x0002020202020000), U64(0x0004010202000000), U64(0x0004040080000000),
    U64(0x0001104000000000), U64(0x0000821040000000), U64(0x0000410410400000), U64(0x0000104104104000),
    U64(0x0000040404040400), U64(0x0000020202020200), U64(0x0000040102020000), U64(0x0000040400800000),
    U64(0x0000011040000000), U64(0x0000008210400000), U64(0x0000004104104000), U64(0x0000002082082000),
    U64(0x0004000808080800), U64(0x0002000404040400), U64(0x0001000202020200), U64(0x0000800802004000),
    U64(0x0000800400A00000), U64(0x0000200100884000), U64(0x0000400082082000), U64(0x0000200041041000),
    U64(0x0002080010101000), U64(0x0001040008080800), U64(0x0000208004010400), U64(0x0000404004010200),
    U64(0x0000840000802000), U64(0x0000404002011000), U64(0x0000808001041000), U64(0x0000404000820800),
    U64(0x0001041000202000), U64(0x0000820800101000), U64(0x0000104400080800), U64(0x0000020080080080),
    U64(0x0000404040040100), U64(0x0000808100020100), U64(0x0001010100020800), U64(0x0000808080010400),
    U64(0x0000820820004000), U64(0x0000410410002000), U64(0x0000082088001000), U64(0x0000002011000800),
    U64(0x0000080100400400), U64(0x0001010101000200), U64(0x0002020202000400), U64(0x0001010101000200),
    U64(0x0000410410400000), U64(0x0000208208200000), U64(0x0000002084100000), U64(0x0000000020880000),
    U64(0x0000001002020000), U64(0x0000040408020000), U64(0x0004040404040000), U64(0x0002020202020000),
    U64(0x0000104104104000), U64(0x0000002082082000), U64(0x0000000020841000), U64(0x0000000000208800),
    U64(0x0000000010020200), U64(0x0000000404080200), U64(0x0000040404040400), U64(0x0002020202020200)
};

/// <summary>Magic numbers for rook squares</summary>
const U64 rookMagic[64] = {
    U64(0x0080001020400080), U64(0x0040001000200040), U64(0x0080081000200080), U64(0x0080040800100080),
    U64(0x0080020400080080), U64(0x0080010200040080), U64(0x0080008001000200), U64(0x0080002040800100),
    U64(0x0000800020400080), U64(0x0000400020005000), U64(0x0000801000200080), U64(0x0000800800100080),
    U64(0x0000800400080080), U64(0x0000800200040080), U64(0x0000800100020080), U64(0x0000800040800100),
    U64(0x0000208000400080), U64(0x0000404000201000), U64(0x0000808010002000), U64(0x0000808008001000),
    U64(0x0000808004000800), U64(0x0000808002000400), U64(0x0000010100020004), U64(0x0000020000408104),
    U64(0x0000208080004000), U64(0x0000200040005000), U64(0x0000100080200080), U64(0x0000080080100080),
    U64(0x0000040080080080), U64(0x0000020080040080), U64(0x0000010080800200), U64(0x0000800080004100),
    U64(0x0000204000800080), U64(0x0000200040401000), U64(0x0000100080802000), U64(0x0000080080801000),
    U64(0x0000040080800800), U64(0x0000020080800400), U64(0x0000020001010004), U64(0x0000800040800100),
    U64(0x0000204000808000), U64(0x0000200040008080), U64(0x0000100020008080), U64(0x0000080010008080),
    U64(0x0000040008008080), U64(0x0000020004008080), U64(0x0000010002008080), U64(0x0000004081020004),
    U64(0x0000204000800080), U64(0x0000200040008080), U64(0x0000100020008080), U64(0x0000080010008080),
    U64(0x0000040008008080), U64(0x0000020004008080), U64(0x0000800100020080), U64(0x0000800041000080),
    U64(0x00FFFCDDFCED714A), U64(0x007FFCDDFCED714A), U64(0x003FFFCDFFD88096), U64(0x0000040810002101),
    U64(0x0001000204080011), U64(0x0001000204000801), U64(0x0001000082000401), U64(0x0001FFFAABFAD1A2)
};

/// <summary>Array contains all masks for rook moves indexed by square</summary>
extern U64 rookMasks[64];

/// <summary>Table stores attacker set for each square and each blocker set</summary>
extern U64 rookTable[64][4096];

/// <summary>Array contains all masks for bishop moves indexed by square</summary>
extern U64 bishopMasks[64];

/// <summary>Table stores attacker set for each square and each blocker set</summary>
extern U64 bishopTable[64][1024];

/// <summary>
/// Generate a bitboard of blockers unique to the index.
/// </summary>
/// <param name="mask"></param>
/// <param name="i"></param>
/// <returns></returns>
U64 getBlockers(U64 mask, int i);

/// <summary>
/// Initialize rook masks with all possible moves from indexed square.
/// </summary>
void initRookMasks();

/// <summary>
/// Initialize bishop masks with all possible moves from indexed square.
/// </summary>
void initBishopMasks();

/// <summary>
/// Initialize blocker masks for every square.
/// </summary>
void initRookMagicTable();

/// <summary>
/// Initialize blocker masks for every square.
/// </summary>
void initBishopMagicTable();

/// <summary>
/// Calculate all possible rook attacks from given square with blockers. This function is used
/// only to initialize the rook table and should not be used in any other case. 
/// Attacks all occupied pieces, check for correct color before generating attack moves.
/// </summary>
/// <param name="square">Start square</param>
/// <param name="blockers">Mask of blockers</param>
/// <returns></returns>
U64 calculateRookMoves(int square, U64 blockers);

/// <summary>
/// Calculate all possible bishop attacks from given square with blockers. This function is used
/// only to initialize the rook table and should not be used in any other case. 
/// Attacks all occupied pieces, check for correct color before generating attack moves.
/// </summary>
/// <param name="square">Start square</param>
/// <param name="blockers">Mask of blockers</param>
/// <returns></returns>
U64 calculateBishopMoves(int square, U64 blockers);

/// <summary>
/// Fast lookup for possible rook moves. Used in move generation. Possible rook moves and captures. 
/// Attacks all occupied pieces, check for correct color before generating attack moves.
/// </summary>
/// <param name="sq">From square</param>
/// <param name="blockers">Bitboard of currently occupied squares</param>
/// <returns>Bitboard of possible moves and captures</returns>
U64 lookUpRookMoves(int sq, U64 blockers);

/// <summary>
/// Fast lookup for possible bishop moves. Used in move generation. Possible bishop moves and captures. 
/// Attacks all occupied pieces, check for correct color before generating attack moves.
/// </summary>
/// <param name="sq">From square</param>
/// <param name="blockers">Bitboard of currently occupied squares</param>
/// <returns>Bitboard of possible moves and captures</returns>
U64 lookUpBishopMoves(int sq, U64 blockers);

#endif // !ATTACKS_H

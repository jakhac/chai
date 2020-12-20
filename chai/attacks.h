#pragma once

#include "board.h"
#include "magics.h"

#ifndef ATTACKS_H
#define ATTACKS_H

/// <summary>Store number of bits required for that square in rook magic table index</summary>
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

/// <summary>Store number of bits required for that square in bishop magic table index</summary>
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
const bitboard_t bishopMagic[64] = {
    bitboard_t(0x0002020202020200), bitboard_t(0x0002020202020000), bitboard_t(0x0004010202000000), bitboard_t(0x0004040080000000),
    bitboard_t(0x0001104000000000), bitboard_t(0x0000821040000000), bitboard_t(0x0000410410400000), bitboard_t(0x0000104104104000),
    bitboard_t(0x0000040404040400), bitboard_t(0x0000020202020200), bitboard_t(0x0000040102020000), bitboard_t(0x0000040400800000),
    bitboard_t(0x0000011040000000), bitboard_t(0x0000008210400000), bitboard_t(0x0000004104104000), bitboard_t(0x0000002082082000),
    bitboard_t(0x0004000808080800), bitboard_t(0x0002000404040400), bitboard_t(0x0001000202020200), bitboard_t(0x0000800802004000),
    bitboard_t(0x0000800400A00000), bitboard_t(0x0000200100884000), bitboard_t(0x0000400082082000), bitboard_t(0x0000200041041000),
    bitboard_t(0x0002080010101000), bitboard_t(0x0001040008080800), bitboard_t(0x0000208004010400), bitboard_t(0x0000404004010200),
    bitboard_t(0x0000840000802000), bitboard_t(0x0000404002011000), bitboard_t(0x0000808001041000), bitboard_t(0x0000404000820800),
    bitboard_t(0x0001041000202000), bitboard_t(0x0000820800101000), bitboard_t(0x0000104400080800), bitboard_t(0x0000020080080080),
    bitboard_t(0x0000404040040100), bitboard_t(0x0000808100020100), bitboard_t(0x0001010100020800), bitboard_t(0x0000808080010400),
    bitboard_t(0x0000820820004000), bitboard_t(0x0000410410002000), bitboard_t(0x0000082088001000), bitboard_t(0x0000002011000800),
    bitboard_t(0x0000080100400400), bitboard_t(0x0001010101000200), bitboard_t(0x0002020202000400), bitboard_t(0x0001010101000200),
    bitboard_t(0x0000410410400000), bitboard_t(0x0000208208200000), bitboard_t(0x0000002084100000), bitboard_t(0x0000000020880000),
    bitboard_t(0x0000001002020000), bitboard_t(0x0000040408020000), bitboard_t(0x0004040404040000), bitboard_t(0x0002020202020000),
    bitboard_t(0x0000104104104000), bitboard_t(0x0000002082082000), bitboard_t(0x0000000020841000), bitboard_t(0x0000000000208800),
    bitboard_t(0x0000000010020200), bitboard_t(0x0000000404080200), bitboard_t(0x0000040404040400), bitboard_t(0x0002020202020200)
};

/// <summary>Magic numbers for rook squares</summary>
const bitboard_t rookMagic[64] = {
    bitboard_t(0x0080001020400080), bitboard_t(0x0040001000200040), bitboard_t(0x0080081000200080), bitboard_t(0x0080040800100080),
    bitboard_t(0x0080020400080080), bitboard_t(0x0080010200040080), bitboard_t(0x0080008001000200), bitboard_t(0x0080002040800100),
    bitboard_t(0x0000800020400080), bitboard_t(0x0000400020005000), bitboard_t(0x0000801000200080), bitboard_t(0x0000800800100080),
    bitboard_t(0x0000800400080080), bitboard_t(0x0000800200040080), bitboard_t(0x0000800100020080), bitboard_t(0x0000800040800100),
    bitboard_t(0x0000208000400080), bitboard_t(0x0000404000201000), bitboard_t(0x0000808010002000), bitboard_t(0x0000808008001000),
    bitboard_t(0x0000808004000800), bitboard_t(0x0000808002000400), bitboard_t(0x0000010100020004), bitboard_t(0x0000020000408104),
    bitboard_t(0x0000208080004000), bitboard_t(0x0000200040005000), bitboard_t(0x0000100080200080), bitboard_t(0x0000080080100080),
    bitboard_t(0x0000040080080080), bitboard_t(0x0000020080040080), bitboard_t(0x0000010080800200), bitboard_t(0x0000800080004100),
    bitboard_t(0x0000204000800080), bitboard_t(0x0000200040401000), bitboard_t(0x0000100080802000), bitboard_t(0x0000080080801000),
    bitboard_t(0x0000040080800800), bitboard_t(0x0000020080800400), bitboard_t(0x0000020001010004), bitboard_t(0x0000800040800100),
    bitboard_t(0x0000204000808000), bitboard_t(0x0000200040008080), bitboard_t(0x0000100020008080), bitboard_t(0x0000080010008080),
    bitboard_t(0x0000040008008080), bitboard_t(0x0000020004008080), bitboard_t(0x0000010002008080), bitboard_t(0x0000004081020004),
    bitboard_t(0x0000204000800080), bitboard_t(0x0000200040008080), bitboard_t(0x0000100020008080), bitboard_t(0x0000080010008080),
    bitboard_t(0x0000040008008080), bitboard_t(0x0000020004008080), bitboard_t(0x0000800100020080), bitboard_t(0x0000800041000080),
    bitboard_t(0x00FFFCDDFCED714A), bitboard_t(0x007FFCDDFCED714A), bitboard_t(0x003FFFCDFFD88096), bitboard_t(0x0000040810002101),
    bitboard_t(0x0001000204080011), bitboard_t(0x0001000204000801), bitboard_t(0x0001000082000401), bitboard_t(0x0001FFFAABFAD1A2)
};

/// <summary>Array contains all masks for rook moves indexed by square</summary>
extern bitboard_t rookMasks[64];

/// <summary>Table stores attacker set for each square and each blocker set</summary>
extern bitboard_t rookTable[64][4096];

/// <summary>Array contains all masks for bishop moves indexed by square</summary>
extern bitboard_t bishopMasks[64];

/// <summary>Table stores attacker set for each square and each blocker set</summary>
extern bitboard_t bishopTable[64][1024];

/// <summary>
/// Generate a bitboard of blockers unique to the index.
/// </summary>
/// <param name="mask"></param>
/// <param name="i"></param>
/// <returns></returns>
bitboard_t getBlockers(bitboard_t mask, int i);

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
bitboard_t calculateRookMoves(int square, bitboard_t blockers);

/// <summary>
/// Calculate all possible bishop attacks from given square with blockers. This function is used
/// only to initialize the rook table and should not be used in any other case. 
/// Attacks all occupied pieces, check for correct color before generating attack moves.
/// </summary>
/// <param name="square">Start square</param>
/// <param name="blockers">Mask of blockers</param>
/// <returns></returns>
bitboard_t calculateBishopMoves(int square, bitboard_t blockers);

/// <summary>
/// Fast lookup for possible rook moves. Used in move generation. Possible rook moves and captures. 
/// Attacks all occupied pieces, check for correct color before generating attack moves.
/// </summary>
/// <param name="sq">From square</param>
/// <param name="blockers">Bitboard of currently occupied squares</param>
/// <returns>Bitboard of possible moves and captures</returns>
bitboard_t lookUpRookMoves(int sq, bitboard_t blockers);

/// <summary>
/// Fast lookup for possible bishop moves. Used in move generation. Possible bishop moves and captures. 
/// Attacks all occupied pieces, check for correct color before generating attack moves.
/// </summary>
/// <param name="sq">From square</param>
/// <param name="blockers">Bitboard of currently occupied squares</param>
/// <returns>Bitboard of possible moves and captures</returns>
bitboard_t lookUpBishopMoves(int sq, bitboard_t blockers);

#endif // !ATTACKS_H

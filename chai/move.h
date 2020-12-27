#pragma once

#include "types.h"

/*
Move format is bits in hex
0000 0000 0000 0000 0000 0000 0001 -> 0x1
0000 0000 0000 0000 0000 0000 1111 -> 0xF

0000 0000 0000 0000 0000 0111 1111 -> From square bits
0000 0000 0000 0011 1111 1000 0000 -> To square bits >> 7
0000 0000 0011 1100 0000 0000 0000 -> captured piece >> 14
0000 0000 0100 0000 0000 0000 0000 -> ep bool
0000 0000 1000 0000 0000 0000 0000 -> pawn start
0000 1111 0000 0000 0000 0000 0000 -> Promoted piece >> 20
0001 0000 0000 0000 0000 0000 0000 -> castling bool 0x1000000
0010 0000 0000 0000 0000 0000 0000 -> prom flag 0x2000000
*/

/// <summary> Move flags enPas move. </summary>
constexpr auto MFLAG_EP = 0x40000;

/// <summary> Move flags pawn start (double push) move. </summary>
constexpr auto MFLAG_PS = 0x80000;

/// <summary> Move flags castling move. </summary>
constexpr auto MFLAG_CAS = 0x1000000;

/// <summary> Checks, if EP flag is set. </summary>
constexpr auto MCHECK_EP = 0x40000;

/// <summary> Checks, if a piece is captured. </summary>
constexpr auto MCHECK_CAP = 0x3C000;

/// <summary> Checks, if move is a promotion. </summary>
constexpr auto MCHECK_PROM = 0xF00000;

/// <summary> Checks, if move is a promotion or capture. </summary>
constexpr auto MCHECK_PROMCAP = MCHECK_PROM | MCHECK_CAP;

/// <summary> Checks, if move is castling. </summary>
constexpr auto MCHECK_CAS = 0x1000000;

/// <summary>
/// Serialize a move into bit move.
/// </summary>
/// <param name="from">From square</param>
/// <param name="to">To square</param>
/// <param name="captured">Captured piece, 0 if none</param>
/// <param name="promoted">Promoting piece, 0 if none</param>
/// <param name="flag">EP, PS, CA flag (OR them together if necessary)</param>
/// <returns></returns>
inline move_t serializeMove(int from, int to, int captured, int promoted, int flag) {
	move_t move = 0;
	move |= from;
	move |= (to << 7);
	move |= (captured << 14);
	move |= (promoted << 20);
	move |= flag;

	return move;
}

/// <summary>
/// Get the fromSq of a serialized move.
/// </summary>
/// <param name="move">Serialized move</param>
/// <returns>From square</returns>
inline int fromSq(move_t move) {
	return move & 0x7F;
}

/// <summary>
/// Get the toSq of a serialized move.
/// </summary>
/// <param name="move">Serialized move</param>
/// <returns>To square</returns>
inline int toSq(move_t move) {
	return (move >> 7) & 0x7F;
}

/// <summary>
/// Get the captured piece of a serialized move.
/// </summary>
/// <param name="move">Serialized move</param>
/// <returns>Captured piece or 0 if none</returns>
inline int capPiece(move_t move) {
	return (move >> 14) & 0xF;
}

/// <summary>
/// Get the promoted piece of a serialized move.
/// </summary>
/// <param name="move">Serializedm move</param>
/// <returns>Promoted piece or 0 if none</returns>
inline int promPiece(move_t move) {
	return (move >> 20) & 0xF;
}



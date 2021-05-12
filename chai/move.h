#pragma once

#include "types.h"
//#include "board.h"

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

/*
16 Bit encoding:
0000 0000 0011 1111 from square
0000 1111 1100 1111 to square
0001 0000 0000 0000 n prom
0010 0000 0000 0000 b prom
0100 0000 0000 0000 r prom
1000 0000 0000 0000 q prom
*/

///**
// * Move flags enPas move.
// */
//constexpr auto MFLAG_EP = 0x40000;
//
///**
// * Move flags pawn start (double push) move.
// */
//constexpr auto MFLAG_PS = 0x80000;
//
///**
// * Move flags castling move.
// */
//constexpr auto MFLAG_CAS = 0x1000000;
//
///**
// * Checks, if EP flag is set.
// */
//constexpr auto MCHECK_EP = 0x40000;
//
///**
// * Checks, if a piece is captured.
// */
//constexpr auto MCHECK_CAP = 0x3C000;
//
///**
// * Checks, if move is a promotion.
// */
//constexpr auto MCHECK_PROM = 0xF00000;
//
///**
// * Checks, if move is a promotion or capture.
// */
//constexpr auto MCHECK_PROM_OR_CAP = MCHECK_PROM | MCHECK_CAP;
//
///**
// * Checks, if move is castling.
// */
//constexpr auto MCHECK_CAS = 0x1000000;

/**
 * Serialize a move into bit move.
 *
 * @param  from	    From square.
 * @param  to	    To square.
 * @param  captured Captured piece, 0 if none.
 * @param  promoted Promoting piece, 0 if none.
 * @param  flag	    EP, PS, CA flag (OR them together if necessary)
 *
 * @returns A move_t.
 */
 //inline move_t serializeMove(int from, int to, int captured, int promoted, int flag) {
 //	move_t move = 0;
 //	move |= from;
 //	move |= (to << 7);
 //	move |= (captured << 14);
 //	move |= (promoted << 20);
 //	move |= flag;
 //
 //	return move;
 //}

 /**
  * Get the fromSq of a serialized move.
  *
  * @param  move Serialized move.
  *
  * @returns From square.
  */
  //inline int fromSq(move_t move) {
	  //return move & sqBitMask;
  //}

  /**
   * Get the toSq of a serialized move.
   *
   * @param  move Serialized move.
   *
   * @returns To square.
   */
   //inline int toSq(move_t move) {
	   //return (move >> 6) & sqBitMask;
   //}

   /**
	* Get the captured piece of a serialized move.
	*
	* @param  move Serialized move.
	*
	* @returns Captured piece or 0 if none.
	*/
	//inline int capPiece(move_t move) {
	//	return (move >> 14) & 0xF;
	//}

	/**
	 * Get the promoted piece of a serialized move.
	 *
	 * @param  move Serializedm move.
	 *
	 * @returns Promoted piece or 0 if none.
	 */
	 //inline int promPiece(move_t move) {
	 //	return (move >> 20) & 0xF;
	 //}




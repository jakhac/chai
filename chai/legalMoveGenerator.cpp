#include "legalMoveGenerator.h"

U64 xrayRookAttacks(U64 occ, U64 blockers, int rookSq) {
	U64 attacks = lookUpRookMoves(rookSq, occ);
	blockers &= attacks;

	//U64 t = occ ^ blockers;

	U64 ret = attacks ^ lookUpRookMoves(rookSq, occ ^ blockers);
	printBitBoard(&ret);

	return ret;
}

void pins(Board b, U64 occ, U64 own, int kingSquare) {

	//U64 opRQ = b.getPieces(ROOK, b.side^1) | b.getPieces(QUEEN, b.side^1);

	//int pinned = 0;
	//U64 pinner = xrayRookAttacks(occ, own, kingSquare) & opRQ;
	//while (pinner) {
	//	int sq = bitscanForward(pinner);
	//	pinned |= obstructed(sq, kingSquare) & own;
	//	pinner &= pinner - 1;
	//}

}


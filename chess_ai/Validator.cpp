#include "Validator.h"

//int sqOnBoard(int sq) {
//	return *fileBoard[sq] == OFFBOARD ? 0 : 1;
//}

int sideValid(const int side) {
	return (side == WHITE || side == BLACK) ? 1 : 0;
}

int fileRankValid(const int fr) {
	return (fr >= 0 && fr <= 7) ? 1 : 0;
}

int pieceValidEmpty(const int pce) {
	return (pce >= EMPTY && pce <= k) ? 1 : 0;
}

int pieceValid(const int pce) {
	return (pce >= P && pce <= k) ? 1 : 0;
}
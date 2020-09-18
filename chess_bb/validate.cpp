#include "validate.h"

int squareOnBoard(int sq) {
	return ((sq >= 0) && (sq <= 64));
}

int pieceValid(int piece) {
	return ((piece >= 1) && (piece <= 13));
}
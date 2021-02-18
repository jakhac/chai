#include "validate.h"

int squareOnBoard(int sq) {
	return ((sq >= 0) && (sq <= 64));
}

int pieceValid(int piece) {
	return ((piece >= 1) && (piece <= 13));
}

bool fileValid(int file) {
	return (file >= 0) && (file <= 7);
}

bool pseudoValidBitMove(move_t move) {
	if (move != NULL_MOVE && move != NO_MOVE) {
		return true;
	}

	return false;
}

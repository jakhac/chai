#include "timeMan.h"

const int pieceValuesStd[7] = { 0, 1, 3, 3, 5, 9, 0 };

int totalMaterial(board_t* b) {
    int value = 0;
	for (int i = 1; i < 7; i++) {
		value += popCount(b->pieces[i]) * pieceValuesStd[i];
	}

    return value;
}

int remainingHalfMoves(board_t* b) {
    int mat = totalMaterial(b);

    if (mat < 20) return mat + 10;
    if (mat <= 60) return ((3./8.)*(float)mat) + 22;
    return ((5./4.)*(float)mat) - 30;
}

// Gaussian distribution. Peak at 25 with value 0.22
static int middleGameFactor(int moveNumber) {
    return 0.22 * exp(- (pow(moveNumber - 20, 2)) / (200));
}

// Time is allocated in three steps:
//      1) Estimate remaining full-moves based on material.
//      2) Equally distribute remaining time
//      3) Time in middle game is slightly increased
int allocateTime(board_t* b, int timeLeft, int inc) {
        // Catch low remaining time
		if (timeLeft <= 2000) {
            return (inc / 8);
		} else if (timeLeft <= 3500) {
            return (inc / 4);
        }
// position startpos moves d2d4 g8f6 c2c4 g7g6 b1c3 f8g7 e2e4 d7d6 g1f3 e8g8 c1e3 e7e5 d4e5 d6e5 d1d8 f8d8 c3d5 d8d7 f1d3 b8a6 d5f6
// go wtime 20921 btime 22041 winc 300 binc 300

        // 1)
        int movesLeft = remainingHalfMoves(b) / 2;

        // 2)
        int timeToAllocate = (timeLeft / movesLeft) - 50;

        // 3)
        timeToAllocate *= (1 + middleGameFactor(b->halfMoves/2));

        cout << "TimeLeft=" << timeLeft 
             << " estMovesLeft=" << movesLeft 
             << " halfMovesPlayer=" << (b->halfMoves/2)
             << " middleGameFactor=" << 1 + middleGameFactor(b->halfMoves/2)
             << " allocatedTime " << timeToAllocate
             << endl;
        
		Assert(timeToAllocate > 0);
        return std::min(timeToAllocate, timeLeft - 100);
}

bool isTimeLeft(search_t* s) {
    return getTimeMs() < (s->startTime + s->allocatedTime);
}
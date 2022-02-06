#include "timeMan.h"

const int pieceValuesPawns[7] = { 0, 1, 3, 3, 5, 9, 0 };

int totalMaterial(Board* b) {

    int value = 0;
    for (int i = 1; i < 7; i++) {
        value += popCount(b->pieces[i]) * pieceValuesPawns[i];
    }

    return value;
}

int remainingHalfMoves(Board* b) {

    int mat = totalMaterial(b);

    if (mat < 20)
        return mat + 10;

    if (mat <= 60)
        return ((3./8.)*(float)mat) + 22;

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
int allocateTime(Board* b, int timeLeft, int inc) {

        // Catch low remaining time
        if (timeLeft <= 2000) {
            return (inc / 8);
        } else if (timeLeft <= 3500) {
            return (inc / 4);
        }

        // 1)
        int movesLeft = remainingHalfMoves(b) / 2;

        // 2)
        int timeToAllocate = (timeLeft / movesLeft) - 50;

        // 3)
        timeToAllocate *= (1 + middleGameFactor(b->halfMoves/2));

#ifdef INFO
        cout << "TimeLeft=" << timeLeft 
             << " estMovesLeft=" << movesLeft 
             << " halfMovesPlayer=" << (b->halfMoves/2)
             << " middleGameFactor=" << 1 + middleGameFactor(b->halfMoves/2)
             << " allocatedTime " << timeToAllocate
             << endl;
#endif // INFO
        
        Assert(timeToAllocate > 0);
        return std::min(timeToAllocate, timeLeft - 100);
}

bool isTimeLeft(Instructions* instr) {
    return getTimeMs() < (instr->startTime + instr->allocatedTime);
}
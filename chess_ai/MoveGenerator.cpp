#include "MoveGenerator.h"

/*
Adds a quiet move into moveList and increments counter.
*/
void MoveGenerator::addQuietMove(const Board* b, int move) {
	// new moves are initialized with a score of 0
	//moveList[moveCounter] = Move(move, 0);
	moveCounter++;
}

/*
Adds a capture move into moveList and increments counter.
*/
void MoveGenerator::addCaptureMove(const Board* b, int move) {
	//moveList[moveCounter] = Move(move, 0);
	moveCounter++;
}

/*
Adds a en pas move into moveList and increments counter.
*/
void MoveGenerator::addEnPasMove(const Board* b, int move) {
	//moveList[moveCounter] = Move(move, 0);
	//moveList.insert(moveList.begin(), Move(move, 0));
	moveCounter++;
}

void MoveGenerator::generateAllMoves(const Board* b) {
	moveCounter = 0;

}

void MoveGenerator::printMoveList(const Move* mL)  {
	for (int i = 0; i < moveCounter; i++) {
		int move = mL[i].move;
		int score = mL[i].score;

		std::cout << i << ". Move " << move << " (score: " << score << ")" << endl;
	}
	std::cout << "Total moves: " << moveCounter << endl;
}



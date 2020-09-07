#include "MoveGenerator.h"

/*
Adds a quiet move into moveList and increments counter.
*/
void MoveGenerator::addQuietMove(const Board* b, int move) {
	// new moves are initialized with a score of 0
	moveList[moveCounter] = Move(move, 0);
	moveCounter++;
}

/*
Adds a capture move into moveList and increments counter.
*/
void MoveGenerator::addCaptureMove(const Board* b, int move) {
	moveList[moveCounter] = Move(move, 0);
	moveCounter++;
}

/*
Adds a en pas move into moveList and increments counter.
*/
void MoveGenerator::addEnPasMove(const Board* b, int move) {
	moveList[moveCounter] = Move(move, 0);
	moveCounter++;
}

/*
Prints the current moves list in console. (Add in io?)
*/
void MoveGenerator::printMoveList(Board* b) {
	for (int i = 0; i < moveCounter; i++) {
		std::cout << "Move " << i << ": " <<  b->printMove(moveList[i].move) << endl;
	}
	std::cout << "Total moves: " << moveCounter << endl;

}

/*
Generate all moves for referenced board.
*/
void MoveGenerator::generateAllMoves(Board* b) {

	moveCounter = 0;

	//return;

	ASSERT(b->checkBoard(b));

	int piece = EMPTY;
	int side = b->side;
	int sq = 0, t_sq = 0;
	int i = 0;

	if (side == WHITE) {
		// loop through all white pawns on board
		for (i = 0; i < b->pieceNumber[P]; i++) {
			sq = b->pieceList[P][i];

			std::cout << "Pawn on square " << sq << endl;

			ASSERT(b->sqOnBoard(sq));

			// pawn non capture moves
			if (b->pieces[sq + 10] == EMPTY) {
				addWhitePawnMove(b, sq, sq+10);
				if (b->rankBoard[sq] == RANK_2 && b->pieces[sq + 20] == EMPTY) {
					addQuietMove(b, MOVE(sq, sq+20, EMPTY, EMPTY, MFLAGPS));
				}
			}

			// pawn capture move diagonals
			if (b->sqOnBoard(sq + 9) && b->pieceCol[b->pieces[sq + 9]] == BLACK) {
				addWhitePawnCapMove(b, sq, sq+9, b->pieces[sq + 9]);
			}
			if (b->sqOnBoard(sq + 11) && b->pieceCol[b->pieces[sq + 11]] == BLACK) {
				addWhitePawnCapMove(b, sq, sq + 11, b->pieces[sq + 11]);
			}

			// pawn en passant captures
			if (sq + 9 == b->enPas) {
				addCaptureMove(b, MOVE(sq, sq + 9, EMPTY, EMPTY, MFLAGEP));
			}
			if (sq + 11 == b->enPas) {
				addCaptureMove(b, MOVE(sq, sq + 11, EMPTY, EMPTY, MFLAGEP));
			}


		}
	}

}

/*
Add all white pawn capture moves.
*/
void MoveGenerator::addWhitePawnCapMove(Board* b, const int from, const int to, const int cap) {
	if (b->fileBoard[from] == RANK_7) {
		// promote if possible
		addCaptureMove(b, MOVE(from, to, cap, Q, 0));
		addCaptureMove(b, MOVE(from, to, cap, R, 0));
		addCaptureMove(b, MOVE(from, to, cap, N, 0));
		addCaptureMove(b, MOVE(from, to, cap, B, 0));
	}
	else {
		addCaptureMove(b, MOVE(from, to, cap, EMPTY, 0));
	}
}

/*
Add all white pawn moves.
*/
void MoveGenerator::addWhitePawnMove(Board* b, const int from, const int to) {
	if (b->fileBoard[from] == RANK_7) {
		// promote if possible
		addQuietMove(b, MOVE(from, to, EMPTY, Q, 0));
		addQuietMove(b, MOVE(from, to, EMPTY, R, 0));
		addQuietMove(b, MOVE(from, to, EMPTY, B, 0));
		addQuietMove(b, MOVE(from, to, EMPTY, N, 0));
	}
	else {
		addQuietMove(b, MOVE(from, to, EMPTY, EMPTY, 0));
	}
}



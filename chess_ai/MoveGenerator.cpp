#include "MoveGenerator.h"

/*
Adds a quiet move into moveList and increments counter.
*/
void MoveGenerator::addQuietMove(int move) {
	moveList[moveCounter] = Move(move, 0);
	moveCounter++;
}

/*
Adds a capture move into moveList and increments counter.
*/
void MoveGenerator::addCaptureMove(int move) {
	moveList[moveCounter] = Move(move, 0);
	moveCounter++;
}

/*
Adds a en pas move into moveList and increments counter.
*/
void MoveGenerator::addEnPasMove(int move) {
	moveList[moveCounter] = Move(move, 0);
	moveCounter++;
}

/*
Add all white pawn capture moves.
*/
void MoveGenerator::addWhitePawnCapMove(Board* board, const int from, const int to, const int cap) {

	ASSERT(pieceValidEmpty(cap));
	ASSERT(board->sqOnBoard(from));
	ASSERT(board->sqOnBoard(to));

	if (board->rankBoard[from] == RANK_7) {
		// promote if possible
		addCaptureMove(MOVE(from, to, cap, Q, 0));
		addCaptureMove(MOVE(from, to, cap, R, 0));
		addCaptureMove(MOVE(from, to, cap, N, 0));
		addCaptureMove(MOVE(from, to, cap, B, 0));
	}
	else {
		addCaptureMove(MOVE(from, to, cap, EMPTY, 0));
	}
}

/*
Add all white pawn moves.
*/
void MoveGenerator::addWhitePawnMove(Board* board, const int from, const int to) {

	ASSERT(board->sqOnBoard(from));
	ASSERT(board->sqOnBoard(to));

	if (board->rankBoard[from] == RANK_7) {
		// promote if possible
		addQuietMove(MOVE(from, to, EMPTY, Q, 0));
		addQuietMove(MOVE(from, to, EMPTY, R, 0));
		addQuietMove(MOVE(from, to, EMPTY, B, 0));
		addQuietMove(MOVE(from, to, EMPTY, N, 0));
	}
	else {
		addQuietMove(MOVE(from, to, EMPTY, EMPTY, 0));
	}
}

/*
Add all black pawn capture moves.
*/
void MoveGenerator::addBlackPawnCapMove(Board* board, const int from, const int to, const int cap) {

	ASSERT(pieceValidEmpty(cap));
	ASSERT(board->sqOnBoard(from));
	ASSERT(board->sqOnBoard(to));

	if (board->rankBoard[from] == RANK_2) {
		// promote if possible
		addCaptureMove(MOVE(from, to, cap, q, 0));
		addCaptureMove(MOVE(from, to, cap, r, 0));
		addCaptureMove(MOVE(from, to, cap, n, 0));
		addCaptureMove(MOVE(from, to, cap, b, 0));
	}
	else {
		addCaptureMove(MOVE(from, to, cap, EMPTY, 0));
	}
}

/*
Add all black pawn moves.
*/
void MoveGenerator::addBlackPawnMove(Board* board, const int from, const int to) {

	ASSERT(board->sqOnBoard(from));
	ASSERT(board->sqOnBoard(to));

	if (board->rankBoard[from] == RANK_2) {
		// promote if possible
		addQuietMove(MOVE(from, to, EMPTY, q, 0));
		addQuietMove(MOVE(from, to, EMPTY, r, 0));
		addQuietMove(MOVE(from, to, EMPTY, b, 0));
		addQuietMove(MOVE(from, to, EMPTY, n, 0));
	}
	else {
		addQuietMove(MOVE(from, to, EMPTY, EMPTY, 0));
	}
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

	ASSERT(b->checkBoard(b));

	int piece = EMPTY;
	int side = b->side;
	int sq = 0, t_sq = 0;
	int i = 0, j = 0;
	int dir = 0, pieceIndex = 0;


	/* pls refactor me */
	if (side == WHITE) {
		std::cout << "\nGenerate white moves." << endl;

		// loop through all white pawns on board
		for (i = 0; i < b->pieceNumber[P]; i++) {
			sq = b->pieceList[P][i];

			ASSERT(b->sqOnBoard(sq));

			// pawn non capture moves
			if (b->pieces[sq + 10] == EMPTY) {
				addWhitePawnMove(b, sq, sq+10);
				if (b->rankBoard[sq] == RANK_2 && b->pieces[sq + 20] == EMPTY) {
					addQuietMove(MOVE(sq, sq+20, EMPTY, EMPTY, MFLAGPS));
				}
			}

			// pawn capture move diagonals
			if (b->sqOnBoard(sq + 9) && pieceCol[b->pieces[sq + 9]] == BLACK) {
				addWhitePawnCapMove(b, sq, sq+9, b->pieces[sq + 9]);
			}
			if (b->sqOnBoard(sq + 11) && pieceCol[b->pieces[sq + 11]] == BLACK) {
				addWhitePawnCapMove(b, sq, sq + 11, b->pieces[sq + 11]);
			}

			// pawn en passant captures
			if (sq + 9 == b->enPas) {
				addCaptureMove(MOVE(sq, sq + 9, EMPTY, EMPTY, MFLAGEP));
			}
			if (sq + 11 == b->enPas) {
				addCaptureMove(MOVE(sq, sq + 11, EMPTY, EMPTY, MFLAGEP));
			}
		}

		// castle moves
		if (b->castlePermission & K_CASTLE) {
			if (b->pieces[F1] == EMPTY && b->pieces[G1] == EMPTY) {
				if (!b->squareAttacked(E1, BLACK, b) && !b->squareAttacked(F1, BLACK, b)) {
					addQuietMove(MOVE(E1, G1, EMPTY, EMPTY, MFLAGCA));
				}
			}
		}

		if (b->castlePermission & Q_CASTLE) {
			if (b->pieces[D1] == EMPTY && b->pieces[C1] == EMPTY && b->pieces[B1] == EMPTY) {
				if (!b->squareAttacked(E1, BLACK, b) && !b->squareAttacked(D1, BLACK, b)) {
					addQuietMove(MOVE(E1, C1, EMPTY, EMPTY, MFLAGCA));
				}
			}
		}

	}
	else {
		std::cout << "\nGenerate black moves." << endl;

		// loop through all black pawns on board
		for (i = 0; i < b->pieceNumber[p]; i++) {
			sq = b->pieceList[p][i];


			if (b->pieces[sq - 10] == EMPTY) {
				addBlackPawnMove(b, sq, sq - 10);
				if (b->rankBoard[sq] == RANK_7 && b->pieces[sq - 20] == EMPTY) {
					addQuietMove(MOVE(sq, sq - 20, EMPTY, EMPTY, MFLAGPS));
				}
			}

			// black pawn capture move diagonals
			if (b->sqOnBoard(sq - 9) && pieceCol[b->pieces[sq - 9]] == WHITE) {
				addBlackPawnCapMove(b, sq, sq - 9, b->pieces[sq - 9]);
			}
			if (b->sqOnBoard(sq - 11) && pieceCol[b->pieces[sq - 11]] == WHITE) {
				addBlackPawnCapMove(b, sq, sq - 11, b->pieces[sq - 11]);
			}

			// black pawn en passant captures
			if (sq - 9 == b->enPas) {
				addCaptureMove(MOVE(sq, sq - 9, EMPTY, EMPTY, MFLAGEP));
			}
			if (sq - 11 == b->enPas) {
				addCaptureMove(MOVE(sq, sq - 11, EMPTY, EMPTY, MFLAGEP));
			}
		}

		
		// castle moves
		if (b->castlePermission & k_CASTLE) {
			if (b->pieces[F8] == EMPTY && b->pieces[G8] == EMPTY) {
				if (!b->squareAttacked(E8, WHITE, b) && !b->squareAttacked(F8, WHITE, b)) {
					addQuietMove(MOVE(E8, C8, EMPTY, EMPTY, MFLAGCA));
				}
			}
		}

		if (b->castlePermission & q_CASTLE) {
			if (b->pieces[D8] == EMPTY && b->pieces[C8] == EMPTY && b->pieces[B8] == EMPTY) {
				if (!b->squareAttacked(E8, WHITE, b) && !b->squareAttacked(D8, WHITE, b)) {
					addQuietMove(MOVE(E8, C8, EMPTY, EMPTY, MFLAGCA));
				}
			}
		}
	}

	/* Loop sliding moves */
	pieceIndex = slideIndex[side];
	piece = loopSlidingPiece[pieceIndex++];

	while (piece != 0) {
		ASSERT(pieceValid(piece));

		for (i = 0; i < b->pieceNumber[piece]; i++) {
			sq = b->pieceList[piece][i];
			ASSERT(b->sqOnBoard(sq));

			for (j = 0; j < numDir[piece]; ++j) {
				dir = pieceDir[piece][j];
				t_sq = sq + dir;

				while (b->sqOnBoard(t_sq)) {
					if (b->pieces[t_sq] != EMPTY) {
						if (pieceCol[b->pieces[t_sq]] == (side ^ 1)) {
							//printf("\t\tCapture move on %s\n", b->getSquareStr(t_sq));
							addCaptureMove(MOVE(sq, t_sq, b->pieces[t_sq], EMPTY, 0));
						}
						break;
					}
					//printf("\t\tNon-Capture move on %s\n", b->getSquareStr(t_sq));
					addQuietMove(MOVE(sq, t_sq, EMPTY, EMPTY, 0));
					t_sq += dir;
				}
			}
		}

		piece = loopSlidingPiece[pieceIndex++];
	}


	/* Loop non sliding moves */
	pieceIndex = nonSlideIndex[side];
	piece = loopNonSlidingPiece[pieceIndex++];

	while (piece != 0) {
		ASSERT(pieceValid(piece));

		for (i = 0; i < b->pieceNumber[piece]; i++) {
			sq = b->pieceList[piece][i];
			ASSERT(b->sqOnBoard(sq));

			for (j = 0; j < numDir[piece]; ++j) {
				dir = pieceDir[piece][j];
				t_sq = sq + dir;

				if (!b->sqOnBoard(t_sq)) {
					continue;
				}

				if (b->pieces[t_sq] != EMPTY) {
					if (pieceCol[b->pieces[t_sq]] == (side ^ 1)) {
						//printf("\t\tCapture move on %s\n", b->getSquareStr(t_sq));
						addCaptureMove(MOVE(sq, t_sq, b->pieces[t_sq], EMPTY, 0));
					}
					continue;
				}
				//printf("\t\tNon-Capture move on %s\n", b->getSquareStr(t_sq));
				addQuietMove(MOVE(sq, t_sq, EMPTY, EMPTY, 0));
			}
		}
	
		piece = loopNonSlidingPiece[pieceIndex++];
	}
}



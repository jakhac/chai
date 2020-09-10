#include "Main.h"


int main() {

	std::cout << "ChessAI" << std::endl;

	Board b;
	b.init();

	string fen = MOVES_48;

	char* fen_ = new char[fen.size() + 1];
	std::copy(fen.begin(), fen.end(), fen_);
	fen_[fen.size()] = '\0';

	// parse fen and print board
	b.parseFen(fen_, &b);

	//b.printBoard(&b);
	//b.checkBoard(&b);

	MoveGenerator moveGenerator;
	moveGenerator.generateAllMoves(&b);

	moveGenerator.printMoveList(&b);

	return 0;
}


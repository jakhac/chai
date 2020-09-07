#include "Main.h"


int main() {

	std::cout << "ChessAI" << std::endl;

	Board b;
	b.init();

	//U64 bb = 0ULL;
	//bb |= (1ULL << b.SQ_64(D2));
	//bb |= (1ULL << b.SQ_64(A1));
	//b.printBitBoard(bb);

	//string fen = STARTING_FEN;

	//char* fen_ = new char[fen.size() + 1];
	//std::copy(fen.begin(), fen.end(), fen_);
	//fen_[fen.size()] = '\0';

	//// parse fen and print board
	//b.parseFen(fen_, &b);
	//b.printBoard(&b);
	//b.checkBoard(&b);

	MoveGenerator moveGenerator;

	return 0;
}


#include "Main.h"


int main() {

	std::cout << "ChessAI" << std::endl;

	Board b;
	b.init();

	string fen = STARTING_FEN;

	char* fen_ = new char[fen.size() + 1];
	std::copy(fen.begin(), fen.end(), fen_);
	fen_[fen.size()] = '\0';

	// parse fen and print board
	b.parseFen(fen_, &b);

	int m = MOVE(A6, A7, EMPTY, EMPTY, EMPTY);

	Perft perft;
	perft.perftRoot(b, 2);


	//b.printBoard(&b);
	//for (int i = 0; i < numMoves; i++) {
	//	cout << "\n----------------\n";

	//	int move = moves[i].move;

	//	// if push returns false -> move is illegal
	//	if (!b.push(&b, move)) continue;

	//	cout << "Make move " << b.printMove(move) << endl;
	//	b.printBoard(&b);

	//	b.pop(&b);
	//	cout << "\nUnmake move " << b.printMove(move) << endl;
	//	b.printBoard(&b);

	//	getchar();
	//}

	return 0;
}


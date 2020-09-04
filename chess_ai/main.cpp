#include "Main.h"


int main() {

	std::cout << "ChessAI" << std::endl;

	Board b;
	b.init();

	//U64 bb = 0ULL;
	//bb |= (1ULL << b.squareConstantsTo64(D2));
	//bb |= (1ULL << b.squareConstantsTo64(A1));
	//b.printBitBoard(bb);

	//char* fen_ = new char[68 + 1];
	//std::copy(BUG_FEN.begin(), BUG_FEN.end(), fen_);
	//fen_[68] = '\0';

	string fen = STARTING_FEN;

	char* fen_ = new char[fen.size() + 1];
	std::copy(fen.begin(), fen.end(), fen_);
	fen_[fen.size()] = '\0';

	b.parseFen(fen_, &b);

	b.printBoard(&b);

	ASSERT(b.checkBoard(&b));

	//for (int i = 0; i < NUM_SQUARES; i++) {
	//	std::cout << b.pieces[i] << endl;
	//}

	//b.printBoard(&b);

	//b.updateListsMaterial(&b);


	return 0;
}




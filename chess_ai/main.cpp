#include "Main.h"


int main() {

	std::cout << "ChessAI" << std::endl;

	Board b;
	b.init();

	//U64 bb = 0ULL;
	//bb |= (1ULL << b.squareConstantsTo64(D2));
	//bb |= (1ULL << b.squareConstantsTo64(A1));
	//b.printBitBoard(bb);


	char* fen_ = new char[57];
	std::copy(STARTING_FEN.begin(), STARTING_FEN.end(), fen_);
	fen_[56] = '\0';


	b.parseFen(fen_, &b);

	//for (int i = 0; i < NUM_SQUARES; i++) {
	//	std::cout << b.pieces[i] << endl;
	//}


	b.printBoard(&b);

	return 0;
}




#include "syzygy.h"

void initEGTB(const char* tbPath) {
	if (!tb_init(tbPath)) {
		cout << "TB init failed." << endl;
		Assert(false);
		exit(1);
	}
	cout << "TB max=" << TB_LARGEST << endl;
}

void freeEGTB() {
#ifdef USE_EGTB
	tb_free();
#endif // USE_EGTB
}

int probeTB(board_t* b) {
	// Do not allow TB probing in root nodes
	// TB only works if both players have no castling rights
	if (b->ply == 0
		|| b->castlePermission != 0
		|| b->fiftyMove > 0
		|| (unsigned) popCount(b->occupied) > TB_LARGEST)
		return TB_RESULT_FAILED;

	int enPas = b->enPas == DEFAULT_EP_SQ ? 0 : 1;
	int stm = b->stm == chai::WHITE ? 1 : 0;

	//return tb_probe_wdl(b->color[chai::WHITE],
	//					b->color[chai::BLACK],
	//					b->pieces[chai::KING],
	//					b->pieces[chai::QUEEN],
	//					b->pieces[chai::ROOK],
	//					b->pieces[chai::BISHOP],
	//					b->pieces[chai::KNIGHT],
	//					b->pieces[chai::PAWN],
	//					enPas,
	//					stm);
	return tb_probe_wdl(b->color[chai::WHITE],
						b->color[chai::BLACK],
						b->pieces[chai::KING],
						b->pieces[chai::QUEEN],
						b->pieces[chai::ROOK],
						b->pieces[chai::BISHOP],
						b->pieces[chai::KNIGHT],
						b->pieces[chai::PAWN],
						0,
						0,
						enPas,
						stm);
}

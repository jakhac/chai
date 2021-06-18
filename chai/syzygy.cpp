#include "syzygy.h"

void probe(board_t* b) {
	bool egtbStatus = tb_init("../syzygy");
	cout << "egtbStatus " << egtbStatus
		<< " TB max " << TB_LARGEST
		<< endl;

	unsigned int wdl = tb_probe_wdl(b->color[chai::WHITE],
									b->color[chai::BLACK],
									b->pieces[chai::KING],
									b->pieces[chai::QUEEN],
									b->pieces[chai::ROOK],
									b->pieces[chai::BISHOP],
									b->pieces[chai::KNIGHT],
									b->pieces[chai::PAWN],
									0,
									0,
									0,
									1);


	switch (wdl) {
		case TB_LOSS:
			cout << "TB LOSS" << endl;
			break;
		case TB_BLESSED_LOSS:
			cout << "TB TB_BLESSED_LOSS" << endl;
			break;
		case TB_DRAW:
			cout << "TB TB_DRAW" << endl;
			break;
		case TB_CURSED_WIN:
			cout << "TB TB_CURSED_WIN" << endl;
			break;
		case TB_WIN:
			cout << "TB TB_WIN" << endl;
			break;
		case TB_RESULT_FAILED:
			cout << "TB TB_RESULT_FAILED" << endl;
			break;
		default:
			break;
	}

	tb_free();
}

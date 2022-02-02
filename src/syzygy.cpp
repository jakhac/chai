#include "syzygy.h"

namespace EGTB {
	
void init(const char* tbPath) {
	if (!tb_init(tbPath)) {
		cout << "TB init failed." << endl;
		Assert(false);
		exit(1);
	}
#ifdef INFO
	cout << "TB " << std::setw(5) << TB_LARGEST << endl << endl;
#endif // INFO
}

void freeEGTB() {
	tb_free();
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
	int stm   = b->stm == WHITE ? 1 : 0;

	return tb_probe_wdl(b->color[WHITE],
						b->color[BLACK],
						b->pieces[cKING],
						b->pieces[cQUEEN],
						b->pieces[cROOK],
						b->pieces[cBISHOP],
						b->pieces[cKNIGHT],
						b->pieces[cPAWN],
						0,
						0,
						enPas,
						stm);
}


} // namespace EGTB
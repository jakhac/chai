#include "syzygy.h"

int probeTB(board_t* b) {
	// Do not allow TB probing in root nodes
	// TB only works if both players have no castling rights
	if (b->ply == 0
		|| b->castlePermission != 0
		|| b->fiftyMove != 0
		|| popCount(b->occupied) > TB_LARGEST)
		return TB_RESULT_FAILED;

	return tb_probe_wdl(b->color[chai::WHITE],
						b->color[chai::BLACK],
						b->pieces[chai::KING],
						b->pieces[chai::QUEEN],
						b->pieces[chai::ROOK],
						b->pieces[chai::BISHOP],
						b->pieces[chai::KNIGHT],
						b->pieces[chai::PAWN],
						b->enPas == DEFAULT_EP_SQ ? 0 : 1,
						b->stm == chai::WHITE ? 1 : 0);
}

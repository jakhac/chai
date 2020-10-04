#pragma once

#include "moveGenerator.h"

class Perft {

public:

	long leafNodes = 0;

	const string fen_48 = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
	const string fen_18 = "8/PPP4k/8/8/8/8/4Kppp/8 w - - 0 1";
	const string fen_pawn_cap = "rnbqkbnr/p1pppppp/8/1p6/P7/8/1PPPPPPP/RNBQKBNR w KQkq b6 0 1";

	long perftRoot(Board b, int depth);
	void perft(Board b, int depth);

};

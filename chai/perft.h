#pragma once

#include "moveGenerator.h"

class Perft {

public:

	long leafNodes = 0;

	const string fen_48 = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
	const string fen_18 = "8/PPP4k/8/8/8/8/4Kppp/8 w - - 0 1";
	const string fen_pawn_cap = "rnbqkbnr/p1pppppp/8/1p6/P7/8/1PPPPPPP/RNBQKBNR w KQkq b6 0 1";

	/// <summary>
	/// Initial call for perfting to given depth. Divides initial moves
	/// and their leaf nodes.
	/// </summary>
	/// <param name="b">Board</param>
	/// <param name="depth">Perft depth</param>
	/// <returns>Number of counted leaf nodes</returns>
	long perftRoot(Board b, int depth);

	/// <summary>
	/// Recursive perft call counts all nodes till depth.
	/// </summary>
	/// <param name="b"></param>
	/// <param name="depth"></param>
	void perft(Board b, int depth);

	/// <summary>
	/// Faster perfting to given depth. Add up length of generated moves and saves time at last depth.
	/// Does work for legal move generator only.
	/// </summary>
	/// <param name="b">Board to perft on</param>
	/// <param name="depth">Perft depth</param>
	U64 perftBulk(Board b, int depth);

};

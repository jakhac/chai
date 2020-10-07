#pragma once

#include <vector>

#include "board.h"
#include "attacks.h"
#include "move.h"

class LegalMoveGenerator {

public:

	vector <Move> _quietMoveList;
	vector <Move> _capMoveList;

	vector<Move> _getAllMoves(Board b);
	void _resetMoveLists();

	void _generateMoves(Board b);

	/* PAWN */
	void _addPawnMoves(Board b, const int side);
	void _whiteSinglePawnPush(Board b);
	void _blackSinglePawnPush(Board b);
	void _whiteDoublePawnPush(Board b);
	void _blackDoublePawnPush(Board b);
	void _whitePawnCaptures(Board b);
	void _blackPawnCaptures(Board b);

	/* KNIGHT */
	/// <summary>
	/// Generate all possible knight moves and add to quietMoveList.
	/// </summary>
	/// <param name="b">Board</param>
	/// <param name="side">Side to move</param>
	void _addKnightMoves(Board b, const int side);

	/// <summary>
	/// Generate all possible knight captures for given side and add to capMoveList.
	/// </summary>
	/// <param name="b">Board</param>
	/// <param name="side">Side to move</param>
	void _addKnightCaptures(Board b, const int side);

	/* KING */
	/// <summary>
	/// Generate all king moves for given side and add to quietMoveList.
	/// </summary>
	/// <param name="b">Board</param>
	/// <param name="side">Side to move</param>
	void _addKingMoves(Board b, const int side);

	/// <summary>
	/// Generate all white king moves. Kings cannot move into squares attacked by opposite king.
	/// King moves includes castling.
	/// </summary>
	/// <param name="board">Board</param>
	void _addWhiteKingMoves(Board b);

	/// <summary>
	/// Generate all black king moves. Kings cannot move into squares attacked by opposite king.
	/// King moves includes castling.
	/// </summary>
	/// <param name="board">Board</param>
	void _addBlackKingMoves(Board b);

	/// <summary>
	/// Generate king captures and add to capMoveList.
	/// </summary>
	/// <param name="b">Board</param>
	/// <param name="side">Side to move</param>
	void _addKingCaptures(Board b, const int side);

	/// <summary>
	/// Generate all white attacking king moves.
	/// </summary>
	/// <param name="board">Board</param>
	void _addWhiteKingCaptures(Board b); // TODO SQ ATTACKED

	/// <summary>
	/// Generate all black attacking king moves.
	/// </summary>
	/// <param name="board">Board</param>
	void _addBlackKingCaptures(Board b); // TODO SQ ATTACKED

	/// <summary>
	/// Generate all rook moves for given board and side and add to quietMoveList.
	/// </summary>
	/// <param name="b">Board to search moves on</param>
	/// <param name="side">Moving side</param>
	void _addRookMoves(Board b, const int side);

	/// <summary>
	/// Generate all bishop moves for given board and side and add to quietMoveList.
	/// </summary>
	/// <param name="b">Board to search moves on</param>
	/// <param name="side">Moving side</param>
	void _addBishopMoves(Board b, const int side);

	/// <summary>
	/// Generate all rook captures for given board and side and add to capMoveList.
	/// </summary>
	/// <param name="b">Board to search captures on</param>
	/// <param name="side">Moving side</param>
	void _addRookCaptures(Board b, const int side);

	/// <summary>
	/// Generate all bishop captures for given board and side and add to capMoveList.
	/// </summary>
	/// <param name="b">Board to search captures on</param>
	/// <param name="side">Moving side</param>
	void _addBishopCaptures(Board b, const int side);

	/// <summary>
	/// Generate all queen moves for given board and side. Add moves to quietMoveList.
	/// </summary>
	/// <param name="b">Board</param>
	/// <param name="side">Side to move</param>
	void _addQueenMoves(Board b, const int side);

	/// <summary>
	/// Generate queen captures and add to capMoveList.
	/// </summary>
	/// <param name="b">Board</param>
	/// <param name="side">Side to move</param>
	void _addQueenCaptures(Board b, const int side);

	/// <summary>
	/// Print all generated moves.
	/// </summary>
	/// <param name="b">Board</param>
	void _printGeneratedMoves(Board b);
};




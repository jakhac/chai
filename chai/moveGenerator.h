#pragma once

#include <vector>

#include "board.h"
#include "attacks.h"
#include "move.h"

class MoveGenerator {

public:

	vector <Move> quietMoveList;
	vector <Move> capMoveList;

	vector<Move> getAllMoves();
	void resetMoveLists();

	void generateMoves(Board b);


	/* PAWN */
	void addPawnMoves(Board b, const int side);
	void whiteSinglePawnPush(Board b);
	void blackSinglePawnPush(Board b);
	void whiteDoublePawnPush(Board b);
	void blackDoublePawnPush(Board b);
	void whitePawnCaptures(Board b);
	void blackPawnCaptures(Board b);

	/* KNIGHT */
	/// <summary>
	/// Generate all possible knight moves and add to quietMoveList.
	/// </summary>
	/// <param name="b">Board</param>
	/// <param name="side">Side to move</param>
	void addKnightMoves(Board b, const int side);

	/// <summary>
	/// Generate all possible knight captures for given side and add to capMoveList.
	/// </summary>
	/// <param name="b">Board</param>
	/// <param name="side">Side to move</param>
	void addKnightCaptures(Board b, const int side);

	/* KING */
	/// <summary>
	/// Generate all king moves for given side and add to quietMoveList.
	/// </summary>
	/// <param name="b">Board</param>
	/// <param name="side">Side to move</param>
	void addKingMoves(Board b, const int side);

	/// <summary>
	/// Generate all white king moves. Kings cannot move into squares attacked by opposite king.
	/// King moves includes castling.
	/// </summary>
	/// <param name="board">Board</param>
	void addWhiteKingMoves(Board b); // TODO CASTLE PERM FUNC

	/// <summary>
	/// Generate all black king moves. Kings cannot move into squares attacked by opposite king.
	/// King moves includes castling.
	/// </summary>
	/// <param name="board">Board</param>
	void addBlackKingMoves(Board b); // TODO CASTLE PERM FUNC

	/// <summary>
	/// Generate king captures and add to capMoveList.
	/// </summary>
	/// <param name="b">Board</param>
	/// <param name="side">Side to move</param>
	void addKingCaptures(Board b, const int side);

	/// <summary>
	/// Generate all white attacking king moves.
	/// </summary>
	/// <param name="board">Board</param>
	void addWhiteKingCaptures(Board b); // TODO SQ ATTACKED

	/// <summary>
	/// Generate all black attacking king moves.
	/// </summary>
	/// <param name="board">Board</param>
	void addBlackKingCaptures(Board b); // TODO SQ ATTACKED

	/// <summary>
	/// Generate all rook moves for given board and side and add to quietMoveList.
	/// </summary>
	/// <param name="b">Board to search moves on</param>
	/// <param name="side">Moving side</param>
	void addRookMoves(Board b, const int side);

	/// <summary>
	/// Generate all bishop moves for given board and side and add to quietMoveList.
	/// </summary>
	/// <param name="b">Board to search moves on</param>
	/// <param name="side">Moving side</param>
	void addBishopMoves(Board b, const int side);

	/// <summary>
	/// Generate all rook captures for given board and side and add to capMoveList.
	/// </summary>
	/// <param name="b">Board to search captures on</param>
	/// <param name="side">Moving side</param>
	void addRookCaptures(Board b, const int side);

	/// <summary>
	/// Generate all bishop captures for given board and side and add to capMoveList.
	/// </summary>
	/// <param name="b">Board to search captures on</param>
	/// <param name="side">Moving side</param>
	void addBishopCaptures(Board b, const int side);

	/// <summary>
	/// Generate all queen moves for given board and side. Add moves to quietMoveList.
	/// </summary>
	/// <param name="b">Board</param>
	/// <param name="side">Side to move</param>
	void addQueenMoves(Board b, const int side);

	/// <summary>
	/// Generate queen captures and add to capMoveList.
	/// </summary>
	/// <param name="b">Board</param>
	/// <param name="side">Side to move</param>
	void addQueenCaptures(Board b, const int side);

	/// <summary>
	/// Print all generated moves.
	/// </summary>
	/// <param name="b">Board</param>
	void printGeneratedMoves(Board b);
};




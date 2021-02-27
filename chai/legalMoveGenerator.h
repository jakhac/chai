//#pragma once
//
//#include <vector>
//
//#include "board.h"
//#include "attacks.h"
//#include "moveGenerator.h"
//#include "move.h"
//
//void _generateMoves(board_t* b, moveList_t* move_s);
//
///* Piece::PAWN */
//void _whiteSinglePawnPush(board_t* b, moveList_t* move_s);
//void _blackSinglePawnPush(board_t* b, moveList_t* move_s);
//void _whiteDoublePawnPush(board_t* b, moveList_t* move_s);
//
//void _blackDoublePawnPush(board_t* b, moveList_t* move_s);
//void _whitePawnCaptures(board_t* b, moveList_t* move_s);
//void _blackPawnCaptures(board_t* b, moveList_t* move_s);
//
///* Piece::KNIGHT */
///// <summary>
///// Generate all possible knight moves and add to quietMoveList.
///// </summary>
///// <param name="b">board_t</param>
///// <param name="side">Side to move</param>
//
//void _addKnightMoves(board_t* b, moveList_t* move_s);
//
///// <summary>
///// Generate all possible knight captures for given side and add to capMoveList.
///// </summary>
///// <param name="b">board_t</param>
///// <param name="side">Side to move</param>
//void _addKnightCaptures(board_t* b, moveList_t* move_s);
//
///* Piece::KING */
///// <summary>
///// Generate all king moves for given side and add to quietMoveList.
///// </summary>
///// <param name="b">board_t</param>
///// <param name="side">Side to move</param>
//void _addKingMoves(board_t* b, moveList_t* move_s);
//
///// <summary>
///// Generate all white king moves. Kings cannot move into squares attacked by opposite king.
///// King moves includes castling.
///// </summary>
///// <param name="board">board_t</param>
//void _addWhiteKingMoves(board_t* b, moveList_t* move_s);
//
///// <summary>
///// Generate all black king moves. Kings cannot move into squares attacked by opposite king.
///// King moves includes castling.
///// </summary>
///// <param name="board">board_t</param>
//void _addBlackKingMoves(board_t* b, moveList_t* move_s);
//
///// <summary>
///// Generate king captures and add to capMoveList.
///// </summary>
///// <param name="b">board_t</param>
///// <param name="side">Side to move</param>
//void _addKingCaptures(board_t* b, moveList_t* move_s);
//
///// <summary>
///// Generate all white attacking king moves.
///// </summary>
///// <param name="board">board_t</param>
//void _addWhiteKingCaptures(board_t b); // TODO SQ ATTACKED
//
///// <summary>
///// Generate all black attacking king moves.
///// </summary>
///// <param name="board">board_t</param>
//void _addBlackKingCaptures(board_t b); // TODO SQ ATTACKED
//
///// <summary>
///// Generate all rook moves for given board and side and add to quietMoveList.
///// </summary>
///// <param name="b">board_t to search moves on</param>
///// <param name="side">Moving side</param>
//void _addRookMoves(board_t* b, moveList_t* move_s);
//
///// <summary>
///// Generate all bishop moves for given board and side and add to quietMoveList.
///// </summary>
///// <param name="b">board_t to search moves on</param>
///// <param name="side">Moving side</param>
//void _addBishopMoves(board_t* b, moveList_t* move_s);
//
///// <summary>
///// Generate all rook captures for given board and side and add to capMoveList.
///// </summary>
///// <param name="b">board_t to search captures on</param>
///// <param name="side">Moving side</param>
//void _addRookCaptures(board_t* b, moveList_t* move_s);
//
///// <summary>
///// Generate all bishop captures for given board and side and add to capMoveList.
///// </summary>
///// <param name="b">board_t to search captures on</param>
///// <param name="side">Moving side</param>
//void _addBishopCaptures(board_t* b, moveList_t* move_s);
//
///// <summary>
///// Generate all queen moves for given board and side. Add moves to quietMoveList.
///// </summary>
///// <param name="b">board_t</param>
///// <param name="side">Side to move</param>
//void _addQueenMoves(board_t* b, moveList_t* move_s);
//
///// <summary>
///// Generate queen captures and add to capMoveList.
///// </summary>
///// <param name="b">board_t</param>
///// <param name="side">Side to move</param>
//void _addQueenCaptures(board_t* b, moveList_t* move_s);
//
///// <summary>
///// Print all generated moves.
///// </summary>
///// <param name="b">board_t</param>
//void _printGeneratedMoves(moveList_t* move_s);
//
//
//
//

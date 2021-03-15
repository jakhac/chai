//#include "legalMoveGenerator.h"
//
//void _generateMoves(board_t* b, moveList_t* move_s) {
//	move_s->attackedSquares = b->attackerSet(b->side ^ 1);
//
//	//pawns
//	if (b->side == WHITE) {
//		_whiteSinglePawnPush(b, move_s);
//		_whiteDoublePawnPush(b, move_s);
//		_whitePawnCaptures(b, move_s);
//	} else {
//		_blackSinglePawnPush(b, move_s);
//		_blackDoublePawnPush(b, move_s);
//		_blackPawnCaptures(b, move_s);
//	}
//
//	//knights
//	_addKnightMoves(b, move_s);
//	_addKnightCaptures(b, move_s);
//
//	// bishops
//	_addBishopMoves(b, move_s);
//	_addBishopCaptures(b, move_s);
//
//	// king
//	_addKingMoves(b, move_s);
//	_addKingCaptures(b, move_s);
//
//	// rooks
//	_addRookMoves(b, move_s);
//	_addRookCaptures(b, move_s);
//
//	// queens
//	_addQueenMoves(b, move_s);
//	_addQueenCaptures(b, move_s);
//
//	// if check filter moves
//	if (b->getPieces(Piece::KING, b->side) & move_s->attackedSquares) {
//		int validMoves = 0;
//		for (int i = 0; i < move_s->cnt; i++) {
//			b->push(move_s->moves[i]);
//			if (b->isCheck(b->side ^ 1)) {
//				// no add
//			} else {
//				move_s->moves[validMoves++] = move_s->moves[i];
//			}
//			b->pop();
//		}
//		move_s->cnt = validMoves;
//	}
//
//	//MOVE_S pseudo[1];
//	//generateMoves(b, pseudo);
//	//int legalMovesCounter = 0;
//	//int legalMoves[MAX_POSITION_MOVES];
//	//for (int i = 0; i < pseudo->moveCounter; i++) {
//	//	b->push(pseudo->moveList[i]);
//	//	if (b->isCheck(b->side ^ 1)) {
//	//		// non valid move leaves side in check
//	//	} else {
//	//		legalMoves[legalMovesCounter++] = pseudo->moveList[i];
//	//	}
//	//	b->pop();
//	//}
//	//for (int i = 0; i < legalMovesCounter; i++) {
//	//	bool found = false;
//	//	int findMe = legalMoves[i];
//	//	for (int j = 0; j < move_s->moveCounter; j++) {
//	//		if (findMe == move_s->moveList[j]) {
//	//			found = true;
//	//		}
//	//	}
//	//	if (!found) {
//	//		cout << "Should be " << endl;
//	//		for (int i = 0; i < legalMovesCounter; i++) {
//	//			printMove(legalMoves[i]);
//	//		}
//	//		cout << "\n\nBut is" << endl;
//	//		for (int i = 0; i < move_s->moveCounter; i++) {
//	//			printMove(move_s->moveList[i]);
//	//		}
//	//		exit(1);
//	//	}
//	//}
//
//}
//
///// <summary>
///// Calculate possible single push pawn moves for white.
///// </summary>
///// <param name="board">board_t</param>
//void _whiteSinglePawnPush(board_t* board, moveList_t* move_s) {
//	int sq, kSq = board->getKingSquare(WHITE);
//	bitboard_t pinners = board->pinner(kSq, WHITE);
//	bitboard_t pinned = board->pinned(kSq, WHITE);
//
//	bitboard_t whitePawns = board->getPieces(Piece::PAWN, WHITE);
//	bitboard_t potPinned = whitePawns & pinned;
//	whitePawns = whitePawns & ~pinned;
//
//	// iterate pinned pawns
//	//while (potPinned) {
//	//	potPinnedSq = popBit(&potPinned);
//	//	if (FILE_LIST[squareToFile[potPinnedSq]] & pinners) {
//	//		whitePawns |= setMask[potPinnedSq];
//	//	}
//	//}
//
//	while (potPinned) {
//		bitboard_t _pinners = pinners;
//		sq = popBit(&potPinned);
//
//		// for each pawn try to find pinner
//		while (_pinners) {
//			int pinnerSq = popBit(&_pinners);
//			bitboard_t attackLine = obstructed(kSq, pinnerSq) | setMask[kSq] | setMask[pinnerSq];
//
//			// pinned piece found, check for same file (diagonal pins do not allow pushes)
//			if ((setMask[sq] & attackLine) && squareToFile[pinnerSq] == squareToFile[sq]) {
//				whitePawns |= setMask[sq];
//			}
//		}
//	}
//
//	bitboard_t pushedPawns = (whitePawns << 8) & ~board->occupied;
//
//	// divide proms and normal pushes
//	bitboard_t promPawns = pushedPawns & RANK_8_HEX;
//	pushedPawns = pushedPawns & ~RANK_8_HEX;
//
//	// normal pawn pushes
//	while (pushedPawns) {
//		sq = popBit(&pushedPawns);
//		move_s->moves[move_s->cnt++] = serializeMove(sq - 8, sq, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY);
//	}
//
//	// prom pawn pushes
//	while (promPawns) {
//		sq = popBit(&promPawns);
//		move_s->moves[move_s->cnt++] = serializeMove(sq - 8, sq, Piece::EMPTY, Piece::Q, Piece::EMPTY);
//		move_s->moves[move_s->cnt++] = serializeMove(sq - 8, sq, Piece::EMPTY, Piece::R, Piece::EMPTY);
//		move_s->moves[move_s->cnt++] = serializeMove(sq - 8, sq, Piece::EMPTY, Piece::B, Piece::EMPTY);
//		move_s->moves[move_s->cnt++] = serializeMove(sq - 8, sq, Piece::EMPTY, Piece::N, Piece::EMPTY);
//	}
//}
//
///// <summary>
///// Calculate possible single push pawn moves for black.
///// </summary>
///// <param name="board">board_t</param> 
//void _blackSinglePawnPush(board_t* board, moveList_t* move_s) {
//	int sq, kSq = board->getKingSquare(BLACK);
//
//	bitboard_t pinners = board->pinner(kSq, BLACK);
//	bitboard_t pinned = board->pinned(kSq, BLACK);
//	bitboard_t blackPawns = board->getPieces(Piece::PAWN, BLACK);
//	bitboard_t potPinned = blackPawns & pinned;
//	blackPawns &= ~pinned;
//
//	//if there is a pinner on same file as pinned pawn, push is allowed
//   //while (potPinned) {
//   //	int potPinnedSq = popBit(&potPinned);
//   //	if (FILE_LIST[squareToFile[potPinnedSq]] & pinners) {
//   //		blackPawns |= setMask[potPinnedSq];
//   //	}
//   //}
//
//	while (potPinned) {
//		bitboard_t _pinners = pinners;
//		sq = popBit(&potPinned);
//
//		// for each pawn try to find pinner
//		while (_pinners) {
//			int pinnerSq = popBit(&_pinners);
//			bitboard_t attackLine = obstructed(kSq, pinnerSq) | setMask[kSq] | setMask[pinnerSq];
//
//			// pinned piece found, check for same file (diagonal pins do not allow pushes)
//			if ((setMask[sq] & attackLine) && squareToFile[pinnerSq] == squareToFile[sq]) {
//				blackPawns |= setMask[sq];
//			}
//		}
//	}
//
//	bitboard_t pushedPawns = (blackPawns >> 8) & ~board->occupied;
//
//	// divide proms and normal pushes
//	bitboard_t promPawns = pushedPawns & RANK_1_HEX;
//	pushedPawns = pushedPawns & ~RANK_1_HEX;
//
//	// normal pawn pushes
//	while (pushedPawns) {
//		sq = popBit(&pushedPawns);
//		move_s->moves[move_s->cnt++] = serializeMove(sq + 8, sq, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY);
//	}
//
//	// prom pawn pushes
//	while (promPawns) {
//		sq = popBit(&promPawns);
//		move_s->moves[move_s->cnt++] = serializeMove(sq + 8, sq, Piece::EMPTY, Piece::q, Piece::EMPTY);
//		move_s->moves[move_s->cnt++] = serializeMove(sq + 8, sq, Piece::EMPTY, Piece::r, Piece::EMPTY);
//		move_s->moves[move_s->cnt++] = serializeMove(sq + 8, sq, Piece::EMPTY, b, Piece::EMPTY);
//		move_s->moves[move_s->cnt++] = serializeMove(sq + 8, sq, Piece::EMPTY, Piece::n, Piece::EMPTY);
//	}
//}
//
///// <summary>
///// Calculate all possible moves with pawnstart for white.
///// </summary>
///// <param name="board">board_t</param>
//void _whiteDoublePawnPush(board_t* board, moveList_t* move_s) {
//	int potPinnedSq, sq, kSq = board->getKingSquare(WHITE);
//	bitboard_t pinners = board->pinner(kSq, WHITE);
//	bitboard_t pinned = board->pinned(kSq, WHITE);
//
//	bitboard_t whitePawns = board->getPieces(Piece::PAWN, WHITE);
//	bitboard_t potPinned = whitePawns & pinned;
//	whitePawns &= ~pinned;
//
//	// iterate pinned pawns
//	while (potPinned) {
//		potPinnedSq = popBit(&potPinned);
//
//		// if there is a pinner on same file as pinned pawn, push is allowed
//		if (FILE_LIST[squareToFile[potPinnedSq]] & pinners) {
//			whitePawns |= setMask[potPinnedSq];
//		}
//	}
//
//	bitboard_t pushedPawns = (whitePawns << 8) & ~board->occupied;
//	pushedPawns = (pushedPawns << 8) & ~board->occupied & RANK_4_HEX;
//
//	while (pushedPawns) {
//		sq = popBit(&pushedPawns);
//		move_s->moves[move_s->cnt++] = serializeMove(sq - 16, sq, Piece::EMPTY, Piece::EMPTY, MFLAG_PS);
//	}
//}
//
///// <summary>
///// Calculate all possible moves with pawnstart for black.
///// </summary>
///// <param name="board">board_t</param>
//void _blackDoublePawnPush(board_t* board, moveList_t* move_s) {
//	int sq, kSq = board->getKingSquare(BLACK);
//
//	bitboard_t pinners = board->pinner(kSq, BLACK);
//	bitboard_t pinned = board->pinned(kSq, BLACK);
//	bitboard_t blackPawns = board->getPieces(Piece::PAWN, BLACK);
//	bitboard_t potPinned = blackPawns & pinned;
//	blackPawns &= ~pinned;
//
//	// if there is a pinner on same file as pinned pawn, push is allowed
//	while (potPinned) {
//		int potPinnedSq = popBit(&potPinned);
//		if (FILE_LIST[squareToFile[potPinnedSq]] & pinners) {
//			blackPawns |= setMask[potPinnedSq];
//		}
//	}
//
//	bitboard_t pushedPawns = (blackPawns >> 8) & ~board->occupied;
//	pushedPawns = (pushedPawns >> 8) & ~board->occupied & RANK_5_HEX;
//
//	while (pushedPawns) {
//		sq = popBit(&pushedPawns);
//		move_s->moves[move_s->cnt++] = serializeMove(sq + 16, sq, Piece::EMPTY, Piece::EMPTY, MFLAG_PS);
//	}
//}
//
///// <summary>
///// Generate all possible east attacks for white pawns
///// </summary>
///// <param name="board">board_t</param>
//void _whitePawnCaptures(board_t* board, moveList_t* move_s) {
//	int sq, atk_sq, kSq = board->getKingSquare(WHITE);
//
//	bitboard_t atks;
//	bitboard_t pinners = board->pinner(kSq, WHITE);
//	bitboard_t pinned = board->pinned(kSq, WHITE);
//	bitboard_t whitePawns = board->getPieces(Piece::PAWN, WHITE);
//	bitboard_t potPinned = whitePawns & pinned;
//	whitePawns &= ~pinned;
//
//	//iterate pinned pawns
//	while (potPinned) {
//		bitboard_t _pinners = pinners;
//		sq = popBit(&potPinned);
//
//		// for each pawn try to find pinner
//		while (_pinners) {
//			int pinnerSq = popBit(&_pinners);
//			bitboard_t attackLine = obstructed(kSq, pinnerSq) | setMask[kSq] | setMask[pinnerSq];
//
//			// pinned piece found
//			if (setMask[sq] & attackLine) {
//				if (pawnAtkMask[WHITE][sq] & setMask[pinnerSq]) {
//					// capture pinner
//					move_s->moves[move_s->cnt++] = serializeMove(sq, pinnerSq, board->pieceAt(pinnerSq), Piece::EMPTY, Piece::EMPTY);
//					_pinners = 0ULL;
//				}
//			}
//		}
//	}
//
//	//divide in prom and non prom attacks
//	bitboard_t whitePawnProm = whitePawns & RANK_7_HEX;
//	whitePawns &= ~RANK_7_HEX;
//
//	// en passant square
//	if ((whitePawns << 7 & ~FILE_H_HEX) & setMask[board->enPas]) {
//		// set white pawn on enPas square and checkif it is pinned
//		board->clearPiece(Piece::PAWN, board->enPas - 8, BLACK);
//		board->setPiece(Piece::PAWN, board->enPas, WHITE);
//
//		if (!(board->squareAttackedBy(kSq, BLACK))) {
//			move_s->moves[move_s->cnt++] = serializeMove(board->enPas - 7, board->enPas, Piece::EMPTY, Piece::EMPTY, MFLAG_EP);
//		}
//
//		board->setPiece(Piece::PAWN, board->enPas - 8, BLACK);
//		board->clearPiece(Piece::PAWN, board->enPas, WHITE);
//	}
//	if ((whitePawns << 9 & ~FILE_A_HEX) & setMask[board->enPas]) {
//		// set white pawn on enPas square and checkif it is pinned
//		board->clearPiece(Piece::PAWN, board->enPas - 8, BLACK);
//		board->setPiece(Piece::PAWN, board->enPas, WHITE);
//
//		if (!(board->squareAttackedBy(kSq, BLACK))) {
//			move_s->moves[move_s->cnt++] = serializeMove(board->enPas - 9, board->enPas, Piece::EMPTY, Piece::EMPTY, MFLAG_EP);
//		}
//
//		board->setPiece(Piece::PAWN, board->enPas - 8, BLACK);
//		board->clearPiece(Piece::PAWN, board->enPas, WHITE);
//	}
//
//	while (whitePawns) {
//		sq = popBit(&whitePawns);
//		atks = pawnAtkMask[WHITE][sq] & board->color[BLACK];
//		while (atks) {
//			atk_sq = popBit(&atks);
//			move_s->moves[move_s->cnt++] = serializeMove(sq, atk_sq, board->pieceAt(atk_sq), Piece::EMPTY, Piece::EMPTY);
//		}
//	}
//
//	// promoting attacks
//	while (whitePawnProm) {
//		sq = popBit(&whitePawnProm);
//		atks = pawnAtkMask[WHITE][sq] & board->color[BLACK];
//		while (atks) {
//			atk_sq = popBit(&atks);
//			move_s->moves[move_s->cnt++] = serializeMove(sq, atk_sq, board->pieceAt(atk_sq), Piece::Q, Piece::EMPTY);
//			move_s->moves[move_s->cnt++] = serializeMove(sq, atk_sq, board->pieceAt(atk_sq), Piece::R, Piece::EMPTY);
//			move_s->moves[move_s->cnt++] = serializeMove(sq, atk_sq, board->pieceAt(atk_sq), Piece::B, Piece::EMPTY);
//			move_s->moves[move_s->cnt++] = serializeMove(sq, atk_sq, board->pieceAt(atk_sq), Piece::N, Piece::EMPTY);
//		}
//	}
//}
//
///// <summary>
///// Generate all possible east attacks for white pawns.
///// </summary>
///// <param name="board">board_t</param>
//void _blackPawnCaptures(board_t* board, moveList_t* move_s) {
//	int sq, atk_sq, kSq = board->getKingSquare(BLACK);
//
//	bitboard_t atks;
//	bitboard_t pinners = board->pinner(kSq, BLACK);
//	bitboard_t pinned = board->pinned(kSq, BLACK);
//	bitboard_t blackPawns = board->getPieces(Piece::PAWN, BLACK);
//	bitboard_t potPinned = blackPawns & pinned;
//	blackPawns &= ~pinned;
//
//	//iterate pinned pawns
//	while (potPinned) {
//		bitboard_t _pinners = pinners;
//		sq = popBit(&potPinned);
//
//		// for each pawn try to find pinner
//		while (_pinners) {
//			int pinnerSq = popBit(&_pinners);
//			bitboard_t attackLine = obstructed(kSq, pinnerSq) | setMask[kSq] | setMask[pinnerSq];
//
//			// pinned piece found
//			if (setMask[sq] & attackLine) {
//				if (pawnAtkMask[BLACK][sq] & setMask[pinnerSq]) {
//					// capture pinner
//					move_s->moves[move_s->cnt++] = serializeMove(sq, pinnerSq, board->pieceAt(pinnerSq), Piece::EMPTY, Piece::EMPTY);
//					_pinners = 0ULL;
//				}
//			}
//		}
//	}
//
//	//divide in prom and non prom attacks
//	bitboard_t blackPawnProm = blackPawns & RANK_2_HEX;
//	blackPawns &= ~RANK_2_HEX;
//
//	// en passant square
//	if ((blackPawns >> 7 & ~FILE_A_HEX) & setMask[board->enPas]) {
//		board->clearPiece(Piece::PAWN, board->enPas + 8, WHITE);
//		board->setPiece(Piece::PAWN, board->enPas, BLACK);
//
//		if (!(board->squareAttackedBy(kSq, WHITE))) {
//			move_s->moves[move_s->cnt++] = serializeMove(board->enPas + 7, board->enPas, Piece::EMPTY, Piece::EMPTY, MFLAG_EP);
//		}
//
//		board->setPiece(Piece::PAWN, board->enPas + 8, WHITE);
//		board->clearPiece(Piece::PAWN, board->enPas, BLACK);
//	}
//	if ((blackPawns >> 9 & ~FILE_H_HEX) & setMask[board->enPas]) {
//		board->clearPiece(Piece::PAWN, board->enPas + 8, WHITE);
//		board->setPiece(Piece::PAWN, board->enPas, BLACK);
//
//		if (!(board->squareAttackedBy(kSq, WHITE))) {
//			move_s->moves[move_s->cnt++] = serializeMove(board->enPas + 9, board->enPas, Piece::EMPTY, Piece::EMPTY, MFLAG_EP);
//		}
//
//		board->setPiece(Piece::PAWN, board->enPas + 8, WHITE);
//		board->clearPiece(Piece::PAWN, board->enPas, BLACK);
//	}
//
//	while (blackPawns) {
//		sq = popBit(&blackPawns);
//		atks = pawnAtkMask[BLACK][sq] & board->color[WHITE];
//		while (atks) {
//			atk_sq = popBit(&atks);
//			move_s->moves[move_s->cnt++] = serializeMove(sq, atk_sq, board->pieceAt(atk_sq), Piece::EMPTY, Piece::EMPTY);
//		}
//	}
//
//	// promoting attacks
//	while (blackPawnProm) {
//		sq = popBit(&blackPawnProm);
//		atks = pawnAtkMask[BLACK][sq] & board->color[WHITE];
//		while (atks) {
//			atk_sq = popBit(&atks);
//			move_s->moves[move_s->cnt++] = serializeMove(sq, atk_sq, board->pieceAt(atk_sq), Piece::q, Piece::EMPTY);
//			move_s->moves[move_s->cnt++] = serializeMove(sq, atk_sq, board->pieceAt(atk_sq), Piece::r, Piece::EMPTY);
//			move_s->moves[move_s->cnt++] = serializeMove(sq, atk_sq, board->pieceAt(atk_sq), b, Piece::EMPTY);
//			move_s->moves[move_s->cnt++] = serializeMove(sq, atk_sq, board->pieceAt(atk_sq), Piece::n, Piece::EMPTY);
//		}
//	}
//}
//
//void _addKnightMoves(board_t* b, moveList_t* move_s) {
//	int sq, atk_sq;
//
//	// LEGAL ADD leave pinned knights out of move gen, since the cant block / capture
//	bitboard_t knights = b->getPieces(Piece::KNIGHT, b->side) & ~b->pinned(b->getKingSquare(b->side), b->side);
//	bitboard_t atks;
//
//	while (knights) {
//		sq = popBit(&knights);
//		atks = knightAtkMask[sq] & ~b->occupied;
//		while (atks) {
//			atk_sq = popBit(&atks);
//			move_s->moves[move_s->cnt++] = serializeMove(sq, atk_sq, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY);
//		}
//	}
//}
//
//void _addKnightCaptures(board_t* b, moveList_t* move_s) {
//	int sq, atk_sq;
//
//	// LEGAL ADD leave pinned knights out of move gen, since the cant block / capture
//	bitboard_t knights = b->getPieces(Piece::KNIGHT, b->side) & ~b->pinned(b->getKingSquare(b->side), b->side);
//	bitboard_t atks;
//
//	while (knights) {
//		sq = popBit(&knights);
//		atks = knightAtkMask[sq] & b->color[b->side ^ 1];
//		while (atks) {
//			atk_sq = popBit(&atks);
//			move_s->moves[move_s->cnt++] = serializeMove(sq, atk_sq, b->pieceAt(atk_sq), Piece::EMPTY, Piece::EMPTY);
//		}
//	}
//}
//
//void _addKingMoves(board_t* b, moveList_t* move_s) {
//	int sq, kSq = b->getKingSquare(b->side);
//	bitboard_t kingMoves = kingAtkMask[kSq] & ~move_s->attackedSquares & ~b->occupied;
//
//	while (kingMoves) {
//		sq = popBit(&kingMoves);
//		move_s->moves[move_s->cnt++] = serializeMove(kSq, sq, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY);
//	}
//
//	switch (b->side) {
//		case WHITE:
//			if (b->castleValid(K_CASTLE, &move_s->attackedSquares)) {
//				Assert(kSq == E1);
//				move_s->moves[move_s->cnt++] = serializeMove(kSq, G1, Piece::EMPTY, Piece::EMPTY, MFLAG_CAS);
//			}
//			if (b->castleValid(Q_CASTLE, &move_s->attackedSquares)) {
//				Assert(kSq == E1);
//				move_s->moves[move_s->cnt++] = serializeMove(kSq, C1, Piece::EMPTY, Piece::EMPTY, MFLAG_CAS);
//			}
//			break;
//		case BLACK:
//			if (b->castleValid(k_CASTLE, &move_s->attackedSquares)) {
//				Assert(kSq == E8);
//				move_s->moves[move_s->cnt++] = serializeMove(kSq, G8, Piece::EMPTY, Piece::EMPTY, MFLAG_CAS);
//			}
//
//			if (b->castleValid(q_CASTLE, &move_s->attackedSquares)) {
//				Assert(kSq == E8);
//				move_s->moves[move_s->cnt++] = serializeMove(kSq, C8, Piece::EMPTY, Piece::EMPTY, MFLAG_CAS);
//			}
//			break;
//		default: break;
//	}
//}
//
//void _addKingCaptures(board_t* b, moveList_t* move_s) {
//	int atk_sq, kSq = b->getKingSquare(b->side);
//	bitboard_t whiteKingAttacks = kingAtkMask[kSq] & ~move_s->attackedSquares & b->color[b->side ^ 1];
//
//	while (whiteKingAttacks) {
//		atk_sq = popBit(&whiteKingAttacks);
//		move_s->moves[move_s->cnt++] = serializeMove(kSq, atk_sq, b->pieceAt(atk_sq), Piece::EMPTY, Piece::EMPTY);
//	}
//}
//
////void _addWhiteKingCaptures(board_t board) {
////	U64 wKing = board.getPieces(Piece::KING, WHITE);
////	int wSq = popBit(&wKing);
////
////	U64 bKing = board.getPieces(Piece::KING, BLACK);
////	int bSq = popBit(&bKing);
////
////	U64 whiteKingAttacks = kingAtkMask[wSq] & ~kingAtkMask[bSq] & board.color[BLACK];
////	int atk_sq;
////
////	while (whiteKingAttacks) {
////		atk_sq = popBit(&whiteKingAttacks);
////
////		// check for non attacked square
////		if (board.squareAttacked(atk_sq, board.side ^ 1)) continue;
////		_capMoveList.push_back(Move(MOVE(wSq, atk_sq, board.pieceAt(atk_sq), Piece::EMPTY, Piece::EMPTY), 0));
////	}
////}
////
////void _addBlackKingCaptures(board_t board) {
////	U64 bKing = board.getPieces(Piece::KING, BLACK);
////	int bSq = popBit(&bKing);
////
////	U64 wKing = board.getPieces(Piece::KING, WHITE);
////	int wSq = popBit(&wKing);
////
////	U64 blackKingAttacks = kingAtkMask[bSq] & ~kingAtkMask[wSq] & board.color[WHITE];
////	int atk_sq;
////
////	while (blackKingAttacks) {
////		atk_sq = popBit(&blackKingAttacks);
////
////		// check for non attacked square
////		if (board.squareAttacked(atk_sq, board.side ^ 1)) continue;
////		_capMoveList.push_back(Move(MOVE(bSq, atk_sq, board.pieceAt(atk_sq), Piece::EMPTY, Piece::EMPTY), 0));
////	}
////}
//
//void _addRookMoves(board_t* b, moveList_t* move_s) {
//	int sq, atk_sq, kSq = b->getKingSquare(b->side);
//	bitboard_t attackSet;
//	bitboard_t pinned = b->pinned(kSq, b->side);
//
//	// LEGAL ADD, only use non pinned rooks in std gen
//	bitboard_t rooks = b->getPieces(Piece::ROOK, b->side) & ~pinned;
//	while (rooks) {
//		sq = popBit(&rooks);
//		attackSet = lookUpRookMoves(sq, b->occupied);
//		Assert(attackSet == calculateRookMoves(sq, b->occupied));
//		attackSet &= ~b->occupied;
//		while (attackSet) {
//			atk_sq = popBit(&attackSet);
//			move_s->moves[move_s->cnt++] = serializeMove(sq, atk_sq, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY);
//		}
//	}
//
//	// pinned rooks only move in pinned direction
//	rooks = b->getPieces(Piece::ROOK, b->side) & pinned;
//	while (rooks) {
//		sq = popBit(&rooks);
//		attackSet = lookUpRookMoves(sq, b->occupied) & lineBB[sq][kSq];
//		attackSet &= ~b->occupied;
//		while (attackSet) {
//			atk_sq = popBit(&attackSet);
//			move_s->moves[move_s->cnt++] = serializeMove(sq, atk_sq, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY);
//		}
//	}
//}
//
//void _addBishopMoves(board_t* b, moveList_t* move_s) {
//	int sq, atk_sq, kSq = b->getKingSquare(b->side);
//	bitboard_t pinned = b->pinned(kSq, b->side);
//
//	bitboard_t bishops = b->getPieces(Piece::BISHOP, b->side) & ~pinned;
//	bitboard_t attackSet;
//	while (bishops) {
//		sq = popBit(&bishops);
//		attackSet = lookUpBishopMoves(sq, b->occupied);
//		attackSet &= ~b->occupied;
//
//		while (attackSet) {
//			atk_sq = popBit(&attackSet);
//			move_s->moves[move_s->cnt++] = serializeMove(sq, atk_sq, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY);
//		}
//	}
//
//	// pinned bishops only move in pinned direction
//	bishops = b->getPieces(Piece::BISHOP, b->side) & pinned;
//	while (bishops) {
//		sq = popBit(&bishops);
//		attackSet = lookUpBishopMoves(sq, b->occupied) & lineBB[sq][kSq];
//		attackSet &= ~b->occupied;
//		while (attackSet) {
//			atk_sq = popBit(&attackSet);
//			move_s->moves[move_s->cnt++] = serializeMove(sq, atk_sq, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY);
//		}
//	}
//}
//
//void _addRookCaptures(board_t* b, moveList_t* move_s) {
//	int sq, atk_sq, kSq = b->getKingSquare(b->side);
//	bitboard_t attackSet;
//	bitboard_t pinned = b->pinned(kSq, b->side);
//
//	// LEGAL ADD, only use non pinned rooks in std gen
//	bitboard_t rooks = b->getPieces(Piece::ROOK, b->side) & ~pinned;
//	while (rooks) {
//		sq = popBit(&rooks);
//		attackSet = lookUpRookMoves(sq, b->occupied);
//		Assert(attackSet == calculateRookMoves(sq, b->occupied));
//		attackSet &= b->color[b->side ^ 1];
//		while (attackSet) {
//			atk_sq = popBit(&attackSet);
//			move_s->moves[move_s->cnt++] = serializeMove(sq, atk_sq, b->pieceAt(atk_sq), Piece::EMPTY, Piece::EMPTY);
//		}
//	}
//
//	// pinned rooks only move in pinned direction
//	rooks = b->getPieces(Piece::ROOK, b->side) & pinned;
//	while (rooks) {
//		sq = popBit(&rooks);
//		attackSet = lookUpRookMoves(sq, b->occupied) & lineBB[sq][kSq];
//		attackSet &= b->color[b->side ^ 1];
//		while (attackSet) {
//			atk_sq = popBit(&attackSet);
//			move_s->moves[move_s->cnt++] = serializeMove(sq, atk_sq, b->pieceAt(atk_sq), Piece::EMPTY, Piece::EMPTY);
//		}
//	}
//}
//
//void _addBishopCaptures(board_t* b, moveList_t* move_s) {
//	int sq, atk_sq, kSq = b->getKingSquare(b->side);
//	bitboard_t attackSet;
//	bitboard_t pinned = b->pinned(kSq, b->side);
//
//	bitboard_t bishops = b->getPieces(Piece::BISHOP, b->side) & ~pinned;
//	while (bishops) {
//		sq = popBit(&bishops);
//		attackSet = lookUpBishopMoves(sq, b->occupied);
//		attackSet &= b->color[b->side ^ 1];
//
//		while (attackSet) {
//			atk_sq = popBit(&attackSet);
//			move_s->moves[move_s->cnt++] = serializeMove(sq, atk_sq, b->pieceAt(atk_sq), Piece::EMPTY, Piece::EMPTY);
//		}
//	}
//
//	// pinned bishops only move in pinned direction
//	bishops = b->getPieces(Piece::BISHOP, b->side) & pinned;
//	while (bishops) {
//		sq = popBit(&bishops);
//		attackSet = lookUpBishopMoves(sq, b->occupied) & lineBB[sq][kSq];
//		attackSet &= b->color[b->side ^ 1];
//		while (attackSet) {
//			atk_sq = popBit(&attackSet);
//			move_s->moves[move_s->cnt++] = serializeMove(sq, atk_sq, b->pieceAt(atk_sq), Piece::EMPTY, Piece::EMPTY);
//		}
//	}
//}
//
//void _addQueenMoves(board_t* b, moveList_t* move_s) {
//	int sq, atk_sq, kSq = b->getKingSquare(b->side);
//	bitboard_t attackSet;
//	bitboard_t pinned = b->pinned(kSq, b->side);
//
//	bitboard_t queens = b->getPieces(Piece::QUEEN, b->side) & ~pinned;
//	while (queens) {
//		sq = popBit(&queens);
//		attackSet = lookUpRookMoves(sq, b->occupied) ^ lookUpBishopMoves(sq, b->occupied);
//		attackSet &= ~b->occupied;
//
//		while (attackSet) {
//			atk_sq = popBit(&attackSet);
//			move_s->moves[move_s->cnt++] = serializeMove(sq, atk_sq, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY);
//		}
//	}
//
//	queens = b->getPieces(Piece::QUEEN, b->side) & pinned;
//	while (queens) {
//		sq = popBit(&queens);
//		attackSet = lookUpRookMoves(sq, b->occupied) ^ lookUpBishopMoves(sq, b->occupied);
//		attackSet &= ~b->occupied & lineBB[sq][kSq];
//
//		while (attackSet) {
//			atk_sq = popBit(&attackSet);
//			move_s->moves[move_s->cnt++] = serializeMove(sq, atk_sq, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY);
//		}
//	}
//}
//
//void _addQueenCaptures(board_t* b, moveList_t* move_s) {
//	int sq, atk_sq, kSq = b->getKingSquare(b->side);
//	bitboard_t attackSet;
//	bitboard_t pinned = b->pinned(kSq, b->side);
//
//	bitboard_t queens = b->getPieces(Piece::QUEEN, b->side) & ~pinned;
//	while (queens) {
//		sq = popBit(&queens);
//		attackSet = lookUpRookMoves(sq, b->occupied) ^ lookUpBishopMoves(sq, b->occupied);
//		attackSet &= b->color[b->side ^ 1];
//
//		while (attackSet) {
//			atk_sq = popBit(&attackSet);
//			move_s->moves[move_s->cnt++] = serializeMove(sq, atk_sq, b->pieceAt(atk_sq), Piece::EMPTY, Piece::EMPTY);
//		}
//	}
//
//	queens = b->getPieces(Piece::QUEEN, b->side) & pinned;
//	while (queens) {
//		sq = popBit(&queens);
//		attackSet = lookUpRookMoves(sq, b->occupied) ^ lookUpBishopMoves(sq, b->occupied);
//		attackSet &= b->color[b->side ^ 1] & lineBB[sq][kSq];
//
//		while (attackSet) {
//			atk_sq = popBit(&attackSet);
//			move_s->moves[move_s->cnt++] = serializeMove(sq, atk_sq, b->pieceAt(atk_sq), Piece::EMPTY, Piece::EMPTY);
//		}
//	}
//}
//
//void _printGeneratedMoves(moveList_t* move_s) {
//
//	cout << "\nGenerated " << move_s->cnt << " moves: " << endl;
//
//	for (int i = 0; i < move_s->cnt; i++) {
//		printMove(move_s->moves[i]);
//	}
//}
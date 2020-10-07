#include "legalMoveGenerator.h"

void LegalMoveGenerator::_generateMoves(Board b) {
	_resetMoveLists();

	const int playerToMove = b.side;

	// pawns
	_addPawnMoves(b, playerToMove);

	//knights
	_addKnightMoves(b, playerToMove);
	_addKnightCaptures(b, playerToMove);

	// bishops
	_addBishopMoves(b, playerToMove);
	_addBishopCaptures(b, playerToMove);

	// king
	_addKingMoves(b, playerToMove);
	_addKingCaptures(b, playerToMove);

	// rooks
	_addRookMoves(b, playerToMove);
	_addRookCaptures(b, playerToMove);

	// queens
	_addQueenMoves(b, playerToMove);
	_addQueenCaptures(b, playerToMove);
}

vector<Move> LegalMoveGenerator::_getAllMoves(Board b) {
	vector <Move> allMoves;
	allMoves.reserve(_capMoveList.size() + _quietMoveList.size()); // preallocate memory
	allMoves.insert(allMoves.end(), _capMoveList.begin(), _capMoveList.end()); // copy
	allMoves.insert(allMoves.end(), _quietMoveList.begin(), _quietMoveList.end()); // copy

	int currentSide = b.side;
	if (b.isCheck(currentSide)) {
		vector<Move> checkEvaMoves;
		for (Move m : allMoves) {
			b.push(m.move);

			if (b.isCheck(currentSide)) {
				b.pop();
				continue;
			}

			checkEvaMoves.push_back(m);
			b.pop();
		}
		return checkEvaMoves;
	}

	return allMoves;
}

void LegalMoveGenerator::_resetMoveLists() {
	_quietMoveList = {};
	_capMoveList = {};
}

/// <summary>
/// Generate all possible pawn moves for given board. Side is retrieved with board instance.
/// </summary>
/// <param name="b">Board</param>
void LegalMoveGenerator::_addPawnMoves(Board b, const int side) {
	if (side) {
		_whiteSinglePawnPush(b); // #legal
		_whiteDoublePawnPush(b); // #legal
		_whitePawnCaptures(b); // #legal
	} else {
		_blackSinglePawnPush(b); // #legal
		_blackDoublePawnPush(b); // #legal
		_blackPawnCaptures(b); // #legal
	}
}

/// <summary>
/// Calculate possible single push pawn moves for white.
/// </summary>
/// <param name="board">Board</param>
void LegalMoveGenerator::_whiteSinglePawnPush(Board board) {
	int potPinnedSq, sq, kSq = board.getKingSquare(WHITE);
	U64 pinners = board.pinners(kSq, WHITE);
	U64 pinned = board.pinned(kSq, WHITE);

	U64 whitePawns = board.getPieces(PAWN, WHITE);
	U64 potPinned = whitePawns & pinned;
	whitePawns = whitePawns & ~pinned;

	// iterate pinned pawns
	while (potPinned) {
		potPinnedSq = popBit(&potPinned);

		// if there is a pinner on same file as pinned pawn, push is allowed
		if (FILE_LIST[squareToFile[potPinnedSq]] & pinners) {
			whitePawns |= setMask[potPinnedSq];
		}
	}

	U64 pushedPawns = (whitePawns << 8) & ~board.occupied;

	// divide proms and normal pushes
	U64 promPawns = pushedPawns & RANK_8_HEX;
	pushedPawns = pushedPawns & ~RANK_8_HEX;

	// normal pawn pushes
	while (pushedPawns) {
		sq = popBit(&pushedPawns);
		_quietMoveList.push_back(Move(MOVE(sq - 8, sq, EMPTY, EMPTY, EMPTY), 0));
	}

	// prom pawn pushes
	while (promPawns) {
		sq = popBit(&promPawns);
		_quietMoveList.push_back(Move(MOVE(sq - 8, sq, EMPTY, Q, EMPTY), 0));
		_quietMoveList.push_back(Move(MOVE(sq - 8, sq, EMPTY, R, EMPTY), 0));
		_quietMoveList.push_back(Move(MOVE(sq - 8, sq, EMPTY, B, EMPTY), 0));
		_quietMoveList.push_back(Move(MOVE(sq - 8, sq, EMPTY, N, EMPTY), 0));
	}
}

/// <summary>
/// Calculate possible single push pawn moves for black.
/// </summary>
/// <param name="board">Board</param> 
void LegalMoveGenerator::_blackSinglePawnPush(Board board) {
	int sq, kSq = board.getKingSquare(BLACK);

	U64 pinners = board.pinners(kSq, BLACK);
	U64 pinned = board.pinned(kSq, BLACK);
	U64 blackPawns = board.getPieces(PAWN, BLACK);
	U64 potPinned = blackPawns & pinned;
	blackPawns &= ~pinned;

	// if there is a pinner on same file as pinned pawn, push is allowed
	while (potPinned) {
		int potPinnedSq = popBit(&potPinned);
		if (FILE_LIST[squareToFile[potPinnedSq]] & pinners) {
			blackPawns |= setMask[potPinnedSq];
		}
	}

	U64 pushedPawns = (blackPawns >> 8) & ~board.occupied;

	// divide proms and normal pushes
	U64 promPawns = pushedPawns & RANK_1_HEX;
	pushedPawns = pushedPawns & ~RANK_1_HEX;

	// normal pawn pushes
	while (pushedPawns) {
		sq = popBit(&pushedPawns);
		_quietMoveList.push_back(Move(MOVE(sq + 8, sq, EMPTY, EMPTY, EMPTY), 0));
	}

	// prom pawn pushes
	while (promPawns) {
		sq = popBit(&promPawns);
		_quietMoveList.push_back(Move(MOVE(sq + 8, sq, EMPTY, q, EMPTY), 0));
		_quietMoveList.push_back(Move(MOVE(sq + 8, sq, EMPTY, r, EMPTY), 0));
		_quietMoveList.push_back(Move(MOVE(sq + 8, sq, EMPTY, b, EMPTY), 0));
		_quietMoveList.push_back(Move(MOVE(sq + 8, sq, EMPTY, n, EMPTY), 0));
	}
}

/// <summary>
/// Calculate all possible moves with pawnstart for white.
/// </summary>
/// <param name="board">Board</param>
void LegalMoveGenerator::_whiteDoublePawnPush(Board board) {
	int potPinnedSq, sq, kSq = board.getKingSquare(WHITE);
	U64 pinners = board.pinners(kSq, WHITE);
	U64 pinned = board.pinned(kSq, WHITE);

	U64 whitePawns = board.getPieces(PAWN, WHITE);
	U64 potPinned = whitePawns & pinned;
	whitePawns &= ~pinned;

	// iterate pinned pawns
	while (potPinned) {
		potPinnedSq = popBit(&potPinned);

		// if there is a pinner on same file as pinned pawn, push is allowed
		if (FILE_LIST[squareToFile[potPinnedSq]] & pinners) {
			whitePawns |= setMask[potPinnedSq];
		}
	}

	U64 pushedPawns = (whitePawns << 8) & ~board.occupied;
	pushedPawns = (pushedPawns << 8) & ~board.occupied & RANK_4_HEX;

	while (pushedPawns) {
		sq = popBit(&pushedPawns);
		_quietMoveList.push_back(Move(MOVE(sq - 16, sq, EMPTY, EMPTY, MFLAGPS), 0));
	}
}

/// <summary>
/// Calculate all possible moves with pawnstart for black.
/// </summary>
/// <param name="board">Board</param>
void LegalMoveGenerator::_blackDoublePawnPush(Board board) {
	int sq, kSq = board.getKingSquare(BLACK);

	U64 pinners = board.pinners(kSq, BLACK);
	U64 pinned = board.pinned(kSq, BLACK);
	U64 blackPawns = board.getPieces(PAWN, BLACK);
	U64 potPinned = blackPawns & pinned;
	blackPawns &= ~pinned;

	// if there is a pinner on same file as pinned pawn, push is allowed
	while (potPinned) {
		int potPinnedSq = popBit(&potPinned);
		if (FILE_LIST[squareToFile[potPinnedSq]] & pinners) {
			blackPawns |= setMask[potPinnedSq];
		}
	}

	U64 pushedPawns = (blackPawns >> 8) & ~board.occupied;
	pushedPawns = (pushedPawns >> 8) & ~board.occupied & RANK_5_HEX;

	while (pushedPawns) {
		sq = popBit(&pushedPawns);
		_quietMoveList.push_back(Move(MOVE(sq + 16, sq, EMPTY, EMPTY, MFLAGPS), 0));
	}
}

/// <summary>
/// Generate all possible east attacks for white pawns
/// </summary>
/// <param name="board">Board</param>
void LegalMoveGenerator::_whitePawnCaptures(Board board) {
	int sq, atk_sq, kSq = board.getKingSquare(WHITE);

	U64 atks;
	U64 pinners = board.pinners(kSq, WHITE);
	U64 pinned = board.pinned(kSq, WHITE);
	U64 whitePawns = board.getPieces(PAWN, WHITE);
	U64 potPinned = whitePawns & pinned;
	whitePawns &= ~pinned;

	//iterate pinned pawns
	while (potPinned) {
		U64 _pinners = pinners;
		sq = popBit(&potPinned);

		// for each pawn try to find pinner
		while (_pinners) {
			int pinnerSq = popBit(&_pinners);
			U64 attackLine = obstructed(kSq, pinnerSq) | setMask[kSq] | setMask[pinnerSq];

			// pinned piece found
			if (setMask[sq] & attackLine) {
				if (pawnAtkMask[WHITE][sq] & setMask[pinnerSq]) {
					// capture pinner
					_capMoveList.push_back(Move(MOVE(sq, pinnerSq, board.pieceAt(pinnerSq), EMPTY, EMPTY), 0));
					_pinners = 0ULL;
				}
			}
		}
	}

	//divide in prom and non prom attacks
	U64 whitePawnProm = whitePawns & RANK_7_HEX;
	whitePawns &= ~RANK_7_HEX;

	// en passant square
	if ((whitePawns << 7 & ~FILE_H_HEX) & setMask[board.enPas]) {
		// set white pawn on enPas square and checkif it is pinned
		board.clearPiece(PAWN, board.enPas - 8, BLACK);
		board.setPiece(PAWN, board.enPas, WHITE);

		if (!(board.squareAttacked(kSq, BLACK))) {
			_capMoveList.push_back(Move(MOVE(board.enPas - 7, board.enPas, EMPTY, EMPTY, MFLAGEP), 0));
		}

		board.setPiece(PAWN, board.enPas - 8, BLACK);
		board.clearPiece(PAWN, board.enPas, WHITE);
	}
	if ((whitePawns << 9 & ~FILE_A_HEX) & setMask[board.enPas]) {
		// set white pawn on enPas square and checkif it is pinned
		board.clearPiece(PAWN, board.enPas - 8, BLACK);
		board.setPiece(PAWN, board.enPas, WHITE);

		if (!(board.squareAttacked(kSq, BLACK))) {
			_capMoveList.push_back(Move(MOVE(board.enPas - 9, board.enPas, EMPTY, EMPTY, MFLAGEP), 0));
		}

		board.setPiece(PAWN, board.enPas - 8, BLACK);
		board.clearPiece(PAWN, board.enPas, WHITE);
	}

	while (whitePawns) {
		sq = popBit(&whitePawns);
		atks = pawnAtkMask[WHITE][sq] & board.color[BLACK];
		while (atks) {
			atk_sq = popBit(&atks);
			_capMoveList.push_back(Move(MOVE(sq, atk_sq, board.pieceAt(atk_sq), EMPTY, EMPTY), 0));
		}
	}

	// promoting attacks
	while (whitePawnProm) {
		sq = popBit(&whitePawnProm);
		atks = pawnAtkMask[WHITE][sq] & board.color[BLACK];
		while (atks) {
			atk_sq = popBit(&atks);
			_capMoveList.push_back(Move(MOVE(sq, atk_sq, board.pieceAt(atk_sq), Q, EMPTY), 0));
			_capMoveList.push_back(Move(MOVE(sq, atk_sq, board.pieceAt(atk_sq), R, EMPTY), 0));
			_capMoveList.push_back(Move(MOVE(sq, atk_sq, board.pieceAt(atk_sq), B, EMPTY), 0));
			_capMoveList.push_back(Move(MOVE(sq, atk_sq, board.pieceAt(atk_sq), N, EMPTY), 0));
		}
	}
}

/// <summary>
/// Generate all possible east attacks for white pawns.
/// </summary>
/// <param name="board">Board</param>
void LegalMoveGenerator::_blackPawnCaptures(Board board) {
	int sq, atk_sq, kSq = board.getKingSquare(BLACK);

	U64 atks;
	U64 pinners = board.pinners(kSq, BLACK);
	U64 pinned = board.pinned(kSq, BLACK);
	U64 blackPawns = board.getPieces(PAWN, BLACK);
	U64 potPinned = blackPawns & pinned;
	blackPawns &= ~pinned;

	//iterate pinned pawns
	while (potPinned) {
		U64 _pinners = pinners;
		sq = popBit(&potPinned);

		// for each pawn try to find pinner
		while (_pinners) {
			int pinnerSq = popBit(&_pinners);
			U64 attackLine = obstructed(kSq, pinnerSq) | setMask[kSq] | setMask[pinnerSq];

			// pinned piece found
			if (setMask[sq] & attackLine) {
				if (pawnAtkMask[BLACK][sq] & setMask[pinnerSq]) {
					// capture pinner
					_capMoveList.push_back(Move(MOVE(sq, pinnerSq, board.pieceAt(pinnerSq), EMPTY, EMPTY), 0));
					_pinners = 0ULL;
				}
			}
		}
	}

	//divide in prom and non prom attacks
	U64 blackPawnProm = blackPawns & RANK_2_HEX;
	blackPawns &= ~RANK_2_HEX;

	// en passant square
	if ((blackPawns >> 7 & ~FILE_A_HEX) & setMask[board.enPas]) {
		board.clearPiece(PAWN, board.enPas + 8, WHITE);
		board.setPiece(PAWN, board.enPas, BLACK);

		if (!(board.squareAttacked(kSq, WHITE))) {
			_capMoveList.push_back(Move(MOVE(board.enPas + 7, board.enPas, EMPTY, EMPTY, MFLAGEP), 0));
		}

		board.setPiece(PAWN, board.enPas + 8, WHITE);
		board.clearPiece(PAWN, board.enPas, BLACK);
	}
	if ((blackPawns >> 9 & ~FILE_H_HEX) & setMask[board.enPas]) {
		board.clearPiece(PAWN, board.enPas + 8, WHITE);
		board.setPiece(PAWN, board.enPas, BLACK);

		if (!(board.squareAttacked(kSq, WHITE))) {
			_capMoveList.push_back(Move(MOVE(board.enPas + 9, board.enPas, EMPTY, EMPTY, MFLAGEP), 0));
		}

		board.setPiece(PAWN, board.enPas + 8, WHITE);
		board.clearPiece(PAWN, board.enPas, BLACK);
	}

	while (blackPawns) {
		sq = popBit(&blackPawns);
		atks = pawnAtkMask[BLACK][sq] & board.color[WHITE];
		while (atks) {
			atk_sq = popBit(&atks);
			_capMoveList.push_back(Move(MOVE(sq, atk_sq, board.pieceAt(atk_sq), EMPTY, EMPTY), 0));
		}
	}

	// promoting attacks
	while (blackPawnProm) {
		sq = popBit(&blackPawnProm);
		atks = pawnAtkMask[BLACK][sq] & board.color[WHITE];
		while (atks) {
			atk_sq = popBit(&atks);
			_capMoveList.push_back(Move(MOVE(sq, atk_sq, board.pieceAt(atk_sq), q, EMPTY), 0));
			_capMoveList.push_back(Move(MOVE(sq, atk_sq, board.pieceAt(atk_sq), r, EMPTY), 0));
			_capMoveList.push_back(Move(MOVE(sq, atk_sq, board.pieceAt(atk_sq), b, EMPTY), 0));
			_capMoveList.push_back(Move(MOVE(sq, atk_sq, board.pieceAt(atk_sq), n, EMPTY), 0));
		}
	}
}

void LegalMoveGenerator::_addKnightMoves(Board b, const int side) {
	int sq, atk_sq;

	// LEGAL ADD leave pinned knights out of move gen, since the cant block / capture
	U64 knights = b.getPieces(KNIGHT, side) & ~b.pinned(b.getKingSquare(side), side);
	U64 atks;

	while (knights) {
		sq = popBit(&knights);
		atks = knightAtkMask[sq] & ~b.occupied;
		while (atks) {
			atk_sq = popBit(&atks);
			_quietMoveList.push_back(Move(MOVE(sq, atk_sq, EMPTY, EMPTY, EMPTY), 0));
		}
	}
}

void LegalMoveGenerator::_addKnightCaptures(Board b, const int side) {
	int sq, atk_sq;

	// LEGAL ADD leave pinned knights out of move gen, since the cant block / capture
	U64 knights = b.getPieces(KNIGHT, side) & ~b.pinned(b.getKingSquare(side), side);
	U64 atks;

	while (knights) {
		sq = popBit(&knights);
		atks = knightAtkMask[sq] & b.color[side ^ 1];
		while (atks) {
			atk_sq = popBit(&atks);
			_capMoveList.push_back(Move(MOVE(sq, atk_sq, b.pieceAt(atk_sq), EMPTY, EMPTY), 0));
		}
	}
}

void LegalMoveGenerator::_addKingMoves(Board b, const int side) {
	if (side == WHITE) _addWhiteKingMoves(b);
	else _addBlackKingMoves(b);
}

void LegalMoveGenerator::_addWhiteKingMoves(Board board) {
	U64 wKing = board.getPieces(KING, WHITE);
	int wSq = popBit(&wKing);

	U64 bKing = board.getPieces(KING, BLACK);
	int bSq = popBit(&bKing);

	// kings cant move on squares attacked by opp king
	U64 kingMoves = kingAtkMask[wSq] & ~kingAtkMask[bSq] & ~board.occupied;
	int sq;

	while (kingMoves) {
		sq = popBit(&kingMoves);

		// check for non attacked square
		if (board.squareAttacked(sq, board.side ^ 1)) continue;
		_quietMoveList.push_back(Move(MOVE(wSq, sq, EMPTY, EMPTY, EMPTY), 0));
	}

	if (board.castleValid(K_CASTLE)) {
		ASSERT(wSq == E1);
		_quietMoveList.push_back(Move(MOVE(wSq, G1, EMPTY, EMPTY, MFLAGCA), 0));
	}

	if (board.castleValid(Q_CASTLE)) {
		ASSERT(wSq == E1);
		_quietMoveList.push_back(Move(MOVE(wSq, C1, EMPTY, EMPTY, MFLAGCA), 0));
	}
}

void LegalMoveGenerator::_addBlackKingMoves(Board board) {
	U64 bKing = board.getPieces(KING, BLACK);
	int bSq = popBit(&bKing);

	U64 wKing = board.getPieces(KING, WHITE);
	int wSq = popBit(&wKing);

	U64 kingMoves = kingAtkMask[bSq] & ~kingAtkMask[wSq] & ~board.occupied;
	int sq;

	while (kingMoves) {
		sq = popBit(&kingMoves);

		// check for non attacked square
		if (board.squareAttacked(sq, board.side ^ 1)) continue;
		_quietMoveList.push_back(Move(MOVE(bSq, sq, EMPTY, EMPTY, EMPTY), 0));
	}

	if (board.castleValid(k_CASTLE)) {
		ASSERT(bSq == E8);
		_quietMoveList.push_back(Move(MOVE(bSq, G8, EMPTY, EMPTY, MFLAGCA), 0));
	}

	if (board.castleValid(q_CASTLE)) {
		ASSERT(bSq == E8);
		_quietMoveList.push_back(Move(MOVE(bSq, C8, EMPTY, EMPTY, MFLAGCA), 0));
	}
}

void LegalMoveGenerator::_addKingCaptures(Board b, const int side) {
	if (side == WHITE) _addWhiteKingCaptures(b);
	else _addBlackKingCaptures(b);
}

void LegalMoveGenerator::_addWhiteKingCaptures(Board board) {
	U64 wKing = board.getPieces(KING, WHITE);
	int wSq = popBit(&wKing);

	U64 bKing = board.getPieces(KING, BLACK);
	int bSq = popBit(&bKing);

	U64 whiteKingAttacks = kingAtkMask[wSq] & ~kingAtkMask[bSq] & board.color[BLACK];
	int atk_sq;

	while (whiteKingAttacks) {
		atk_sq = popBit(&whiteKingAttacks);

		// check for non attacked square
		if (board.squareAttacked(atk_sq, board.side ^ 1)) continue;
		_capMoveList.push_back(Move(MOVE(wSq, atk_sq, board.pieceAt(atk_sq), EMPTY, EMPTY), 0));
	}
}

void LegalMoveGenerator::_addBlackKingCaptures(Board board) {
	U64 bKing = board.getPieces(KING, BLACK);
	int bSq = popBit(&bKing);

	U64 wKing = board.getPieces(KING, WHITE);
	int wSq = popBit(&wKing);

	U64 blackKingAttacks = kingAtkMask[bSq] & ~kingAtkMask[wSq] & board.color[WHITE];
	int atk_sq;

	while (blackKingAttacks) {
		atk_sq = popBit(&blackKingAttacks);

		// check for non attacked square
		if (board.squareAttacked(atk_sq, board.side ^ 1)) continue;
		_capMoveList.push_back(Move(MOVE(bSq, atk_sq, board.pieceAt(atk_sq), EMPTY, EMPTY), 0));
	}
}

void LegalMoveGenerator::_addRookMoves(Board b, const int side) {
	int sq, atk_sq, kSq = b.getKingSquare(side);
	U64 attackSet;
	U64 pinned = b.pinned(kSq, side);

	// LEGAL ADD, only use non pinned rooks in std gen
	U64 rooks = b.getPieces(ROOK, side) & ~pinned;
	while (rooks) {
		sq = popBit(&rooks);
		attackSet = lookUpRookMoves(sq, b.occupied);
		ASSERT(attackSet == calculateRookMoves(sq, b.occupied));
		attackSet &= ~b.occupied;
		while (attackSet) {
			atk_sq = popBit(&attackSet);
			_quietMoveList.push_back(Move(MOVE(sq, atk_sq, EMPTY, EMPTY, EMPTY), 0));
		}
	}

	// pinned rooks only move in pinned direction
	rooks = b.getPieces(ROOK, side) & pinned;
	while (rooks) {
		sq = popBit(&rooks);
		attackSet = lookUpRookMoves(sq, b.occupied) & lineBB[sq][kSq];
		attackSet &= ~b.occupied;
		while (attackSet) {
			atk_sq = popBit(&attackSet);
			_quietMoveList.push_back(Move(MOVE(sq, atk_sq, EMPTY, EMPTY, EMPTY), 0));
		}
	}
}

void LegalMoveGenerator::_addBishopMoves(Board b, const int side) {
	int sq, atk_sq, kSq = b.getKingSquare(side);
	U64 pinned = b.pinned(kSq, side);

	U64 bishops = b.getPieces(BISHOP, side) & ~pinned;
	U64 attackSet;
	while (bishops) {
		sq = popBit(&bishops);
		attackSet = lookUpBishopMoves(sq, b.occupied);
		attackSet &= ~b.occupied;

		while (attackSet) {
			atk_sq = popBit(&attackSet);
			_quietMoveList.push_back(Move(MOVE(sq, atk_sq, EMPTY, EMPTY, EMPTY), 0));
		}
	}

	// pinned bishops only move in pinned direction
	bishops = b.getPieces(BISHOP, side) & pinned;
	while (bishops) {
		sq = popBit(&bishops);
		attackSet = lookUpBishopMoves(sq, b.occupied) & lineBB[sq][kSq];
		attackSet &= ~b.occupied;
		while (attackSet) {
			atk_sq = popBit(&attackSet);
			_quietMoveList.push_back(Move(MOVE(sq, atk_sq, EMPTY, EMPTY, EMPTY), 0));
		}
	}
}

void LegalMoveGenerator::_addRookCaptures(Board b, const int side) {
	int sq, atk_sq, kSq = b.getKingSquare(side);
	U64 attackSet;
	U64 pinned = b.pinned(kSq, side);

	// LEGAL ADD, only use non pinned rooks in std gen
	U64 rooks = b.getPieces(ROOK, side) & ~pinned;
	while (rooks) {
		sq = popBit(&rooks);
		attackSet = lookUpRookMoves(sq, b.occupied);
		ASSERT(attackSet == calculateRookMoves(sq, b.occupied));
		attackSet &= b.color[side ^ 1];
		while (attackSet) {
			atk_sq = popBit(&attackSet);
			_capMoveList.push_back(Move(MOVE(sq, atk_sq, b.pieceAt(atk_sq), EMPTY, EMPTY), 0));
		}
	}

	// pinned rooks only move in pinned direction
	rooks = b.getPieces(ROOK, side) & pinned;
	while (rooks) {
		sq = popBit(&rooks);
		attackSet = lookUpRookMoves(sq, b.occupied) & lineBB[sq][kSq];
		attackSet &= b.color[side ^ 1];
		while (attackSet) {
			atk_sq = popBit(&attackSet);
			_capMoveList.push_back(Move(MOVE(sq, atk_sq, b.pieceAt(atk_sq), EMPTY, EMPTY), 0));
		}
	}
}

void LegalMoveGenerator::_addBishopCaptures(Board b, const int side) {
	int sq, atk_sq, kSq = b.getKingSquare(side);
	U64 attackSet;
	U64 pinned = b.pinned(kSq, side);

	U64 bishops = b.getPieces(BISHOP, side) & ~pinned;
	while (bishops) {
		sq = popBit(&bishops);
		attackSet = lookUpBishopMoves(sq, b.occupied);
		attackSet &= b.color[side ^ 1];

		while (attackSet) {
			atk_sq = popBit(&attackSet);
			_capMoveList.push_back(Move(MOVE(sq, atk_sq, b.pieceAt(atk_sq), EMPTY, EMPTY), 0));
		}
	}

	// pinned bishops only move in pinned direction
	bishops = b.getPieces(BISHOP, side) & pinned;
	while (bishops) {
		sq = popBit(&bishops);
		attackSet = lookUpBishopMoves(sq, b.occupied) & lineBB[sq][kSq];
		attackSet &= b.color[side ^ 1];
		while (attackSet) {
			atk_sq = popBit(&attackSet);
			_capMoveList.push_back(Move(MOVE(sq, atk_sq, b.pieceAt(atk_sq), EMPTY, EMPTY), 0));
		}
	}
}

void LegalMoveGenerator::_addQueenMoves(Board b, const int side) {
	int sq, atk_sq, kSq = b.getKingSquare(side);
	U64 attackSet;
	U64 pinned = b.pinned(kSq, side);

	U64 queens = b.getPieces(QUEEN, side) & ~pinned;
	while (queens) {
		sq = popBit(&queens);
		attackSet = lookUpRookMoves(sq, b.occupied) ^ lookUpBishopMoves(sq, b.occupied);
		attackSet &= ~b.occupied;

		while (attackSet) {
			atk_sq = popBit(&attackSet);
			_quietMoveList.push_back(Move(MOVE(sq, atk_sq, EMPTY, EMPTY, EMPTY), 0));
		}
	}

	queens = b.getPieces(QUEEN, side) & pinned;
	while (queens) {
		sq = popBit(&queens);
		attackSet = lookUpRookMoves(sq, b.occupied) ^ lookUpBishopMoves(sq, b.occupied);
		attackSet &= ~b.occupied & lineBB[sq][kSq];

		while (attackSet) {
			atk_sq = popBit(&attackSet);
			_quietMoveList.push_back(Move(MOVE(sq, atk_sq, EMPTY, EMPTY, EMPTY), 0));
		}
	}
}

void LegalMoveGenerator::_addQueenCaptures(Board b, const int side) {
	int sq, atk_sq, kSq = b.getKingSquare(side);
	U64 attackSet;
	U64 pinned = b.pinned(kSq, side);

	U64 queens = b.getPieces(QUEEN, side) & ~pinned;
	while (queens) {
		sq = popBit(&queens);
		attackSet = lookUpRookMoves(sq, b.occupied) ^ lookUpBishopMoves(sq, b.occupied);
		attackSet &= b.color[side ^ 1];

		while (attackSet) {
			atk_sq = popBit(&attackSet);
			_capMoveList.push_back(Move(MOVE(sq, atk_sq, b.pieceAt(atk_sq), EMPTY, EMPTY), 0));
		}
	}

	queens = b.getPieces(QUEEN, side) & pinned;
	while (queens) {
		sq = popBit(&queens);
		attackSet = lookUpRookMoves(sq, b.occupied) ^ lookUpBishopMoves(sq, b.occupied);
		attackSet &= b.color[side ^ 1] & lineBB[sq][kSq];

		while (attackSet) {
			atk_sq = popBit(&attackSet);
			_capMoveList.push_back(Move(MOVE(sq, atk_sq, b.pieceAt(atk_sq), EMPTY, EMPTY), 0));
		}
	}
}

void LegalMoveGenerator::_printGeneratedMoves(Board b) {

	cout << "\nGenerated " << _capMoveList.size() + _quietMoveList.size() << " moves for: " << colorString[b.side] << endl;

	vector <Move> allMoves;
	allMoves.reserve(_capMoveList.size() + _quietMoveList.size()); // preallocate memory
	allMoves.insert(allMoves.end(), _capMoveList.begin(), _capMoveList.end()); // copy
	allMoves.insert(allMoves.end(), _quietMoveList.begin(), _quietMoveList.end()); // copy

	for (Move move : allMoves) {
		printMove(move.move);
	}
}
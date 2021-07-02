#include "board.h"

// Include extern vars declared in board.h
move_t killer[2][MAX_GAME_MOVES];
move_t mateKiller[MAX_GAME_MOVES];
move_t counterHeuristic[64][64][2];

int histHeuristic[2][64][64];
int histMax = 0;

bool checkBoard(board_t* b) {
	Assert(b->castlePermission >= 0 && b->castlePermission <= 15);
	Assert(popCount(b->occupied) >= 2 && popCount(b->occupied) <= 32);
	Assert(validEnPasSq(b->enPas) || b->enPas == DEFAULT_EP_SQ);
	Assert(b->zobristKey == generateZobristKey(b));
	Assert(b->zobristPawnKey == generatePawnHashKey(b));
	return true;
}

void setPiece(board_t* b, int piece, int square, int side) {
	b->pieces[pieceType[piece]] ^= (1ULL << square);
	b->color[side] ^= (1ULL << square);
	b->occupied ^= (1ULL << square);
}

void clearPiece(board_t* b, int piece, int square, int side) {
	b->pieces[pieceType[piece]] ^= (1ULL << square);
	b->color[side] ^= (1ULL << square);
	b->occupied ^= (1ULL << square);
}

int fileRankToSq(int f, int r) {
	return 8 * r + f;
}

void reset(board_t* b) {
	b->stm = chai::WHITE;
	b->enPas = DEFAULT_EP_SQ;
	b->halfMoves = 0;
	b->ply = 0;
	b->undoPly = 0;
	b->fiftyMove = 0;
	b->zobristKey = 0x0;
	b->zobristPawnKey = 0x0;
	b->castlePermission = 0;

	// Reset all heuristics used to order moves during search.

	for (int i = 0; i < MAX_GAME_MOVES; i++) {
		mateKiller[i] = MOVE_NONE;
	}

	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 64; j++) {
			for (int k = 0; k < 2; k++) {
				counterHeuristic[i][j][k] = MOVE_NONE;
			}
		}
	}

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < NUM_SQUARES; j++) {
			for (int k = 0; k < NUM_SQUARES; k++) {
				histHeuristic[i][j][k] = 0;
			}
		}
	}
	histMax = 0;

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < MAX_DEPTH; j++) {
			killer[i][j] = MOVE_NONE;
		}
	}

	for (int i = chai::NO_TYPE; i <= chai::KING; i++) b->pieces[i] = 0ULL;

	b->color[0] = 0ULL;
	b->color[1] = 0ULL;
	b->occupied = 0ULL;
}

key_t generateZobristKey(board_t* b) {
	key_t finalZobristKey = 0;
	bitboard_t occ = b->occupied;
	int square = 0;
	int piece = 0;

	// Hash all pieces on their current square
	while (occ) {
		square = getPopLSB(&occ);
		piece = pieceAt(b, square);
		finalZobristKey ^= pieceKeys[piece][square];
	}

	// Hash in sideKey if white plays
	if (b->stm == chai::WHITE) {
		finalZobristKey ^= sideKey;
	}

	// Hash in en passant square
	finalZobristKey ^= pieceKeys[Pieces::NO_PIECE][b->enPas];

	// Hash in castlePermission
	finalZobristKey ^= castleKeys[b->castlePermission];

	Assert(finalZobristKey != 0);
	return finalZobristKey;
}

key_t generatePawnHashKey(board_t* b) {
	int sq;
	key_t finalPawnKey = 0x0;
	bitboard_t whitePawns = getPieces(b, chai::PAWN, chai::WHITE);
	bitboard_t blackPawns = getPieces(b, chai::PAWN, chai::BLACK);

	while (whitePawns) {
		sq = getPopLSB(&whitePawns);
		finalPawnKey ^= pieceKeys[Pieces::P][sq];
	}

	while (blackPawns) {
		sq = getPopLSB(&blackPawns);
		finalPawnKey ^= pieceKeys[Pieces::p][sq];
	}

	return finalPawnKey;
}

bitboard_t getPieces(board_t* b, int piece, int side) {
	return b->pieces[pieceType[piece]] & b->color[side];
}

bitboard_t getDiagPieces(board_t* b, int side) {
	return getPieces(b, chai::BISHOP, b->stm) | getPieces(b, chai::QUEEN, b->stm);
}

bitboard_t getVerticalPieces(board_t* b, int side) { //TODO side stm
	return getPieces(b, chai::ROOK, b->stm) | getPieces(b, chai::QUEEN, b->stm);
}

int pieceAt(board_t* b, int square) {
	for (int i = 0; i < 7; i++) {
		if (b->pieces[i] & (1ULL << square)) {
			return i + (b->color[chai::BLACK] & (1ULL << square) ?
						6 : 0);
		}
	}
	return 0;
}

int countMajorPieces(board_t* b, int side) {
	int cnt = 0;
	for (int i = 2; i < 7; i++) {
		cnt += popCount(getPieces(b, i, side));
	}

	return cnt;
}

void printBoard(board_t* b) {
#ifdef INFO
	int sq, file, rank, piece;

	// print board
	for (rank = RANK_8; rank >= RANK_1; rank--) {
		printf("%d  ", rank + 1);
		for (file = FILE_A; file <= FILE_H; file++) {
			sq = fileRankToSq(file, rank);
			piece = pieceAt(b, sq);
			printf("%2c", pieceChar[piece]);
		}
		printf("\n");
	}

	printf("\n   ");
	for (file = FILE_A; file <= FILE_H; file++) {
		printf("%2c", 'a' + file);
	}

	cout << "\n\nPlayer to move: " << b->stm << endl;
	printf("Zobrist key: %llX\n", b->zobristKey);
	printf("Pawn key: %llX\n", b->zobristPawnKey);
	cout << "En passant square: " << b->enPas << endl;
	cout << "Halfmoves " << b->halfMoves << ", undoPly " << b->undoPly
		<< ", ply " << b->ply << ", fiftyMoves " << b->fiftyMove << endl;

	printf("Castle permission: %c%c%c%c\n",
		   b->castlePermission & K_CASTLE ? 'K' : ' ',
		   b->castlePermission & Q_CASTLE ? 'Q' : ' ',
		   b->castlePermission & k_CASTLE ? 'k' : ' ',
		   b->castlePermission & q_CASTLE ? 'q' : ' '
	);

	cout << endl;
#endif // INFO
}

bool parseFen(board_t* board, std::string fen) {
	reset(board);

	// Shortest fen (2 kings, no rights) "8/8/8/k7/K7/8/8/8 w - - 0 1"
	if (fen.length() < 27) {
		return true;
	}

	int file = FILE_A, rank = RANK_8;
	int index = 0, square = 0, piece = 0, count = 0;

	while (rank >= RANK_1) {
		count = 1;
		switch (fen[index]) {
			case 'p': piece = Pieces::p; break;
			case 'r': piece = Pieces::r; break;
			case 'n': piece = Pieces::n; break;
			case 'b': piece = Pieces::b; break;
			case 'k': piece = Pieces::k; break;
			case 'q': piece = Pieces::q; break;
			case 'P': piece = Pieces::P; break;
			case 'R': piece = Pieces::R; break;
			case 'N': piece = Pieces::N; break;
			case 'B': piece = Pieces::B; break;
			case 'K': piece = Pieces::K; break;
			case 'Q': piece = Pieces::Q; break;

			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
				piece = chai::NO_TYPE;
				count = fen[index] - '0';
				break;

			case '/':
			case ' ':
				rank--;
				file = FILE_A;
				index++;
				continue;

			default:
				cout << "FEN error: " << fen[index] << endl;
				return true;
		}

		for (int i = 0; i < count; i++) {
			square = rank * 8 + file;
			if (piece != chai::NO_TYPE) {
				setPiece(board, piece, square, pieceCol[piece]);
			}
			file++;
		}
		index++;
	}

	// assert for correct position
	Assert(fen[index] == 'w' || fen[index] == 'b');
	board->stm = (fen[index] == 'w') ? chai::WHITE : chai::BLACK;
	index += 2;

	// castle permission
	for (int i = 0; i < 4; i++) {
		if (fen[index] == ' ') {
			break;
		}
		switch (fen[index]) {
			case 'K': board->castlePermission |= K_CASTLE; break;
			case 'Q': board->castlePermission |= Q_CASTLE; break;
			case 'k': board->castlePermission |= k_CASTLE; break;
			case 'q': board->castlePermission |= q_CASTLE; break;
			default: break;
		}
		index++;
	}
	index++;
	Assert(board->castlePermission >= 0 && board->castlePermission <= 15);

	// en passant square
	if (fen[index] != '-') {
		file = fen[index] - 'a';
		rank = fen[index + 1] - '1';

		Assert(file >= FILE_A && file <= FILE_H);
		Assert(rank >= RANK_1 && rank <= RANK_8);

		board->enPas = fileRankToSq(file, rank);
		index += 3;
	} else {
		index += 2;
	}

	board->fiftyMove += atoi(&fen[index]);

	index += 2;

	std::string fullMoveStr = "";
	while (fen[index]) {
		fullMoveStr += fen[index];
		index++;
	}

	board->halfMoves += std::stoi(fullMoveStr) * 2;

	board->zobristPawnKey = generatePawnHashKey(board);
	board->zobristKey = generateZobristKey(board);

	checkBoard(board);

	return false;
}

std::string getFEN(board_t* b) {
	int piece;
	int empty = 0;
	std::string fen = "";

	int i = 0;
	int r = RANK_8;
	int f = FILE_A;

	while (r >= RANK_1) {

		f = FILE_A;
		while (f <= FILE_H) {
			i = fileRankToSq(f, r);

			piece = pieceAt(b, i);
			if (pieceValid(piece)) {
				if (empty) {
					fen += std::to_string(empty);
					empty = 0;
				}
				fen += pieceChar[piece];
			} else {
				empty++;
			}

			f++;
		}

		if (empty) {
			fen += std::to_string(empty);
		}

		empty = 0;

		if (r != RANK_1) {
			fen += "/";
		}

		r--;
	}

	if (b->stm == chai::WHITE) {
		fen += " w KQkq ";
	} else {
		fen += " b KQkq ";
	}

	if (b->enPas) {
		fen += ('a' + squareToFile[b->enPas]);
		fen += ('1' + squareToRank[b->enPas]);
	}

	fen += " " + std::to_string(b->undoPly) + " " + std::to_string(b->halfMoves);

	return fen;
}

move_t parseMove(board_t* b, std::string move) {
	// trivial case for null move
	if (move == "0000") return MOVE_NULL;

	int from = fileRankToSq(move[0] - 97, move[1] - 49);
	int to = fileRankToSq(move[2] - 97, move[3] - 49);
	int movingPiece = pieceAt(b, from);
	int promPiece = 0;
	int MOVE_FLAG = NORMAL_MOVE;

	// set possible pawn flags
	if (piecePawn[movingPiece]) {
		int moveDistance = abs(from - to);

		// set ep flag if to square is en passant (ep capture)
		if (to == b->enPas && (moveDistance == 7 || moveDistance == 9)) {
			MOVE_FLAG = EP_MOVE;

		} else if (squareToRank[to] == RANK_1 || squareToRank[to] == RANK_8) {
			MOVE_FLAG = PROM_MOVE;
			switch (move[4]) {
				case 'n': promPiece = PROM_TO_KNIGHT; break;
				case 'b': promPiece = PROM_TO_BISHOP; break;
				case 'r': promPiece = PROM_TO_ROOK; break;
				default: promPiece = PROM_TO_QUEEN; break;
			}
		}
	}

	if (pieceKing[movingPiece] && abs(from - to) == 2) {
		MOVE_FLAG = CASTLE_MOVE;
	}

	return serializeMove(from, to, MOVE_FLAG, promPiece);
}

void clearCastleRights(board_t* b, int stm) {
	if (stm == chai::WHITE) {
		b->castlePermission &= ~K_CASTLE;
		b->castlePermission &= ~Q_CASTLE;
	} else if (stm == chai::BLACK) {
		b->castlePermission &= ~k_CASTLE;
		b->castlePermission &= ~q_CASTLE;
	} else {
		Assert(false);
	}
}

void pushEnPas(board_t* b, move_t move) {
	Assert(b->enPas == toSq(move));
	int fromSquare = fromSq(move);
	int toSquare = b->enPas;
	int clearSquare = toSquare + 8 - (b->stm << 4);

	int fromPiece = stmPiece[chai::PAWN][b->stm];
	int enPasPiece = stmPiece[chai::PAWN][b->stm ^ 1];

	// Update pawn key
	b->zobristPawnKey ^=
		pieceKeys[fromPiece][fromSquare]       // Remove fromSq
		^ pieceKeys[fromPiece][toSquare]       // Add toSq
		^ pieceKeys[enPasPiece][clearSquare];  // Clear captured pawn

	// Update zobrist key
	b->zobristKey ^=
		pieceKeys[fromPiece][fromSquare]
		^ pieceKeys[fromPiece][toSquare]
		^ pieceKeys[enPasPiece][clearSquare];

	clearPiece(b, chai::PAWN, clearSquare, b->stm ^ 1);
	clearPiece(b, chai::PAWN, fromSquare, b->stm);
	setPiece(b, chai::PAWN, b->enPas, b->stm);

	b->fiftyMove = 0;
	b->enPas = DEFAULT_EP_SQ;
}

void pushPromotion(board_t* b, move_t move) {
	int fromSquare = fromSq(move);
	int toSquare = toSq(move);

	int fromPiece = stmPiece[chai::PAWN][b->stm];
	int toPiece = pieceAt(b, toSquare);
	int promotedPiece = promPiece(b, move);
	Assert(pieceValidPromotion(promotedPiece));

	b->zobristPawnKey ^= pieceKeys[fromPiece][fromSquare];
	b->zobristKey ^= pieceKeys[fromPiece][fromSquare]
		^ pieceKeys[promotedPiece][toSquare];

	// In case of promoting capture
	if (toPiece) {
		b->undoHistory[b->undoPly].cap = toPiece;
		b->zobristKey ^= pieceKeys[toPiece][toSquare];
		clearPiece(b, toPiece, toSquare, b->stm ^ 1);
	}

	clearPiece(b, fromPiece, fromSquare, b->stm);
	setPiece(b, promotedPiece, toSquare, b->stm);

	b->fiftyMove = 0;
	b->enPas = DEFAULT_EP_SQ;
}

void pushCastle(board_t* b, move_t move) {
	int fromSquare = fromSq(move);
	int toSquare = toSq(move);
	int rClearSq, rSetSq;

	int movingRook = stmPiece[chai::ROOK][b->stm];
	int movingKing = stmPiece[chai::KING][b->stm];

	switch (toSquare) {
		case C1:
			rClearSq = A1;
			rSetSq = D1;
			break;
		case G1:
			rClearSq = H1;
			rSetSq = F1;
			break;
		case C8:
			rClearSq = A8;
			rSetSq = D8;
			break;
		case G8:
			rClearSq = H8;
			rSetSq = F8;
			break;
		default:
			Assert(false);
			break;
	}

	b->zobristKey ^= pieceKeys[movingRook][rClearSq]
		^ pieceKeys[movingRook][rSetSq]
		^ pieceKeys[movingKing][fromSquare]
		^ pieceKeys[movingKing][toSquare];

	clearPiece(b, chai::ROOK, rClearSq, b->stm);
	setPiece(b, chai::ROOK, rSetSq, b->stm);

	clearPiece(b, chai::KING, fromSquare, b->stm);
	setPiece(b, chai::KING, toSquare, b->stm);

	// Update castle permission TODO clear castle for()
	b->zobristKey ^= castleKeys[b->castlePermission];
	clearCastleRights(b, b->stm);
	b->zobristKey ^= castleKeys[b->castlePermission];

	b->enPas = DEFAULT_EP_SQ;
}

void pushNormal(board_t* b, move_t move) {
	int fromSquare = fromSq(move);
	int toSquare = toSq(move);

	int fromPiece = pieceAt(b, fromSquare);
	int toPiece = pieceAt(b, toSquare);
	int capturedPiece = capPiece(b, move);

	// Update captured piece
	if (capturedPiece) {
		b->zobristKey ^= pieceKeys[capturedPiece][toSquare];
		clearPiece(b, capturedPiece, toSquare, b->stm ^ 1);

		if (piecePawn[capturedPiece]) {
			b->zobristPawnKey ^= pieceKeys[capturedPiece][toSquare];
		}

		b->undoHistory[b->undoPly].cap = capturedPiece;
		b->fiftyMove = 0;
	}

	// Update normal move
	b->zobristKey ^= pieceKeys[fromPiece][fromSquare]
		^ pieceKeys[fromPiece][toSquare];

	clearPiece(b, fromPiece, fromSquare, b->stm);
	setPiece(b, fromPiece, toSquare, b->stm);

	// Pawn start changes enPas square
	b->enPas = DEFAULT_EP_SQ;
	if (piecePawn[fromPiece] && (toSquare ^ fromSquare) == 16) {
		b->enPas = b->stm == chai::WHITE ? toSquare - 8 : toSquare + 8;
	}

	// Pawn moves reset 50-Move-Rule and change pawnKey
	if (piecePawn[fromPiece]) {
		b->zobristPawnKey ^= pieceKeys[fromPiece][fromSquare]
			^ pieceKeys[fromPiece][toSquare];
		b->fiftyMove = 0;
	}

	// Moving rooks loose their right to castle
	if (pieceRook[fromPiece]) {
		b->zobristKey ^= castleKeys[b->castlePermission];
		switch (fromSquare) {
			case A1: b->castlePermission &= ~Q_CASTLE; break;
			case H1: b->castlePermission &= ~K_CASTLE; break;
			case A8: b->castlePermission &= ~q_CASTLE; break;
			case H8: b->castlePermission &= ~k_CASTLE; break;
			default: break;
		}
		b->zobristKey ^= castleKeys[b->castlePermission];
	}

	// Moving kings loose their right to castle
	if (pieceKing[fromPiece]) {
		b->zobristKey ^= castleKeys[b->castlePermission];
		clearCastleRights(b, b->stm);
		b->zobristKey ^= castleKeys[b->castlePermission];
	}

}

bool push(board_t* b, move_t move) {
	Assert(b->enPas == DEFAULT_EP_SQ || validEnPasSq(b->enPas));
	Assert(b->undoPly >= 0 && b->undoPly <= MAX_GAME_MOVES);

	// Store data that is not worth recomputing
	undo_t* undo = &b->undoHistory[b->undoPly];
	undo->enPas = b->enPas;
	undo->castle = b->castlePermission;
	undo->zobKey = b->zobristKey;
	undo->pawnKey = b->zobristPawnKey;
	undo->move = move;
	undo->cap = Pieces::NO_PIECE; // Might get updated by push[MoveType]
	undo->fiftyMove = b->fiftyMove;

	// Always let helper functions determine next ep square
	b->zobristKey ^= pieceKeys[Pieces::NO_PIECE][b->enPas];

	// Always increment 50-move counter: Reset in helper functions
	b->fiftyMove++;

	int moveType = move & (3 << 12);

	// Execute helper function depending on move types
	if (moveType == NORMAL_MOVE)
		pushNormal(b, move);

	if (moveType == EP_MOVE)
		pushEnPas(b, move);

	if (moveType == CASTLE_MOVE)
		pushCastle(b, move);

	if (moveType == PROM_MOVE)
		pushPromotion(b, move);

	// Hash in new EP square
	b->zobristKey ^= pieceKeys[Pieces::NO_PIECE][b->enPas];

	// Update game state variables
	b->stm ^= 1;
	b->zobristKey ^= sideKey;
	b->halfMoves++;
	b->undoPly++;
	b->ply++;

	Assert(b->zobristPawnKey == generatePawnHashKey(b));
	Assert(b->zobristKey == generateZobristKey(b));

	// Check if move leaves king in check
	if (isCheck(b, b->stm ^ 1)) {
		pop(b);
		return false;
	}

	return true;
}

void pushNull(board_t* b) {
	Assert(!isCheck(b, b->stm));

	undo_t* undo = &b->undoHistory[b->undoPly];
	undo->enPas = b->enPas;
	undo->castle = b->castlePermission;
	undo->zobKey = b->zobristKey;
	undo->pawnKey = b->zobristPawnKey;
	undo->move = MOVE_NULL;
	undo->cap = chai::NO_TYPE;

	b->zobristKey ^= pieceKeys[Pieces::NO_PIECE][b->enPas]; // ep out
	b->enPas = 0;
	b->zobristKey ^= pieceKeys[Pieces::NO_PIECE][b->enPas]; // ep in

	// update game state variables
	b->stm ^= 1;
	b->zobristKey ^= sideKey;

	Assert(b->zobristKey == generateZobristKey(b));
	Assert(b->zobristPawnKey == generatePawnHashKey(b));

	// update gameState variable and store in undo struct
	undo->fiftyMove = b->fiftyMove;

	b->fiftyMove++;
	b->halfMoves++;
	b->undoPly++;
	b->ply++;
}

void clearCastlePermission(board_t* b, int side) {
	if (side == chai::WHITE) {
		b->castlePermission &= ~K_CASTLE;
		b->castlePermission &= ~Q_CASTLE;
	} else {
		b->castlePermission &= ~k_CASTLE;
		b->castlePermission &= ~q_CASTLE;
	}
}

undo_t pop(board_t* b) {
	b->halfMoves--;
	b->undoPly--;
	b->ply--;

	// Flip side before clear and set pieces
	b->stm ^= 1;

	// reset board variables
	undo_t* undo = &b->undoHistory[b->undoPly];
	b->castlePermission = undo->castle;
	b->fiftyMove = undo->fiftyMove;
	b->zobristKey = undo->zobKey;
	b->zobristPawnKey = undo->pawnKey;
	b->enPas = undo->enPas;

	int from_square = fromSq(undo->move);
	int to_square = toSq(undo->move);
	int movingPiece = pieceAt(b, to_square);
	int capturedPiece = undo->cap;

	// Move back moving piece
	setPiece(b, movingPiece, from_square, b->stm);
	clearPiece(b, movingPiece, to_square, b->stm);

	// Reset captured piece
	if (capturedPiece) {
		setPiece(b, capturedPiece, to_square, b->stm ^ 1);
	}

	// EP captures
	if (isEnPassant(undo->move)) {
		if (b->stm == chai::WHITE) setPiece(b, chai::PAWN, to_square - 8, b->stm ^ 1);
		else setPiece(b, chai::PAWN, to_square + 8, b->stm ^ 1);
	}

	// Promotions
	if (promPiece(b, undo->move)) {
		clearPiece(b, movingPiece, from_square, b->stm);
		setPiece(b, chai::PAWN, from_square, b->stm);
	}

	// undo castles
	if (isCastling(undo->move)) {
		switch (to_square) {
			case C1: popCastle(b, D1, A1, chai::WHITE); break;
			case G1: popCastle(b, F1, H1, chai::WHITE); break;
			case C8: popCastle(b, D8, A8, chai::BLACK); break;
			case G8: popCastle(b, F8, H8, chai::BLACK); break;
			default: Assert(0);
		}
	}

	Assert(b->undoPly >= 0);
	Assert(validEnPasSq(b->enPas) || b->enPas == DEFAULT_EP_SQ);
	Assert(checkBoard(b));
	return *undo;
}

void popCastle(board_t* b, int clearRookSq, int setRookSq, int side) {
	Assert(pieceAt(b, clearRookSq) == Pieces::R || pieceAt(b, clearRookSq) == Pieces::r);
	clearPiece(b, chai::ROOK, clearRookSq, side);
	setPiece(b, chai::ROOK, setRookSq, side);
}

undo_t popNull(board_t* b) {
	b->halfMoves--;
	b->undoPly--;
	b->ply--;
	Assert(b->undoPly >= 0);

	// Flip side before clear and set pieces
	b->stm ^= 1;

	// Reset board variables
	undo_t* undo = &b->undoHistory[b->undoPly];
	b->castlePermission = undo->castle;
	b->fiftyMove = undo->fiftyMove;
	b->zobristKey = undo->zobKey;
	b->zobristPawnKey = undo->pawnKey;
	b->enPas = undo->enPas;
	Assert(undo->move == MOVE_NULL);

	return *undo;
}

move_t getCurrentMove(board_t* b) {
	return b->undoHistory[b->undoPly - 1].move;
}

bitboard_t getPinner(board_t* b, int kSq, int kSide) {
	bitboard_t kingSlider = lookUpRookMoves(kSq, b->occupied);
	bitboard_t potPinned = kingSlider & b->color[kSide];
	bitboard_t xrays = kingSlider ^ lookUpRookMoves(kSq, b->occupied ^ potPinned);

	bitboard_t pinner = xrays & (getPieces(b, chai::QUEEN, kSide ^ 1) | (getPieces(b, chai::ROOK, kSide ^ 1)));

	kingSlider = lookUpBishopMoves(kSq, b->occupied);
	potPinned = kingSlider & b->color[kSide];
	xrays = kingSlider ^ lookUpBishopMoves(kSq, b->occupied ^ potPinned);
	pinner |= xrays & (getPieces(b, chai::QUEEN, kSide ^ 1) | (getPieces(b, chai::BISHOP, kSide ^ 1)));

	return pinner;
}

bitboard_t getPinned(board_t* b, int kSq, int kSide) {
	bitboard_t pinned = 0;

	bitboard_t kingSlider = lookUpRookMoves(kSq, b->occupied);
	bitboard_t potPinned = kingSlider & b->color[kSide];
	bitboard_t xrays = kingSlider ^ lookUpRookMoves(kSq, b->occupied ^ potPinned);
	bitboard_t pinner = xrays & (getPieces(b, chai::QUEEN, kSide ^ 1) | (getPieces(b, chai::ROOK, kSide ^ 1)));

	while (pinner) {
		int sq = getPopLSB(&pinner);
		pinned |= obstructed(sq, kSq) & b->color[kSide];
	}

	kingSlider = lookUpBishopMoves(kSq, b->occupied);
	potPinned = kingSlider & b->color[kSide];
	xrays = kingSlider ^ lookUpBishopMoves(kSq, b->occupied ^ potPinned);
	pinner = xrays & (getPieces(b, chai::QUEEN, kSide ^ 1) | (getPieces(b, chai::BISHOP, kSide ^ 1)));

	while (pinner) {
		int sq = getPopLSB(&pinner);
		pinned |= obstructed(sq, kSq) & b->color[kSide];
	}

	return pinned;
}

int getKingSquare(board_t* b, int side) {
	return getLSB(getPieces(b, chai::KING, side));
}

bitboard_t attackerSet(board_t* b, int side) {
	int sq;
	bitboard_t attackerSet = 0ULL, piece;

	// pawn attacks
	piece = getPieces(b, chai::PAWN, side);
	while (piece) {
		sq = getPopLSB(&piece);
		attackerSet |= pawnAtkMask[side][sq];
	}

	// king
	int kSq = getKingSquare(b, side);
	attackerSet |= kingAtkMask[kSq];

	// knight attacks
	piece = getPieces(b, chai::KNIGHT, side);
	while (piece) {
		sq = getPopLSB(&piece);
		attackerSet |= knightAtkMask[sq];
	}

	// bishop attacks OR in queen square
	piece = getPieces(b, chai::BISHOP, side) | getPieces(b, chai::QUEEN, side);
	while (piece) {
		sq = getPopLSB(&piece);
		attackerSet |= lookUpBishopMoves(sq, b->occupied);
	}

	// rook attacks OR in queen square
	piece = getPieces(b, chai::ROOK, side) | getPieces(b, chai::QUEEN, side);
	while (piece) {
		sq = getPopLSB(&piece);
		attackerSet |= lookUpRookMoves(sq, b->occupied);
	}

	return attackerSet;
}

bitboard_t blockerSet(board_t* b, int side, int blockSq) {
	bitboard_t piece, blockerSet = 0ULL;
	bitboard_t blockSqBoard = (1ULL << blockSq);
	int sq;

	// find pawn pushes, that block the square
	piece = getPieces(b, chai::PAWN, side);
	bitboard_t pushedPawns;
	if (side == chai::WHITE) {
		// Single push
		pushedPawns = (piece << 8) & ~b->occupied & blockSqBoard;
		blockerSet |= (pushedPawns >> 8);

		// Double Push
		pushedPawns = ((((piece & RANK_2_HEX) << 8) & ~b->occupied) << 8) & ~b->occupied & blockSqBoard;
		blockerSet |= (pushedPawns >> 16);

		// En Pas
		if (b->enPas == blockSq) {
			if ((piece << 7 & ~FILE_A_HEX) & (1ULL << b->enPas)) {
				blockerSet |= blockSqBoard >> 7;
			}
			if ((piece << 9 & ~FILE_H_HEX) & (1ULL << b->enPas)) {
				blockerSet |= blockSqBoard >> 9;
			}
		}

	} else {
		pushedPawns = (piece >> 8) & ~b->occupied & blockSqBoard;
		blockerSet |= (pushedPawns << 8);

		pushedPawns = ((((piece & RANK_7_HEX) >> 8) & ~b->occupied) >> 8) & ~b->occupied & blockSqBoard;
		blockerSet |= (pushedPawns << 16);

		if (b->enPas == blockSq) {
			if ((piece >> 7 & ~FILE_A_HEX) & (1ULL << b->enPas)) {
				blockerSet |= blockSqBoard << 7;
			}
			if ((piece >> 9 & ~FILE_H_HEX) & (1ULL << b->enPas)) {
				blockerSet |= blockSqBoard << 9;
			}
		}
	}

	piece = getPieces(b, chai::KNIGHT, side);
	while (piece) {
		sq = getPopLSB(&piece);
		if (knightAtkMask[sq] & blockSqBoard) {
			blockerSet |= (1ULL << sq);
		}
	}

	piece = getPieces(b, chai::BISHOP, side) | getPieces(b, chai::QUEEN, side);
	while (piece) {
		sq = getPopLSB(&piece);
		if (lookUpBishopMoves(sq, b->occupied) & blockSqBoard) {
			blockerSet |= (1ULL << sq);
		}
	}

	piece = getPieces(b, chai::ROOK, side) | getPieces(b, chai::QUEEN, side);
	while (piece) {
		sq = getPopLSB(&piece);
		if (lookUpRookMoves(sq, b->occupied) & blockSqBoard) {
			blockerSet |= (1ULL << sq);
		}
	}

	return blockerSet;
}

bitboard_t squareAttackedBy(board_t* b, int square, int side) {
	bitboard_t attacker = 0ULL;
	attacker |= pawnAtkMask[side ^ 1][square] & getPieces(b, chai::PAWN, side);
	attacker |= knightAtkMask[square] & getPieces(b, chai::KNIGHT, side);
	attacker |= lookUpBishopMoves(square, b->occupied) & (getPieces(b, chai::BISHOP, side) | getPieces(b, chai::QUEEN, side));
	attacker |= lookUpRookMoves(square, b->occupied) & (getPieces(b, chai::ROOK, side) | getPieces(b, chai::QUEEN, side));
	return attacker;
}

bitboard_t squareAtkDef(board_t* b, int square) {
	bitboard_t attacker = 0ULL;
	attacker |= (pawnAtkMask[b->stm ^ 1][square] | pawnAtkMask[b->stm][square]) & b->pieces[chai::PAWN];
	attacker |= knightAtkMask[square] & b->pieces[chai::KNIGHT];
	attacker |= lookUpBishopMoves(square, b->occupied) & (b->pieces[chai::BISHOP] | b->pieces[chai::QUEEN]);
	attacker |= lookUpRookMoves(square, b->occupied) & (b->pieces[chai::ROOK] | b->pieces[chai::QUEEN]);
	return attacker;
}

bitboard_t squareAtkDefOcc(board_t* b, bitboard_t occupied, int square) {
	bitboard_t attacker = 0ULL;
	attacker |= (pawnAtkMask[b->stm ^ 1][square] | pawnAtkMask[b->stm][square]) & b->pieces[chai::PAWN];
	attacker |= knightAtkMask[square] & b->pieces[chai::KNIGHT];
	attacker |= lookUpBishopMoves(square, occupied) & (b->pieces[chai::BISHOP] | b->pieces[chai::QUEEN]);
	attacker |= lookUpRookMoves(square, occupied) & (b->pieces[chai::ROOK] | b->pieces[chai::QUEEN]);
	return attacker;
}

bool isCheck(board_t* b, int side) {
	int kSq = getKingSquare(b, side);
	if (pawnAtkMask[side][kSq] & getPieces(b, chai::PAWN, side ^ 1)) {
		return true;
	}
	if (knightAtkMask[kSq] & getPieces(b, chai::KNIGHT, side ^ 1)) {
		return true;
	}
	if (lookUpBishopMoves(kSq, b->occupied) & (getPieces(b, chai::BISHOP, side ^ 1) | getPieces(b, chai::QUEEN, side ^ 1))) {
		return true;
	}
	if (lookUpRookMoves(kSq, b->occupied) & (getPieces(b, chai::ROOK, side ^ 1) | getPieces(b, chai::QUEEN, side ^ 1))) {
		return true;
	}
	return false;
}

bool sqIsBlockerForKing(board_t* b, int kSq, int movingSide, int potBlockerSq) {
	bitboard_t pinner = 0;
	bitboard_t blocker = 0;

	int sq;
	bitboard_t kingSlider = lookUpRookMoves(kSq, b->occupied);
	bitboard_t potBlocker = kingSlider & b->color[movingSide];
	bitboard_t xrays = kingSlider ^ lookUpRookMoves(kSq, b->occupied ^ potBlocker);

	pinner |= xrays & (getVerticalPieces(b, movingSide));
	while (pinner) {
		sq = getPopLSB(&pinner);
		if (obstructed(kSq, sq) & (1ULL << potBlockerSq)) return true;
	}

	kingSlider = lookUpBishopMoves(kSq, b->occupied);
	potBlocker = kingSlider & b->color[movingSide];
	xrays = kingSlider ^ lookUpBishopMoves(kSq, b->occupied ^ potBlocker);

	pinner |= xrays & (getDiagPieces(b, movingSide));
	while (pinner) {
		sq = getPopLSB(&pinner);
		if (obstructed(kSq, sq) & (1ULL << potBlockerSq)) return true;
	}

	return false;
}

bool checkingMove(board_t* b, move_t move) {
	int from = fromSq(move);
	int to = toSq(move);
	int movingPiece = pieceAt(b, from);
	int kingSq = getKingSquare(b, b->stm ^ 1);
	bitboard_t kingMask = (1ULL << kingSq);

	// 1. Direct check: atkMask hits kingSq
	switch (movingPiece) {
		case Pieces::P:
		case Pieces::p:
			if (pawnAtkMask[b->stm][to] & kingMask) return true; break;
		case Pieces::N:
		case Pieces::n:
			if (knightAtkMask[to] & kingMask) return true; break;
		case Pieces::B:
		case Pieces::b:
			if (lookUpBishopMoves(to, b->occupied) & kingMask) return true; break;
		case Pieces::R:
		case Pieces::r:
			if (lookUpRookMoves(to, b->occupied) & kingMask) return true; break;
		case Pieces::Q:
		case Pieces::q:
			if (lookUpBishopMoves(to, b->occupied) & kingMask
				|| lookUpRookMoves(to, b->occupied) & kingMask) return true; break;
	}

	// 2. Discovered attack: Slider can hit kingSq
	// If (from, to, kSq) are not aligned, check if xray from king hits any sliders
	if (!aligned(kingSq, from, to)
		&& (sqIsBlockerForKing(b, kingSq, b->stm, from))) {
		return true;
	}

	// 3. Special pawn checks 
	if (piecePawn[movingPiece]) {
		// 3.1 Promoted piece gives check
		switch (promPiece(b, move)) {
			case Pieces::N:
			case Pieces::n:
				return (knightAtkMask[to] & kingMask);
			case Pieces::B:
			case Pieces::b:
				return (lookUpBishopMoves(to, b->occupied ^ (1ULL << from)) & kingMask);
			case Pieces::R:
			case Pieces::r:
				return (lookUpRookMoves(to, b->occupied ^ (1ULL << from)) & kingMask);
			case Pieces::Q:
			case Pieces::q:
				return (lookUpBishopMoves(to, b->occupied ^ (1ULL << from)) & kingMask)
					|| (lookUpRookMoves(to, b->occupied ^ (1ULL << from)) & kingMask);
		}

		// 3.2 EnPassant check
		if (isEnPassant(move)) {
			int capSq = b->enPas + ((b->stm == chai::WHITE) ? -8 : 8);
			b->occupied ^= (1ULL << capSq);
			b->occupied ^= (1ULL << from);
			b->occupied ^= (1ULL << to);
			bool check = squareAttackedBy(b, kingSq, b->stm);
			b->occupied ^= (1ULL << capSq);
			b->occupied ^= (1ULL << to);
			b->occupied ^= (1ULL << from);
			return check;
		}
		return false;
	}

	// 4. Castled rook gives check
	if (isCastling(move)) {
		switch (to) {
			case G1: // => rook lands on F1
				return lookUpRookMoves(F1, b->occupied) & kingMask;
			case C1: // => rook lands on D1
				return lookUpRookMoves(D1, b->occupied) & kingMask;
			case G8: // => rook lands on F8
				return lookUpRookMoves(F8, b->occupied) & kingMask;
			case C8: // => rook lands on D8
				return lookUpRookMoves(D8, b->occupied) & kingMask;
			default: Assert(false); break;
		}
	}

	return false;
}

bool castleValid(board_t* b, int castle, bitboard_t* attackerSet) {
	if (isCheck(b, b->stm)) return false;

	if (!(b->castlePermission & castle)) return false;

	switch (castle) {
		case K_CASTLE:
			if ((setMask[F1] | setMask[G1]) & b->occupied) return false;
			if (pieceAt(b, H1) != Pieces::R) return false;
			if ((*attackerSet & setMask[F1]) | (*attackerSet & setMask[G1])) return false;
			break;
		case Q_CASTLE:
			if ((setMask[D1] | setMask[C1] | setMask[B1]) & b->occupied) return false;
			if (pieceAt(b, A1) != Pieces::R) return false;
			if ((*attackerSet & setMask[D1]) | (*attackerSet & setMask[C1])) return false;
			break;
		case k_CASTLE:
			if ((setMask[F8] | setMask[G8]) & b->occupied) return false;
			if (pieceAt(b, H8) != Pieces::r) return false;
			if ((*attackerSet & setMask[F8]) | (*attackerSet & setMask[G8])) return false;
			break;
		case q_CASTLE:
			if ((setMask[D8] | setMask[C8] | setMask[B8]) & b->occupied) return false;
			if (pieceAt(b, A8) != Pieces::r) return false;
			if ((*attackerSet & setMask[D8]) | (*attackerSet & setMask[C8])) return false;
			break;
		default: return false; break;
	}

	return true;
}

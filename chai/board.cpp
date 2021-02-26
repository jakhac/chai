#include "board.h"

// Include extern vars declared in board.h
key_t castleKeys[16];
move_t killer[2][MAX_GAME_MOVES];
move_t mateKiller[MAX_GAME_MOVES];
int histHeuristic[13][64];
int histMax = 0;
move_t counterHeuristic[64][64][2];


bool checkBoard(board_t* b) {

	// check castle permission
	Assert(b->castlePermission >= 0 && b->castlePermission <= 15);

	// check min/max pieces on board
	Assert(countBits(b->occupied) >= 2 && countBits(b->occupied) <= 32);

	// check valid en pas square and rank regarding side
	if (b->side == WHITE) {
		Assert(b->enPas == 0 || ((b->enPas <= H6) && (b->enPas >= A6)));
	} else {
		Assert(b->enPas == 0 || ((b->enPas <= H3) && (b->enPas >= A3)));
	}

	Assert(b->zobristKey == generateZobristKey(b));
	Assert(b->zobristPawnKey == generatePawnHashKey(b));

	return true;
}

void setPiece(board_t* b, int piece, int square, int side) {
	b->pieces[pieceType[piece]] |= setMask[square];
	b->color[side] |= setMask[square];
	b->occupied |= setMask[square];
}

void clearPiece(board_t* b, int piece, int square, int side) {
	b->pieces[pieceType[piece]] &= clearMask[square];
	b->color[side] &= clearMask[square];
	b->occupied &= clearMask[square];
}

void reset(board_t* b) {
	b->side = WHITE;
	b->enPas = 0;
	b->halfMoves = 0;
	b->ply = 0;
	b->undoPly = 0;
	b->fiftyMove = 0;
	b->zobristKey = 0x0;
	b->zobristPawnKey = 0x0;
	b->castlePermission = 0;

	for (int i = 0; i < MAX_GAME_MOVES; i++) {
		mateKiller[i] = NO_MOVE;
	}

	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 64; j++) {
			for (int k = 0; k < 2; k++) {
				counterHeuristic[i][j][k] = NO_MOVE;
			}
		}
	}

	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < 64; j++) {
			histHeuristic[i][j] = 0;
		}
	}
	histMax = 0;

	// reset history of killer heuristic
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < MAX_DEPTH; j++) {
			killer[i][j] = NO_MOVE;
		}
	}

	for (int i = NO_PIECE; i <= KING; i++) b->pieces[i] = 0ULL;

	b->color[0] = 0ULL;
	b->color[1] = 0ULL;
	b->occupied = 0ULL;
}

void initHashKeys(board_t* b) {
	// every piece on every square with a random 64bit number
	//for (int i = 0; i < 13; i++) {
	//	for (int j = 0; j < NUM_SQUARES; j++) {
	//		pieceKeys[i][j] = rand64();
	//	}
	//}

	// random key if white to move
	//sideKey = rand64();

	for (int i = 0; i < 16; i++) {
		castleKeys[i] = rand64(); // TODO fixed castle keys
	}
}

key_t generateZobristKey(board_t* b) {
	key_t finalZobristKey = 0;
	bitboard_t occ = b->occupied;
	int square = 0;
	int piece = 0;

	// hash all pieces on current square
	while (occ) {
		square = popBit(&occ);
		piece = pieceAt(b, square);
		finalZobristKey ^= pieceKeys[piece][square];
	}

	// hash in sideKey if white plays
	if (b->side == WHITE) {
		finalZobristKey ^= sideKey;
	}

	// hash in en passant square
	finalZobristKey ^= pieceKeys[EMPTY][b->enPas];

	// hash in castlePermission
	finalZobristKey ^= castleKeys[b->castlePermission];

	Assert(finalZobristKey != 0);
	return finalZobristKey;
}

key_t generatePawnHashKey(board_t* b) {
	int sq;
	key_t finalPawnKey = 0x0;
	bitboard_t whitePawns = getPieces(b, PAWN, WHITE);
	bitboard_t blackPawns = getPieces(b, PAWN, BLACK);

	while (whitePawns) {
		sq = popBit(&whitePawns);
		finalPawnKey ^= pieceKeys[P][sq];
	}

	while (blackPawns) {
		sq = popBit(&blackPawns);
		finalPawnKey ^= pieceKeys[p][sq];
	}

	return finalPawnKey;
}

bitboard_t getPieces(board_t* b, int piece, int side) {
	return b->pieces[pieceType[piece]] & b->color[side];
}

int pieceAt(board_t* b, int square) {

	for (int i = 0; i < 7; i++) {
		// find piece type
		if (b->pieces[i] & setMask[square]) {
			// find side
			if (b->color[WHITE] & setMask[square]) return i;
			if (b->color[BLACK] & setMask[square]) return i + 6;
		}
	}
	return 0;
}

int countMajorPieces(board_t* b, int side) {
	int cnt = 0;
	for (int i = 2; i < 7; i++) {
		cnt += countBits(getPieces(b, i, side));
	}

	return cnt;
}

void printBoard(board_t* b) {
	int sq, file, rank, piece;

	// print board
	for (rank = RANK_8; rank >= RANK_1; rank--) {
		printf("%d  ", rank + 1);
		for (file = FILE_A; file <= FILE_H; file++) {
			sq = file_rank_2_sq(file, rank);
			piece = pieceAt(b, sq);
			printf("%2c", pieceChar[piece]);
		}
		printf("\n");
	}

	printf("\n   ");
	for (file = FILE_A; file <= FILE_H; file++) {
		printf("%2c", 'a' + file);
	}

	cout << "\n\nPlayer to move: " << b->side << endl;
	printf("Zobrist key: %llX\n", b->zobristKey);
	printf("Pawn key: %llX\n", b->zobristPawnKey);
	cout << "En passant square: " << b->enPas << endl;
	cout << "Halfmoves " << b->halfMoves << ", undoPly " << b->undoPly << ", ply " << b->ply << endl;
	printf("Castle permission: %c%c%c%c\n",
		b->castlePermission & K_CASTLE ? 'K' : ' ',
		b->castlePermission & Q_CASTLE ? 'Q' : ' ',
		b->castlePermission & k_CASTLE ? 'k' : ' ',
		b->castlePermission & q_CASTLE ? 'q' : ' '
	);

	cout << endl;
}

bool parseFen(board_t* board, string fen) {
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
			case 'p': piece = p; break;
			case 'r': piece = r; break;
			case 'n': piece = n; break;
			case 'b': piece = b; break;
			case 'k': piece = k; break;
			case 'q': piece = q; break;
			case 'P': piece = P; break;
			case 'R': piece = R; break;
			case 'N': piece = N; break;
			case 'B': piece = B; break;
			case 'K': piece = K; break;
			case 'Q': piece = Q; break;

			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
				piece = NO_PIECE;
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
			if (piece != NO_PIECE) {
				setPiece(board, piece, square, pieceCol[piece]);
			}
			file++;
		}
		index++;
	}

	// assert for correct position
	Assert(fen[index] == 'w' || fen[index] == 'b');
	board->side = (fen[index] == 'w') ? WHITE : BLACK;
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

		board->enPas = file_rank_2_sq(file, rank);
		index += 3;
	} else {
		index += 2;
	}

	board->halfMoves += atoi(&fen[index]);
	index += 2;

	string fullMoveStr = "";
	while (fen[index]) {
		fullMoveStr += fen[index];
		index++;
	}

	board->halfMoves += stoi(fullMoveStr) * 2;

	board->zobristPawnKey = generatePawnHashKey(board);
	board->zobristKey = generateZobristKey(board);

	checkBoard(board);

	return false;
}

string getFEN(board_t* b) {
	int piece;
	int empty = 0;
	string fen = "";

	int i = 0;
	int r = RANK_8;
	int f = FILE_A;

	while (r >= RANK_1) {

		f = FILE_A;
		while (f <= FILE_H) {
			i = file_rank_2_sq(f, r);

			piece = pieceAt(b, i);
			if (pieceValid(piece)) {
				if (empty) {
					fen += to_string(empty);
					empty = 0;
				}
				fen += pieceChar[piece];
			} else {
				empty++;
			}

			f++;
		}

		if (empty) {
			fen += to_string(empty);
		}

		empty = 0;

		if (r != RANK_1) {
			fen += "/";
		}

		r--;
	}

	if (b->side == WHITE) {
		fen += " w KQkq ";
	} else {
		fen += " b KQkq ";
	}

	if (b->enPas) {
		fen += ('a' + squareToFile[b->enPas]);
		fen += ('1' + squareToRank[b->enPas]);
	}

	fen += " " + to_string(b->undoPly) + " " + to_string(b->halfMoves);

	return fen;
}

move_t parseMove(board_t* b, string move) {

	// trivial case for null move
	if (move == "0000") return -1;

	int from = file_rank_2_sq(move[0] - 97, move[1] - 49);
	int to = file_rank_2_sq(move[2] - 97, move[3] - 49);
	int movingPiece = pieceAt(b, from);
	int flag = 0, promPiece = 0;

	// set possible pawn flags
	if (piecePawn[movingPiece]) {
		int moveDistance = abs(from - to);
		// set pawnStart flag if square difference is 16
		if (moveDistance == 16) flag |= MFLAG_PS;

		// set ep flag if to square is en passant (ep capture)
		if (to == b->enPas && (moveDistance == 7 || moveDistance == 9)) flag |= MFLAG_EP;

		// set prom flag if first / last rank
		if (squareToRank[to] == RANK_1 || squareToRank[to] == RANK_8) {
			switch (move[4]) {
				case 'n': promPiece = 2; break;
				case 'b': promPiece = 3; break;
				case 'r': promPiece = 4; break;
				default: promPiece = 5; break;
			}
			// add 6 to address black piece indices
			if (b->side == BLACK) promPiece += 6;
		}
	}

	if (pieceKing[movingPiece] && abs(from - to) == 2) {
		flag |= MFLAG_CAS;
	}

	return serializeMove(from, to, pieceAt(b, to), promPiece, flag);
}

bool push(board_t* b, int move) {
	int from_square = fromSq(move), to_square = toSq(move);
	int promoted = promPiece(move);
	int movingPiece = pieceAt(b, from_square);

	//// trivial case, kings cannot be captured
	//if (capPiece(move) == K || capPiece(move) == k) {
	//	return false;
	//}

	undo_t undo_s[1]{};
	undo_s->enPas = b->enPas;
	undo_s->castle = b->castlePermission;
	undo_s->zobKey = b->zobristKey;
	undo_s->pawnKey = b->zobristPawnKey;
	undo_s->move = move;
	undo_s->fiftyMove = b->fiftyMove;

	// assert valid from to squares and pieces

	if (!(squareOnBoard(from_square))) {
		printMoveStatus(move);
	}

	Assert(squareOnBoard(from_square));
	Assert(squareOnBoard(to_square));
	Assert(pieceValid(movingPiece));
	Assert(b->undoPly >= 0 && b->undoPly <= MAX_GAME_MOVES);

	b->fiftyMove++;

	// pawn moves reset fiftyMove rule, update pawn key
	if (movingPiece == P || movingPiece == p) {
		b->zobristPawnKey ^= pieceKeys[movingPiece][to_square]; //set piece
		b->zobristPawnKey ^= pieceKeys[movingPiece][from_square]; // clear piece
		b->fiftyMove = 0;
	}

	// clear to_square and move piece
	if (MCHECK_CAP & move) {
		int captured = capPiece(move);
		Assert(captured != K && captured != k);
		b->fiftyMove = 0;
		b->zobristKey ^= pieceKeys[captured][to_square];
		clearPiece(b, captured, to_square, b->side ^ 1);

		// delete captured pawn from pawn key
		if (captured == P || captured == p) {
			b->zobristPawnKey ^= pieceKeys[captured][to_square];
		}
	}

	b->zobristKey ^= pieceKeys[movingPiece][to_square];
	setPiece(b, movingPiece, to_square, b->side);

	b->zobristKey ^= pieceKeys[movingPiece][from_square];
	clearPiece(b, movingPiece, from_square, b->side);

	// if en passant capture, delete pawn
	if (MFLAG_EP & move) {
		int sq = (b->side == WHITE) ? -8 : 8;
		sq += to_square;

		b->zobristKey ^= pieceKeys[pieceAt(b, sq)][sq];
		b->zobristPawnKey ^= pieceKeys[pieceAt(b, sq)][sq];
		clearPiece(b, PAWN, sq, b->side ^ 1);
	}

	// handle ep key
	b->zobristKey ^= pieceKeys[EMPTY][b->enPas]; // ep out
	if (MFLAG_PS & move) {
		if (b->side == WHITE) b->enPas = to_square - 8;
		else b->enPas = to_square + 8;
	} else {
		b->enPas = 0;
	}
	b->zobristKey ^= pieceKeys[EMPTY][b->enPas]; // ep in

	// handle promotions
	if (MCHECK_PROM & move) {
		b->zobristKey ^= pieceKeys[movingPiece][to_square];
		b->zobristPawnKey ^= pieceKeys[movingPiece][to_square];
		clearPiece(b, movingPiece, to_square, b->side);

		b->zobristKey ^= pieceKeys[promPiece(move)][to_square];
		setPiece(b, promPiece(move), to_square, b->side);
	}

	// handle castling and castle permission
	b->zobristKey ^= castleKeys[b->castlePermission]; // hash CA out
	if (MFLAG_CAS & move) {
		switch (to_square) {
			case C1: pushCastle(b, A1, D1, b->side); break;
			case G1: pushCastle(b, H1, F1, b->side); break;
			case C8: pushCastle(b, A8, D8, b->side); break;
			case G8: pushCastle(b, H8, F8, b->side); break;
			default: Assert(0); break;
		}
	} else if (pieceRook[movingPiece]) {
		switch (from_square) {
			case A1: b->castlePermission &= ~Q_CASTLE; break;
			case H1: b->castlePermission &= ~K_CASTLE; break;
			case A8: b->castlePermission &= ~q_CASTLE; break;
			case H8: b->castlePermission &= ~k_CASTLE; break;
			default: break;
		}
	} else if (pieceKing[movingPiece]) {
		clearCastlePermission(b, b->side);
	}
	b->zobristKey ^= castleKeys[b->castlePermission]; // hash CA in

	// update game state variables
	b->side ^= 1;
	b->zobristKey ^= sideKey;
	b->undoHistory[b->undoPly] = *undo_s;

	b->halfMoves++;
	b->undoPly++;
	b->ply++;

	Assert(b->zobristPawnKey == generatePawnHashKey(b));
	Assert(b->zobristKey == generateZobristKey(b));
	if (isCheck(b, b->side ^ 1)) {
		pop(b);
		return false;
	}

	return true;
}

void pushCastle(board_t* b, int clearRookSq, int setRookSq, int side) {
	int rook = pieceAt(b, clearRookSq);

	Assert(rook == r || rook == R);

	b->zobristKey ^= pieceKeys[rook][clearRookSq];
	clearPiece(b, ROOK, clearRookSq, side);

	b->zobristKey ^= pieceKeys[rook][setRookSq];
	setPiece(b, ROOK, setRookSq, side);

	clearCastlePermission(b, side);
}

void pushNull(board_t* b) {
	Assert(!isCheck(b, b->side));

	undo_t undo_s{};
	undo_s.enPas = b->enPas;
	undo_s.castle = b->castlePermission;
	undo_s.zobKey = b->zobristKey;
	undo_s.pawnKey = b->zobristPawnKey;
	undo_s.move = NULL_MOVE;

	b->zobristKey ^= pieceKeys[EMPTY][b->enPas]; // ep out
	b->enPas = 0;
	b->zobristKey ^= pieceKeys[EMPTY][b->enPas]; // ep in

	// update game state variables
	b->side ^= 1;
	b->zobristKey ^= sideKey;

	Assert(b->zobristKey == generateZobristKey(b));
	Assert(b->zobristPawnKey == generatePawnHashKey(b));

	// update gameState variable and store in undo struct

	undo_s.fiftyMove = b->fiftyMove;
	b->undoHistory[b->undoPly] = undo_s;

	b->fiftyMove++; // might cause negative ply in isRepetition() check
	b->halfMoves++;
	b->undoPly++;
	b->ply++;
}

void clearCastlePermission(board_t* b, int side) {
	if (side == WHITE) {
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

	Assert(b->undoPly >= 0);

	undo_t undo = b->undoHistory[b->undoPly];

	// change side before clear and set pieces
	b->side ^= 1;

	// reset board variables
	b->castlePermission = undo.castle;
	b->fiftyMove = undo.fiftyMove;
	b->zobristKey = undo.zobKey;
	b->zobristPawnKey = undo.pawnKey;
	b->enPas = undo.enPas;

	// trivial case for null moves
	if (undo.move == NULL_MOVE) {
		Assert(b->ply >= 0);
		return undo;
	}

	int from_square = fromSq(undo.move);
	int to_square = toSq(undo.move);
	int movingPiece = pieceAt(b, to_square);

	// draw back moving piece
	setPiece(b, movingPiece, from_square, b->side);
	clearPiece(b, movingPiece, to_square, b->side);

	// reset captured piece
	if (MCHECK_CAP & undo.move) {
		setPiece(b, capPiece(undo.move), to_square, b->side ^ 1);
	}

	// undo ep captures
	if (MFLAG_EP & undo.move) {
		if (b->side == WHITE) setPiece(b, PAWN, to_square - 8, b->side ^ 1);
		else setPiece(b, PAWN, to_square + 8, b->side ^ 1);
	}

	// undo promotions
	if (MCHECK_PROM & undo.move) {
		clearPiece(b, movingPiece, from_square, b->side);
		setPiece(b, PAWN, from_square, b->side);
	}

	// undo castles
	if (MFLAG_CAS & undo.move) {
		switch (to_square) {
			case C1: popCastle(b, D1, A1, WHITE); break;
			case G1: popCastle(b, F1, H1, WHITE); break;
			case C8: popCastle(b, D8, A8, BLACK); break;
			case G8: popCastle(b, F8, H8, BLACK); break;
			default: Assert(0);
		}
	}

	Assert(checkBoard(b));
	return undo;
}

void popCastle(board_t* b, int clearRookSq, int setRookSq, int side) {
	Assert(pieceAt(b, clearRookSq) == R || pieceAt(b, clearRookSq) == r);
	clearPiece(b, ROOK, clearRookSq, side);
	setPiece(b, ROOK, setRookSq, side);
}

bitboard_t getPinner(board_t* b, int kSq, int kSide) {
	bitboard_t kingSlider = lookUpRookMoves(kSq, b->occupied);
	bitboard_t potPinned = kingSlider & b->color[kSide];
	bitboard_t xrays = kingSlider ^ lookUpRookMoves(kSq, b->occupied ^ potPinned);

	bitboard_t pinner = xrays & (getPieces(b, QUEEN, kSide ^ 1) | (getPieces(b, ROOK, kSide ^ 1)));

	kingSlider = lookUpBishopMoves(kSq, b->occupied);
	potPinned = kingSlider & b->color[kSide];
	xrays = kingSlider ^ lookUpBishopMoves(kSq, b->occupied ^ potPinned);
	pinner |= xrays & (getPieces(b, QUEEN, kSide ^ 1) | (getPieces(b, BISHOP, kSide ^ 1)));

	return pinner;
}

bitboard_t getPinned(board_t* b, int kSq, int kSide) {
	bitboard_t pinned = 0;

	bitboard_t kingSlider = lookUpRookMoves(kSq, b->occupied);
	bitboard_t potPinned = kingSlider & b->color[kSide];
	bitboard_t xrays = kingSlider ^ lookUpRookMoves(kSq, b->occupied ^ potPinned);
	bitboard_t pinner = xrays & (getPieces(b, QUEEN, kSide ^ 1) | (getPieces(b, ROOK, kSide ^ 1)));

	while (pinner) {
		int sq = popBit(&pinner);
		pinned |= obstructed(sq, kSq) & b->color[kSide];
	}

	kingSlider = lookUpBishopMoves(kSq, b->occupied);
	potPinned = kingSlider & b->color[kSide];
	xrays = kingSlider ^ lookUpBishopMoves(kSq, b->occupied ^ potPinned);
	pinner = xrays & (getPieces(b, QUEEN, kSide ^ 1) | (getPieces(b, BISHOP, kSide ^ 1)));

	while (pinner) {
		int sq = popBit(&pinner);
		pinned |= obstructed(sq, kSq) & b->color[kSide];
	}

	return pinned;
}

int getKingSquare(board_t* b, int side) {
	return bitscanForward(getPieces(b, KING, side));
}

bitboard_t attackerSet(board_t* b, int side) {
	int sq;
	bitboard_t attackerSet = 0ULL, piece;

	// pawn attacks
	piece = getPieces(b, PAWN, side);
	while (piece) {
		sq = popBit(&piece);
		attackerSet |= pawnAtkMask[side][sq];
	}

	// king
	int kSq = getKingSquare(b, side);
	attackerSet |= kingAtkMask[kSq];

	// knight attacks
	piece = getPieces(b, KNIGHT, side);
	while (piece) {
		sq = popBit(&piece);
		attackerSet |= knightAtkMask[sq];
	}

	// bishop attacks OR in queen square
	piece = getPieces(b, BISHOP, side) | getPieces(b, QUEEN, side);
	while (piece) {
		sq = popBit(&piece);
		attackerSet |= lookUpBishopMoves(sq, b->occupied);
	}

	// rook attacks OR in queen square
	piece = getPieces(b, ROOK, side) | getPieces(b, QUEEN, side);
	while (piece) {
		sq = popBit(&piece);
		attackerSet |= lookUpRookMoves(sq, b->occupied);
	}

	return attackerSet;
}

bitboard_t blockerSet(board_t* b, int side, int blockSq) {
	bitboard_t piece, blockerSet = 0ULL;
	bitboard_t blockSqBoard = setMask[blockSq];
	int sq;

	// find pawn pushes, that block the square
	piece = getPieces(b, PAWN, side);
	bitboard_t pushedPawns;
	if (side == WHITE) {
		// Single push
		pushedPawns = (piece << 8) & ~b->occupied & blockSqBoard;
		blockerSet |= (pushedPawns >> 8);

		// Double Push
		pushedPawns = ((((piece & RANK_2_HEX) << 8) & ~b->occupied) << 8) & ~b->occupied & blockSqBoard;
		blockerSet |= (pushedPawns >> 16);

		// En Pas
		if (b->enPas == blockSq) {
			if ((piece << 7 & ~FILE_A_HEX) & setMask[b->enPas]) {
				blockerSet |= blockSqBoard >> 7;
			}
			if ((piece << 9 & ~FILE_H_HEX) & setMask[b->enPas]) {
				blockerSet |= blockSqBoard >> 9;
			}
		}

	} else {
		pushedPawns = (piece >> 8) & ~b->occupied & blockSqBoard;
		blockerSet |= (pushedPawns << 8);

		pushedPawns = ((((piece & RANK_7_HEX) >> 8) & ~b->occupied) >> 8) & ~b->occupied & blockSqBoard;
		blockerSet |= (pushedPawns << 16);

		if (b->enPas == blockSq) {
			if ((piece >> 7 & ~FILE_A_HEX) & setMask[b->enPas]) {
				blockerSet |= blockSqBoard << 7;
			}
			if ((piece >> 9 & ~FILE_H_HEX) & setMask[b->enPas]) {
				blockerSet |= blockSqBoard << 9;
			}
		}
	}

	piece = getPieces(b, KNIGHT, side);
	while (piece) {
		sq = popBit(&piece);
		if (knightAtkMask[sq] & blockSqBoard) {
			blockerSet |= setMask[sq];
		}
	}

	piece = getPieces(b, BISHOP, side) | getPieces(b, QUEEN, side);
	while (piece) {
		sq = popBit(&piece);
		if (lookUpBishopMoves(sq, b->occupied) & blockSqBoard) {
			blockerSet |= setMask[sq];
		}
	}

	piece = getPieces(b, ROOK, side) | getPieces(b, QUEEN, side);
	while (piece) {
		sq = popBit(&piece);
		if (lookUpRookMoves(sq, b->occupied) & blockSqBoard) {
			blockerSet |= setMask[sq];
		}
	}

	return blockerSet;
}

bitboard_t squareAttackedBy(board_t* b, int square, int side) {
	bitboard_t attacker = 0ULL;
	attacker |= pawnAtkMask[side ^ 1][square] & getPieces(b, PAWN, side);
	attacker |= knightAtkMask[square] & getPieces(b, KNIGHT, side);
	attacker |= lookUpBishopMoves(square, b->occupied) & (getPieces(b, BISHOP, side) | getPieces(b, QUEEN, side));
	attacker |= lookUpRookMoves(square, b->occupied) & (getPieces(b, ROOK, side) | getPieces(b, QUEEN, side));
	return attacker;
}

bitboard_t squareAtkDef(board_t* b, int square) {
	bitboard_t attacker = 0ULL;
	attacker |= (pawnAtkMask[b->side ^ 1][square] | pawnAtkMask[b->side][square]) & b->pieces[PAWN];
	attacker |= knightAtkMask[square] & b->pieces[KNIGHT];
	attacker |= lookUpBishopMoves(square, b->occupied) & (b->pieces[BISHOP] | b->pieces[QUEEN]);
	attacker |= lookUpRookMoves(square, b->occupied) & (b->pieces[ROOK] | b->pieces[QUEEN]);
	return attacker;
}

bool isCheck(board_t* b, int side) {
	//return squareAttackedBy(getKingSquare(side), side ^ 1);

	int kSq = getKingSquare(b, side);
	if (pawnAtkMask[side][kSq] & getPieces(b, PAWN, side ^ 1)) {
		return true;
	}
	if (knightAtkMask[kSq] & getPieces(b, KNIGHT, side ^ 1)) {
		return true;
	}
	if (lookUpBishopMoves(kSq, b->occupied) & (b, getPieces(b, BISHOP, side ^ 1) | getPieces(b, QUEEN, side ^ 1))) {
		return true;
	}
	if (lookUpRookMoves(kSq, b->occupied) & (getPieces(b, ROOK, side ^ 1) | getPieces(b, QUEEN, side ^ 1))) {
		return true;
	}
	return false;
}

// does move leave the king in check?
bool leavesKingInCheck(board_t* b, const move_t move, const bool inCheck) {
	cout << "Do not use. board_t::leavesKingInCheck did not passed perft testing.\n"; exit(0);
	// Always return true if in check, because check evasions gen only adds legal evasions
	if (inCheck) {
		return false;
	}
	Assert(!isCheck(b, b->side));


	int to = toSq(move);
	int from = fromSq(move);
	int movingPiece = pieceAt(b, from);

	// If moving piece is king, check if to square is attacked by opponent.
	if (movingPiece == K || movingPiece == k) {
		bitboard_t atks = attackerSet(b, b->side ^ 1) & setMask[to];
		return atks;
	}

	// if moving piece is pinned to king, check if toSq is within pinning line + pinner
	int kSq = getKingSquare(b, b->side);
	bitboard_t pinned = getPinned(b, kSq, b->side);
	if (pinned & setMask[fromSq(move)]) {
		bitboard_t pinner = getPinner(b, kSq, b->side);
		bitboard_t pinningLine;
		int pinnerSq;

		// find pinner for moving piece
		while (&pinner) {
			pinnerSq = popBit(&pinner);
			pinningLine = line_bb(pinnerSq, kSq);

			if (pinningLine & setMask[from]) {
				break;
			}
		}
		// check if pinned piece is moving along pinning line
		return !(pinningLine & setMask[to]);
	}

	// Check if enPas capture discovers check
	if (MCHECK_EP & move) {
		Assert(b->enPas > 0);
		Assert(to == b->enPas);
		Assert(piecePawn[movingPiece]);
		// check all obstructed lines between king and sliders with both pawns removed

		// remove both pawns and check for if king can be attacked
		int offSet = (b->side == WHITE) ? -8 : 8;
		int capPawn = pieceAt(b, b->enPas + offSet);

		clearPiece(b, capPawn, b->enPas + offSet, b->side ^ 1); // clear captured pawn
		clearPiece(b, movingPiece, from, b->side); // clear en passanting pawn

		bool legal = squareAttackedBy(b, kSq, b->side ^ 1);

		setPiece(b, capPawn, from, b->side ^ 1);
		setPiece(b, movingPiece, b->enPas + offSet, b->side);

		return legal;

		/*bitboard_t capturedPawn = setMask[enPas];
		bitboard_t diagPawn = setMask[from];
		bitboard_t slidingAttackers = b->getPieces(ROOK, side ^ 1) | b->getPieces(QUEEN, side ^ 1) | b->getPieces(BISHOP, side ^ 1);

		int sq, kSq = b->getKingSquare(side);
		bitboard_t obstructedLine;
		while (slidingAttackers) {
			sq = popBit(&slidingAttackers);
			obstructedLine = obstructed(sq, kSq);

			// Skip if there is no line between kSq and captured pawn
			if (!obstructedLine) {
				continue;
			}

			// Found discovered en passant check if:
			// Obstructed line between kSq and attacker without captured pawn is empty
			if (!(obstructedLine & (b->occupied & ~capturedPawn))) {
				printBitBoard(&obstructedLine);
				cout << "Attacker on sq " << sq << endl;
				return true;
			}
		}*/
	}

	return false;
}

bool castleValid(board_t* b, int castle, bitboard_t* attackerSet) {
	if (isCheck(b, b->side)) return false;

	if (!(b->castlePermission & castle)) return false;

	switch (castle) {
		case K_CASTLE:
			if ((setMask[F1] | setMask[G1]) & b->occupied) return false;
			if (pieceAt(b, H1) != R) return false;
			if ((*attackerSet & setMask[F1]) | (*attackerSet & setMask[G1])) return false;
			break;
		case Q_CASTLE:
			if ((setMask[D1] | setMask[C1] | setMask[B1]) & b->occupied) return false;
			if (pieceAt(b, A1) != R) return false;
			if ((*attackerSet & setMask[D1]) | (*attackerSet & setMask[C1])) return false;
			break;
		case k_CASTLE:
			if ((setMask[F8] | setMask[G8]) & b->occupied) return false;
			if (pieceAt(b, H8) != r) return false;
			if ((*attackerSet & setMask[F8]) | (*attackerSet & setMask[G8])) return false;
			break;
		case q_CASTLE:
			if ((setMask[D8] | setMask[C8] | setMask[B8]) & b->occupied) return false;
			if (pieceAt(b, A8) != r) return false;
			if ((*attackerSet & setMask[D8]) | (*attackerSet & setMask[C8])) return false;
			break;
		default: return false; break;
	}

	return true;
}

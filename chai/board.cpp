#include "board.h"

// include extern vars
bitboard_t setMask[64];
bitboard_t clearMask[64];

int squareToRank[64];
int squareToFile[64];

bitboard_t pawnAtkMask[2][64];
bitboard_t knightAtkMask[64];
bitboard_t kingAtkMask[64];

bitboard_t dirBitmap[64][8];
bitboard_t inBetween[64][64];
int dirFromTo[64][64];
bitboard_t lineBB[64][64];


bool Board::checkBoard() {

	// check castle permission
	Assert(castlePermission >= 0 && castlePermission <= 15);

	// check min/max pieces on board
	Assert(countBits(occupied) >= 2 && countBits(occupied) <= 32);

	// check valid en pas square and rank regarding side
	if (side == WHITE) {
		Assert(enPas == 0 || ((enPas <= H6) && (enPas >= A6)));
	} else {
		Assert(enPas == 0 || ((enPas <= H3) && (enPas >= A3)));
	}

	Assert(zobristKey == generateZobristKey());
	Assert(zobristPawnKey == generatePawnHashKey());

	return true;
}

void Board::setPiece(int piece, int square, int side) {
	pieces[pieceType[piece]] |= setMask[square];
	color[side] |= setMask[square];
	occupied |= setMask[square];
}

void Board::clearPiece(int piece, int square, int side) {
	pieces[pieceType[piece]] &= clearMask[square];
	color[side] &= clearMask[square];
	occupied &= clearMask[square];
}

void Board::reset() {
	side = WHITE;
	enPas = 0;
	halfMoves = 0;
	ply = 0;
	undoPly = 0;
	fiftyMove = 0;
	zobristKey = 0x0;
	zobristPawnKey = 0x0;
	castlePermission = 0;

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

	color[0] = 0ULL;
	color[1] = 0ULL;

	for (int i = NO_PIECE; i <= KING; i++) pieces[i] = 0ULL;

	occupied = 0ULL;
}

void Board::initHashKeys() {
	// every piece on every square with a random 64bit number
	//for (int i = 0; i < 13; i++) {
	//	for (int j = 0; j < NUM_SQUARES; j++) {
	//		pieceKeys[i][j] = rand64();
	//	}
	//}

	// random key if white to move
	//sideKey = rand64();

	for (int i = 0; i < 16; i++) {
		castleKeys[i] = rand64();
	}
}

bitboard_t Board::generateZobristKey() {
	bitboard_t finalZobristKey = 0;
	bitboard_t occ = occupied;
	int square = 0;
	int piece = 0;

	// hash all pieces on current square
	while (occ) {
		square = popBit(&occ);
		piece = pieceAt(square);
		finalZobristKey ^= pieceKeys[piece][square];
	}

	// hash in sideKey if white plays
	if (side == WHITE) {
		finalZobristKey ^= sideKey;
	}

	// hash in en passant square
	finalZobristKey ^= pieceKeys[EMPTY][enPas];

	// hash in castlePermission
	finalZobristKey ^= castleKeys[castlePermission];

	return finalZobristKey;
}

bitboard_t Board::generatePawnHashKey() {
	int sq;
	bitboard_t finalPawnKey = 0x0;
	bitboard_t whitePawns = getPieces(PAWN, WHITE);
	bitboard_t blackPawns = getPieces(PAWN, BLACK);

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

bitboard_t Board::getPieces(int piece, int side) {
	return pieces[pieceType[piece]] & color[side];
}

int Board::pieceAt(int square) {

	for (int i = 0; i < 7; i++) {
		// find piece type
		if (pieces[i] & setMask[square]) {
			// find side
			if (color[WHITE] & setMask[square]) return i;
			if (color[BLACK] & setMask[square]) return i + 6;
		}
	}
	return 0;
}

int Board::countMajorPieces(int side) {
	int cnt = 0;
	for (int i = 2; i < 7; i++) {
		cnt += countBits(getPieces(i, side));
	}

	return cnt;
}

void Board::printBoard() {
	int sq, file, rank, piece;

	// print board
	for (rank = RANK_8; rank >= RANK_1; rank--) {
		printf("%d  ", rank + 1);
		for (file = FILE_A; file <= FILE_H; file++) {
			sq = file_rank_2_sq(file, rank);
			piece = pieceAt(sq);
			printf("%2c", pieceChar[piece]);
		}
		printf("\n");
	}

	printf("\n   ");
	for (file = FILE_A; file <= FILE_H; file++) {
		printf("%2c", 'a' + file);
	}

	cout << "\n\nPlayer to move: " << side << endl;
	printf("Zobrist key: %llX\n", zobristKey);
	printf("Pawn key: %llX\n", zobristPawnKey);
	cout << "En passant square: " << enPas << endl;
	cout << "Halfmoves " << halfMoves << ", undoPly " << undoPly << ", ply " << ply << endl;
	printf("Castle permission: %c%c%c%c\n",
		castlePermission & K_CASTLE ? 'K' : ' ',
		castlePermission & Q_CASTLE ? 'Q' : ' ',
		castlePermission & k_CASTLE ? 'k' : ' ',
		castlePermission & q_CASTLE ? 'q' : ' '
	);

	cout << endl;
}

void Board::parseFen(string fen) {
	reset();

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
				return;
		}

		for (int i = 0; i < count; i++) {
			square = rank * 8 + file;
			if (piece != NO_PIECE) {
				setPiece(piece, square, pieceCol[piece]);
			}
			file++;
		}
		index++;
	}

	// assert for correct position
	Assert(fen[index] == 'w' || fen[index] == 'b');
	side = (fen[index] == 'w') ? WHITE : BLACK;
	index += 2;

	// castle permission
	for (int i = 0; i < 4; i++) {
		if (fen[index] == ' ') {
			break;
		}
		switch (fen[index]) {
			case 'K': castlePermission |= K_CASTLE; break;
			case 'Q': castlePermission |= Q_CASTLE; break;
			case 'k': castlePermission |= k_CASTLE; break;
			case 'q': castlePermission |= q_CASTLE; break;
			default: break;
		}
		index++;
	}
	index++;
	Assert(castlePermission >= 0 && castlePermission <= 15);

	// en passant square
	if (fen[index] != '-') {
		file = fen[index] - 'a';
		rank = fen[index + 1] - '1';

		Assert(file >= FILE_A && file <= FILE_H);
		Assert(rank >= RANK_1 && rank <= RANK_8);

		enPas = file_rank_2_sq(file, rank);
		index += 3;
	} else {
		index += 2;
	}

	halfMoves += atoi(&fen[index]);
	index += 2;

	string fullMoveStr = "";
	while (fen[index]) {
		fullMoveStr += fen[index];
		index++;
	}

	halfMoves += stoi(fullMoveStr) * 2;

	zobristPawnKey = generatePawnHashKey();
	zobristKey = generateZobristKey();

	checkBoard();
	log("parseFen() finished.");
}

int Board::parseMove(string move) {

	// trivial case for null move
	if (move == "0000") return -1;

	int from = file_rank_2_sq(move[0] - 97, move[1] - 49);
	int to = file_rank_2_sq(move[2] - 97, move[3] - 49);
	int movingPiece = pieceAt(from);
	int flag = 0, promPiece = 0;

	// set possible pawn flags
	if (piecePawn[movingPiece]) {
		int moveDistance = abs(from - to);
		// set pawnStart flag if square difference is 16
		if (moveDistance == 16) flag |= MFLAG_PS;

		// set ep flag if to square is en passant (ep capture)
		if (to == enPas && (moveDistance == 7 || moveDistance == 9)) flag |= MFLAG_EP;

		// set prom flag if first / last rank
		if (squareToRank[to] == RANK_1 || squareToRank[to] == RANK_8) {
			switch (move[4]) {
				case 'n': promPiece = 2; break;
				case 'b': promPiece = 3; break;
				case 'r': promPiece = 4; break;
				default: promPiece = 5; break;
			}
			// add 6 to address black piece indices
			if (side == BLACK) promPiece += 6;
		}
	}

	if (pieceKing[movingPiece] && abs(from - to) == 2) {
		flag |= MFLAG_CAS;
	}

	return serializeMove(from, to, pieceAt(to), promPiece, flag);
}

bool Board::push(int move) {
	int from_square = fromSq(move), to_square = toSq(move);
	int promoted = promPiece(move);
	int movingPiece = pieceAt(from_square);

	// trivial case, kings cannot be captured
	if (capPiece(move) == K || capPiece(move) == k) {
		return false;
	}

	undo_t undo_s[1]{};
	undo_s->enPas = enPas;
	undo_s->castle = castlePermission;
	undo_s->zobKey = zobristKey;
	undo_s->pawnKey = zobristPawnKey;
	undo_s->move = move;
	undo_s->fiftyMove = fiftyMove;

	// assert valid from to squares and pieces

	if (!(squareOnBoard(from_square))) {
		printMoveStatus(move);
	}

	Assert(squareOnBoard(from_square));
	Assert(squareOnBoard(to_square));
	Assert(pieceValid(movingPiece));
	Assert(undoPly >= 0 && undoPly <= MAX_GAME_MOVES);

	fiftyMove++;

	// pawn moves reset fiftyMove rule, update pawn key
	if (movingPiece == P || movingPiece == p) {
		zobristPawnKey ^= pieceKeys[movingPiece][to_square]; //set piece
		zobristPawnKey ^= pieceKeys[movingPiece][from_square]; // clear piece
		fiftyMove = 0;
	}

	// clear to_square and move piece
	if (MCHECK_CAP & move) {
		int captured = capPiece(move);
		Assert(captured != K && captured != k);
		fiftyMove = 0;
		zobristKey ^= pieceKeys[captured][to_square];
		clearPiece(captured, to_square, side ^ 1);

		// delete captured pawn from pawn key
		if (captured == P || captured == p) {
			zobristPawnKey ^= pieceKeys[captured][to_square];
		}
	}

	zobristKey ^= pieceKeys[movingPiece][to_square];
	setPiece(movingPiece, to_square, side);

	zobristKey ^= pieceKeys[movingPiece][from_square];
	clearPiece(movingPiece, from_square, side);

	// if en passant capture, delete pawn
	if (MFLAG_EP & move) {
		int sq = (side == WHITE) ? -8 : 8;
		sq += to_square;

		zobristKey ^= pieceKeys[pieceAt(sq)][sq];
		zobristPawnKey ^= pieceKeys[pieceAt(sq)][sq];
		clearPiece(PAWN, sq, side ^ 1);
	}

	// handle ep key
	zobristKey ^= pieceKeys[EMPTY][enPas]; // ep out
	if (MFLAG_PS & move) {
		if (side == WHITE) enPas = to_square - 8;
		else enPas = to_square + 8;
	} else {
		enPas = 0;
	}
	zobristKey ^= pieceKeys[EMPTY][enPas]; // ep in

	// handle promotions
	if (MCHECK_PROM & move) {
		zobristKey ^= pieceKeys[movingPiece][to_square];
		zobristPawnKey ^= pieceKeys[movingPiece][to_square];
		clearPiece(movingPiece, to_square, side);

		zobristKey ^= pieceKeys[promPiece(move)][to_square];
		setPiece(promPiece(move), to_square, side);
	}

	// handle castling and castle permission
	zobristKey ^= castleKeys[castlePermission]; // hash CA out
	if (MFLAG_CAS & move) {
		switch (to_square) {
			case C1: pushCastle(A1, D1, side); break;
			case G1: pushCastle(H1, F1, side); break;
			case C8: pushCastle(A8, D8, side); break;
			case G8: pushCastle(H8, F8, side); break;
			default: Assert(0); break;
		}
	} else if (pieceRook[movingPiece]) {
		switch (from_square) {
			case A1: castlePermission &= ~Q_CASTLE; break;
			case H1: castlePermission &= ~K_CASTLE; break;
			case A8: castlePermission &= ~q_CASTLE; break;
			case H8: castlePermission &= ~k_CASTLE; break;
			default: break;
		}
	} else if (pieceKing[movingPiece]) {
		clearCastlePermission(side);
	}
	zobristKey ^= castleKeys[castlePermission]; // hash CA in

	// update game state variables
	side ^= 1;
	zobristKey ^= sideKey;
	undoHistory[undoPly] = *undo_s;

	halfMoves++;
	undoPly++;
	ply++;

	Assert(zobristPawnKey == generatePawnHashKey());
	Assert(zobristKey == generateZobristKey());
	if (isCheck(side ^ 1)) {
		pop();
		return false;
	}

	return true;
}

void Board::pushCastle(int clearRookSq, int setRookSq, int side) {
	int rook = pieceAt(clearRookSq);

	Assert(rook == r || rook == R);

	zobristKey ^= pieceKeys[rook][clearRookSq];
	clearPiece(ROOK, clearRookSq, side);

	zobristKey ^= pieceKeys[rook][setRookSq];
	setPiece(ROOK, setRookSq, side);

	clearCastlePermission(side);
}

void Board::pushNull() {
	Assert(!isCheck(side));

	undo_t undo_s[1]{};
	undo_s->enPas = enPas;
	undo_s->castle = castlePermission;
	undo_s->zobKey = zobristKey;
	undo_s->pawnKey = zobristPawnKey;
	undo_s->move = NULL_MOVE;

	zobristKey ^= pieceKeys[EMPTY][enPas]; // ep out
	enPas = 0;
	zobristKey ^= pieceKeys[EMPTY][enPas]; // ep in

	// update game state variables
	side ^= 1;
	zobristKey ^= sideKey;

	Assert(zobristKey == generateZobristKey());
	Assert(zobristPawnKey == generatePawnHashKey());

	// update gameState variable and store in undo struct

	undo_s->fiftyMove = fiftyMove;
	undoHistory[undoPly] = *undo_s;

	fiftyMove++; // might cause negative ply in isRepetition() check
	halfMoves++;
	undoPly++;
	ply++;
}

void Board::clearCastlePermission(int side) {
	if (side == WHITE) {
		castlePermission &= ~K_CASTLE;
		castlePermission &= ~Q_CASTLE;
	} else {
		castlePermission &= ~k_CASTLE;
		castlePermission &= ~q_CASTLE;
	}
}

undo_t Board::pop() {
	halfMoves--;
	undoPly--;
	ply--;

	Assert(undoPly >= 0);

	undo_t undo = undoHistory[undoPly];

	// change side before clear and set pieces
	side ^= 1;

	// reset board variables
	castlePermission = undo.castle;
	fiftyMove = undo.fiftyMove;
	zobristKey = undo.zobKey;
	zobristPawnKey = undo.pawnKey;
	enPas = undo.enPas;

	// trivial case for null moves
	if (undo.move == -1) {
		Assert(ply >= 0);
		return undo;
	}

	int from_square = fromSq(undo.move);
	int to_square = toSq(undo.move);
	int movingPiece = pieceAt(to_square);

	// draw back moving piece
	setPiece(movingPiece, from_square, side);
	clearPiece(movingPiece, to_square, side);

	// reset captured piece
	if (MCHECK_CAP & undo.move) {
		setPiece(capPiece(undo.move), to_square, side ^ 1);
	}

	// undo ep captures
	if (MFLAG_EP & undo.move) {
		if (side == WHITE) setPiece(PAWN, to_square - 8, side ^ 1);
		else setPiece(PAWN, to_square + 8, side ^ 1);
	}

	// undo promotions
	if (MCHECK_PROM & undo.move) {
		clearPiece(movingPiece, from_square, side);
		setPiece(PAWN, from_square, side);
	}

	// undo castles
	if (MFLAG_CAS & undo.move) {
		switch (to_square) {
			case C1: popCastle(D1, A1, WHITE); break;
			case G1: popCastle(F1, H1, WHITE); break;
			case C8: popCastle(D8, A8, BLACK); break;
			case G8: popCastle(F8, H8, BLACK); break;
			default: Assert(0);
		}
	}

	Assert(checkBoard());
	return undo;
}

void Board::popCastle(int clearRookSq, int setRookSq, int side) {
	Assert(pieceAt(clearRookSq) == R || pieceAt(clearRookSq) == r);
	clearPiece(ROOK, clearRookSq, side);
	setPiece(ROOK, setRookSq, side);
}

//TODO combine pinner and pinned
bitboard_t Board::pinner(int kSq, int kSide) {
	bitboard_t kingSlider = lookUpRookMoves(kSq, occupied);
	bitboard_t potPinned = kingSlider & color[kSide];
	bitboard_t xrays = kingSlider ^ lookUpRookMoves(kSq, occupied ^ potPinned);

	bitboard_t pinner = xrays & (getPieces(QUEEN, kSide ^ 1) | (getPieces(ROOK, kSide ^ 1)));

	kingSlider = lookUpBishopMoves(kSq, occupied);
	potPinned = kingSlider & color[kSide];
	xrays = kingSlider ^ lookUpBishopMoves(kSq, occupied ^ potPinned);
	pinner |= xrays & (getPieces(QUEEN, kSide ^ 1) | (getPieces(BISHOP, kSide ^ 1)));

	return pinner;
}

bitboard_t Board::pinned(int kSq, int kSide) {
	bitboard_t pinned = 0;

	bitboard_t kingSlider = lookUpRookMoves(kSq, occupied);
	bitboard_t potPinned = kingSlider & color[kSide];
	bitboard_t xrays = kingSlider ^ lookUpRookMoves(kSq, occupied ^ potPinned);
	bitboard_t pinner = xrays & (getPieces(QUEEN, kSide ^ 1) | (getPieces(ROOK, kSide ^ 1)));

	while (pinner) {
		int sq = popBit(&pinner);
		pinned |= obstructed(sq, kSq) & color[kSide];
	}

	kingSlider = lookUpBishopMoves(kSq, occupied);
	potPinned = kingSlider & color[kSide];
	xrays = kingSlider ^ lookUpBishopMoves(kSq, occupied ^ potPinned);
	pinner = xrays & (getPieces(QUEEN, kSide ^ 1) | (getPieces(BISHOP, kSide ^ 1)));

	while (pinner) {
		int sq = popBit(&pinner);
		pinned |= obstructed(sq, kSq) & color[kSide];
	}

	return pinned;
}

int Board::getKingSquare(int side) {
	return bitscanForward(getPieces(KING, side));
}

bitboard_t Board::attackerSet(int side) {
	int sq;
	bitboard_t attackerSet = 0ULL, piece;

	// pawn attacks
	piece = getPieces(PAWN, side);
	while (piece) {
		sq = popBit(&piece);
		attackerSet |= pawnAtkMask[side][sq];
	}

	// king
	int kSq = getKingSquare(side);
	attackerSet |= kingAtkMask[kSq];

	// knight attacks
	piece = getPieces(KNIGHT, side);
	while (piece) {
		sq = popBit(&piece);
		attackerSet |= knightAtkMask[sq];
	}

	// bishop attacks OR in queen square
	piece = getPieces(BISHOP, side) | getPieces(QUEEN, side);
	while (piece) {
		sq = popBit(&piece);
		attackerSet |= lookUpBishopMoves(sq, occupied);
	}

	// rook attacks OR in queen square
	piece = getPieces(ROOK, side) | getPieces(QUEEN, side);
	while (piece) {
		sq = popBit(&piece);
		attackerSet |= lookUpRookMoves(sq, occupied);
	}

	return attackerSet;
}

bitboard_t Board::blockerSet(int side, int blockSq) {
	bitboard_t piece, blockerSet = 0ULL;
	bitboard_t blockSqBoard = setMask[blockSq];
	int sq;

	// find pawn pushes, that block the square
	piece = getPieces(PAWN, side);
	bitboard_t pushedPawns;
	if (side == WHITE) {
		// Single push
		pushedPawns = (piece << 8) & ~occupied & blockSqBoard;
		blockerSet |= (pushedPawns >> 8);

		// Double Push
		pushedPawns = ((((piece & RANK_2_HEX) << 8) & ~occupied) << 8) & ~occupied & blockSqBoard;
		blockerSet |= (pushedPawns >> 16);

		// En Pas
		if (enPas == blockSq) {
			if ((piece << 7 & ~FILE_A_HEX) & setMask[enPas]) {
				blockerSet |= blockSqBoard >> 7;
			}
			if ((piece << 9 & ~FILE_H_HEX) & setMask[enPas]) {
				blockerSet |= blockSqBoard >> 9;
			}
		}

	} else {
		pushedPawns = (piece >> 8) & ~occupied & blockSqBoard;
		blockerSet |= (pushedPawns << 8);

		pushedPawns = ((((piece & RANK_7_HEX) >> 8) & ~occupied) >> 8) & ~occupied & blockSqBoard;
		blockerSet |= (pushedPawns << 16);

		if (enPas == blockSq) {
			if ((piece >> 7 & ~FILE_A_HEX) & setMask[enPas]) {
				blockerSet |= blockSqBoard << 7;
			}
			if ((piece >> 9 & ~FILE_H_HEX) & setMask[enPas]) {
				blockerSet |= blockSqBoard << 9;
			}
		}
	}

	piece = getPieces(KNIGHT, side);
	while (piece) {
		sq = popBit(&piece);
		if (knightAtkMask[sq] & blockSqBoard) {
			blockerSet |= setMask[sq];
		}
	}

	piece = getPieces(BISHOP, side) | getPieces(QUEEN, side);
	while (piece) {
		sq = popBit(&piece);
		if (lookUpBishopMoves(sq, occupied) & blockSqBoard) {
			blockerSet |= setMask[sq];
		}
	}

	piece = getPieces(ROOK, side) | getPieces(QUEEN, side);
	while (piece) {
		sq = popBit(&piece);
		if (lookUpRookMoves(sq, occupied) & blockSqBoard) {
			blockerSet |= setMask[sq];
		}
	}

	return blockerSet;
}

bitboard_t Board::squareAttackedBy(int square, int side) {
	bitboard_t attacker = 0ULL;
	attacker |= pawnAtkMask[side ^ 1][square] & getPieces(PAWN, side);
	attacker |= knightAtkMask[square] & getPieces(KNIGHT, side);
	attacker |= lookUpBishopMoves(square, occupied) & (getPieces(BISHOP, side) | getPieces(QUEEN, side));
	attacker |= lookUpRookMoves(square, occupied) & (getPieces(ROOK, side) | getPieces(QUEEN, side));
	return attacker;
}

bitboard_t Board::squareAtkDef(int square) {
	bitboard_t attacker = 0ULL;
	attacker |= (pawnAtkMask[side ^ 1][square] | pawnAtkMask[side][square]) & pieces[PAWN];
	attacker |= knightAtkMask[square] & pieces[KNIGHT];
	attacker |= lookUpBishopMoves(square, occupied) & (pieces[BISHOP] | pieces[QUEEN]);
	attacker |= lookUpRookMoves(square, occupied) & (pieces[ROOK] | pieces[QUEEN]);
	return attacker;
}

bitboard_t Board::isCheck(int side) {
	// TODO use early return or write gives check function
	return squareAttackedBy(getKingSquare(side), side ^ 1);
}

bool Board::castleValid(int castle, bitboard_t* attackerSet) {
	if (isCheck(side)) return false;

	if (!(castlePermission & castle)) return false;

	switch (castle) {
		case K_CASTLE:
			if ((setMask[F1] | setMask[G1]) & occupied) return false;
			if (pieceAt(H1) != R) return false;
			if ((*attackerSet & setMask[F1]) | (*attackerSet & setMask[G1])) return false;
			break;
		case Q_CASTLE:
			if ((setMask[D1] | setMask[C1] | setMask[B1]) & occupied) return false;
			if (pieceAt(A1) != R) return false;
			if ((*attackerSet & setMask[D1]) | (*attackerSet & setMask[C1])) return false;
			break;
		case k_CASTLE:
			if ((setMask[F8] | setMask[G8]) & occupied) return false;
			if (pieceAt(H8) != r) return false;
			if ((*attackerSet & setMask[F8]) | (*attackerSet & setMask[G8])) return false;
			break;
		case q_CASTLE:
			if ((setMask[D8] | setMask[C8] | setMask[B8]) & occupied) return false;
			if (pieceAt(A8) != r) return false;
			if ((*attackerSet & setMask[D8]) | (*attackerSet & setMask[C8])) return false;
			break;
		default: return false; break;
	}

	return true;
}



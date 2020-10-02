#include "board.h"

// include extern vars
U64 setMask[64];
U64 clearMask[64];

int squareToRank[64];
int squareToFile[64];

U64 pawnAtkMask[2][64];
U64 knightAtkMask[64];
U64 kingAtkMask[64];

int Board::checkBoard() {

	// check castle permission
	ASSERT(castlePermission >= 0 && castlePermission <= 15);

	// check min/max pieces on board
	ASSERT(countBits(occupied) >= 2 && countBits(occupied) <= 32);

	// check valid en pas square and rank regarding side
	if (side == WHITE) {
		ASSERT(enPas == 0 || ((enPas <= H6) && (enPas >= A6)));
	} else {
		ASSERT(enPas == 0 || ((enPas <= H3) && (enPas >= A3)));
	}

	// check non overlapping squares in bitboards
	// TODO

	// check correct zobrist hash
	ASSERT(zobristKey == generateZobristKey());

	return 1;
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
	ply = 0;
	fiftyMove = 0;
	zobristKey = 0x0;
	castlePermission = 0;

	color[0] = 0ULL;
	color[1] = 0ULL;

	for (int i = NO_PIECE; i <= KING; i++) pieces[i] = 0ULL;

	occupied = 0ULL;
}

void Board::initHashKeys() {
	// every piece on every square with a random 64bit number
	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < NUM_SQUARES; j++) {
			pieceKeys[i][j] = rand64();
		}
	}

	// random key if white to move
	sideKey = rand64();

	for (int i = 0; i < 16; i++) {
		castleKeys[i] = rand64();
	}
}

U64 Board::generateZobristKey() {
	U64 finalZobristKey = 0;


	U64 occ = occupied;
	U64 empty = ~occupied;
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

U64 Board::getPieces(int piece, int side) {
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
	cout << "En passant square: " << enPas << endl;
	printf("Castle permission: %c%c%c%c\n",
		castlePermission & K_CASTLE ? 'K' : ' ',
		castlePermission & Q_CASTLE ? 'Q' : ' ',
		castlePermission & k_CASTLE ? 'k' : ' ',
		castlePermission & q_CASTLE ? 'q' : ' '
	);
}

void Board::parseFen(string fen) {

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
	ASSERT(fen[index] == 'w' || fen[index] == 'b');
	side = fen[index] == 'w' ? WHITE : BLACK;
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
	ASSERT(castlePermission >= 0 && castlePermission <= 15);

	// en passant square
	if (fen[index] != '-') {
		file = fen[index] - 'a';
		rank = fen[index+1] - '1';
		ASSERT(file >= FILE_A && file <= FILE_H);
		ASSERT(rank >= RANK_1 && rank <= RANK_8);

		enPas = file_rank_2_sq(file, rank);
	}

	// TODO parse ply?

	//attackedSquares[WHITE] = attackerSet(WHITE);
	//attackedSquares[BLACK] = attackerSet(BLACK);
	zobristKey = generateZobristKey();
}

int Board::parseMove(string move) {

	// trivial case for null move
	if (move == "0000") return 0;

	// TODO check moveValidator for pseudo-legal move: color, capture

	int from = file_rank_2_sq(move[0] - 97, move[1] - 49);
	int to = file_rank_2_sq(move[2] - 97, move[3] - 49);
	int movingPiece = pieceAt(from);
	int flag = 0, promPiece = 0;

	// set possible pawn flags
	if (piecePawn[movingPiece]) {
		// set pawnStart flag if square difference is 16
		if (abs(from - to) == 16) flag |= MFLAGPS;

		// set ep flag if to square is en passant (ep capture)
		if (to == enPas) flag |= MFLAGEP;

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

	// TODO set castle flag
	if (pieceKing[movingPiece] && abs(from - to) == 2) {
		flag |= MFLAGCA;
	}

	return MOVE(from, to, pieceAt(to), promPiece, flag);
}

bool Board::push(int move) {
	int from_square = FROMSQ(move), to_square = TOSQ(move);
	int promoted = PROMOTED(move);
	int movingPiece = pieceAt(from_square);

	cout << "\nPush move: ";
	printMove(move);

	Undo undo;
	undo.enPas = enPas;
	undo.castle = castlePermission;
	undo.zobKey = zobristKey;
	//undo.attackedSquares[0] = attackedSquares[0];
	//undo.attackedSquares[1] = attackedSquares[1];
	undo.move = move;

	// assert valid from to squares and pieces
	ASSERT(squareOnBoard(from_square));
	ASSERT(squareOnBoard(to_square));
	ASSERT(pieceValid(movingPiece));

	// clear to_square and move piece (TODO updatePiece method?)
	if (MCHECKCAP & move) {
		zobristKey ^= pieceKeys[CAPTURED(move)][to_square];
		clearPiece(CAPTURED(move), to_square, side^1);
	}

	zobristKey ^= pieceKeys[movingPiece][to_square];
	setPiece(movingPiece, to_square, side);

	zobristKey ^= pieceKeys[movingPiece][from_square];
	clearPiece(movingPiece, from_square, side);

	// if en passant capture, delete pawn
	if (MFLAGEP & move) {
		int sq = (side == WHITE) ? -8 : 8;
		sq += to_square;

		zobristKey ^= pieceKeys[pieceAt(sq)][sq];
		clearPiece(PAWN, sq, side^1);
	}

	// handle en passant square
	zobristKey ^= pieceKeys[EMPTY][enPas]; // ep out
	if (MFLAGPS & move) {
		if (side == WHITE) enPas = to_square - 8;
		else enPas = to_square + 8;
	} else {
		enPas = 0;
	}
	zobristKey ^= pieceKeys[EMPTY][enPas]; // ep in

	// handle promotions
	if (MCHECKPROM & move) {
		zobristKey ^= pieceKeys[movingPiece][to_square];
		clearPiece(movingPiece, to_square, side);

		zobristKey ^= pieceKeys[PROMOTED(move)][to_square];
		setPiece(PROMOTED(move), to_square, side);
	}

	// handle castling and castle permission
	zobristKey ^= castleKeys[castlePermission]; // hash CA out
	if (MFLAGCA & move) {
		switch (to_square) {
			case C1: pushCastle(A1, D1, side); break;
			case G1: pushCastle(H1, F1, side); break;
			case C8: pushCastle(A8, D8, side); break;
			case G8: pushCastle(H8, F8, side); break;
			default: ASSERT(0); break;
		}
	} else if (pieceRook[movingPiece]) {
		switch (from_square) {
			case A1: castlePermission &= ~Q_CASTLE;
			case H1: castlePermission &= ~K_CASTLE;
			case A8: castlePermission &= ~q_CASTLE;
			case H8: castlePermission &= ~k_CASTLE;
			default: break;
		}
	} else if (pieceKing[movingPiece]) {
		clearCastlePermission(side);
	}
	zobristKey ^= castleKeys[castlePermission]; // hash CA in

	if (MCHECKCAP & move) {
		fiftyMove = 0;
	} else if (false) {
		// TODO stalemate, draw, win loss
	}

	// update game state variables
	side ^= 1;
	zobristKey ^= sideKey;

	ply++;
	fiftyMove++;

	printBoard();
	ASSERT(zobristKey == generateZobristKey());

	undoStack.push(undo);

	if (isCheck(side ^ 1)) {
		cout << "Move leaves " << (side^1) << " in check. Popped move from stack." << endl;
		pop();
		return 0;
	}

	return 1;
}

void Board::pushCastle(int clearRookSq, int setRookSq, int side) {
	int rook = pieceAt(clearRookSq);
	ASSERT(rook == r || rook == R);

	zobristKey ^= pieceKeys[rook][clearRookSq];
	clearPiece(ROOK, clearRookSq, side);

	zobristKey ^= pieceKeys[rook][setRookSq];
	setPiece(ROOK, setRookSq, side);

	clearCastlePermission(side);
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

Undo Board::pop() {
	ASSERT(!undoStack.empty());

	Undo undo = undoStack.top();
	undoStack.pop();

	// change side before clear and set pieces
	side ^= 1;

	// reset board variables
	castlePermission = undo.castle;
	fiftyMove = undo.fiftyMove;
	zobristKey = undo.zobKey;
	enPas = undo.enPas;

	int from_square = FROMSQ(undo.move);
	int to_square = TOSQ(undo.move);
	int movingPiece = pieceAt(to_square);

	// draw back moving piece
	setPiece(movingPiece, from_square, side);
	clearPiece(movingPiece, to_square, side);

	// reset captured piece
	if (MCHECKCAP & undo.move) {
		setPiece(CAPTURED(undo.move), to_square, side^1);
	}

	// undo ep captures
	if (MFLAGEP & undo.move) {
		if (side == WHITE) setPiece(PAWN, to_square - 8, side^1);
		else setPiece(PAWN, to_square + 8, side^1);
	}

	// undo promotions
	if (MCHECKPROM & undo.move) {
		clearPiece(movingPiece, from_square, side);
		setPiece(PAWN, from_square, side);
	}

	// undo castles
	if (MFLAGCA & undo.move) {
		switch (to_square) {
			case C1: popCastle(D1, A1, WHITE); break;
			case G1: popCastle(F1, H1, WHITE); break;
			case C8: popCastle(D8, A8, BLACK); break;
			case G8: popCastle(F8, H8, BLACK); break;
			default: ASSERT(0);
		}
	}

	cout << "\nPopped move: ";
	printMove(undo.move);
	printBoard();

	ASSERT(checkBoard());
	return undo;
}

void Board::popCastle(int clearRookSq, int setRookSq, int side) {
	ASSERT(pieceAt(clearRookSq) == R || pieceAt(clearRookSq) == r);
	clearPiece(ROOK, clearRookSq, side);
	setPiece(ROOK, setRookSq, side);
}

U64 Board::attackerSet(int side) {
	int sq;
	U64 attackerSet = 0ULL, piece;

	// pawn attacks
	piece = getPieces(PAWN, side);
	while (piece) {
		sq = popBit(&piece);
		attackerSet |= pawnAtkMask[side][sq];
	}

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

bool Board::squareAttacked(int square, int side) {
	return attackerSet(side) & setMask[square];
}

bool Board::isCheck(int side) {
	U64 kingBB = getPieces(KING, side);
	return squareAttacked(popBit(&kingBB), side^1);
}

bool Board::castleValid(int castle) {
	castle = K_CASTLE;

	if (isCheck(side)) return false;

	if (!(castlePermission & castle)) return false;

	switch (castle) {
		case K_CASTLE: 
			if (setMask[F1] & setMask[G1] & occupied) return false; 
			if (squareAttacked(F1, BLACK)) return false;
			break;
		case Q_CASTLE: 
			if (setMask[D1] & setMask[C1] & occupied) return false; 
			if (squareAttacked(D1, BLACK)) return false;
			break;
		case k_CASTLE: 
			if (setMask[F8] & setMask[G8] & occupied) return false; 
			if (squareAttacked(F8, WHITE)) return false;
			break;
		case q_CASTLE: 
			if (setMask[D8] & setMask[C8] & occupied) return false; 
			if (squareAttacked(D8, WHITE)) return false;
			break;
		default: return false; break;
	}

	return true;
}



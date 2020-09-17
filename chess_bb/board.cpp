#include "board.h"
#include <iostream>

/// <summary>
/// Counts bits of given ULL integer.
/// </summary>
int Board::countBits(U64* bb) {
	int cnt = 0;
	while (*bb) {
		cnt += *bb & 1;
		*bb >>= 1;
	}
	return cnt;
}

/// <summary>
/// Pops least significant bit and returns index.
/// </summary>
int Board::popBit(U64* bb) {
	U64 b = *bb ^ (*bb - 1);
	unsigned int fold = (unsigned)((b & 0xffffffff) ^ (b >> 32));
	*bb &= (*bb - 1);
	return bitTable[(fold * 0x783a9b23) >> 26];
}

/// <summary>
/// Set bit at given bitboard.
/// </summary>
void Board::setBit(U64* bb, int i) {
	*bb |= setMask[i];
}

/// <summary>
/// Clear bit at given bitboard.
/// </summary>
void Board::clearBit(U64* bb, int i) {
	*bb &= clearMask[i];
}

/// <summary>
/// Set bit at given index to 1 in side, occupied and piece bitboard.
/// </summary>
void Board::setPiece(int piece, int square, int side) {
	pieces[pieceType[piece]] |= setMask[square];
	color[side] |= setMask[square];
	occupied |= setMask[square];
}

/// /// <summary>
/// Set bit at given index to 0 in side, occupied and piece bitboard.
/// </summary>
void Board::clearPiece(int piece, int square, int side) {
	pieces[piece] &= clearMask[square];
	color[side] &= clearMask[square];
	occupied &= clearMask[square];
}

/// <summary>
/// Initialize all masks, keys and arrays.
/// </summary>
void Board::init() {
	initClearSetMask();
	initHashKeys();
}

/// <summary>
/// Initialize clear and set mask arrays for usage.
/// </summary>
void Board::initClearSetMask() {
	int index = 0;

	// Zero both arrays
	for (index = 0; index < 64; index++) {
		setMask[index] = 0ULL << index;
		clearMask[index] = 0ULL << index;
	}

	for (index = 0; index < 64; index++) {
		setMask[index] |= (1ULL << index);
		clearMask[index] = ~setMask[index];
	}
}

/// <summary>
/// Initialize hash keys on each square for each piece.
/// </summary>
void Board::initHashKeys() {
	// every piece on every square with a random 64bit number
	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < NUM_SQUARES; j++) {
			pieceKeys[i][j] = rand64();
		}
	}

	sideKey = rand64();
	for (int i = 0; i < 16; i++) {
		castleKeys[i] = rand64();
	}
}

/// <summary>
/// Reset board values to default.
/// </summary>
void Board::reset() {
	side = WHITE;
	enPas = -1;
	ply = 0;
	zobristKey = 0x0;
	castlePermission = 0;

	color[0] = 0ULL;
	color[1] = 0ULL;

	for (int i = NO_PIECE; i <= KING; i++) pieces[i] = 0ULL;

	occupied = 0ULL;
}

/// <summary>
/// Generate a unique key to describe current board state.
/// </summary>
U64 Board::generateZobristHash(Board* b) {
	U64 zobristKey = 0;

	// hash all pieces on current square

	// hash in side to play

	// hash in en passant square

	// hash in castlePermission


	return zobristKey;
}

/// <summary>
/// Returns a bitboard of given pieces of given side.
/// </summary>
U64 Board::getPiecesByColor(int piece, int side) {
	return pieces[piece] & color[side];
}

/// <summary>
/// Print given bitboard.
/// </summary>
void Board::printBitBoard(U64* bb) {
	U64 shiftBit = 1ULL;
	int sq;

	std::cout << std::endl;
	for (int rank = RANK_8; rank >= RANK_1; rank--) {
		for (int file = FILE_A; file <= FILE_H; file++) {
			sq = 8 * rank + file;

			if ((shiftBit << sq) & *bb) {
				std::cout << "1 ";
			} else {
				std::cout << ". ";
			}
		}
		std::cout << std::endl;
	}
}

/// <summary>
/// Return the piece index at given square.
/// </summary>
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

/// <summary>
/// Print current board with piece chars, rank and file indication.
/// </summary>
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

/// <summary>
/// Parse fen notation into bitboards and board variables.
/// </summary>
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

}

/// <summary>
/// Check board for valid bitboard entries and board variables
/// </summary>
int Board::checkBoard() {

	// check castle permission
	ASSERT(castlePermission >= 0 && castlePermission <= 15);

	// check min/max pieces on board
	ASSERT(countBits(&occupied) >= 2 && countBits(&occupied) <= 24);

	// check valid en pas square and rank regarding side
	if (side == WHITE) {
		ASSERT(enPas == -1 || ((enPas <= H6) && (enPas >= A6)));
	} else {
		ASSERT(enPas == -1 || ((enPas <= H3) && (enPas >= A3)));
	}

	// check non overlapping squares in bitboards
	ASSERT(!((color[WHITE] & pieces[PAWN]) & (color[BLACK] & pieces[PAWN])));
	ASSERT(!((color[WHITE] & pieces[KNIGHT]) & (color[BLACK] & pieces[KNIGHT])));
	ASSERT(!((color[WHITE] & pieces[BISHOP]) & (color[BLACK] & pieces[BISHOP])));
	ASSERT(!((color[WHITE] & pieces[ROOK]) & (color[BLACK] & pieces[ROOK])));
	ASSERT(!((color[WHITE] & pieces[QUEEN]) & (color[BLACK] & pieces[QUEEN])));
	ASSERT(!((color[WHITE] & pieces[KING]) & (color[BLACK] & pieces[KING])));

	return 1;
}

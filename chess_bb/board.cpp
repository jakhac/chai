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
/// Clear bit at given bitboard.
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
/// Set bit at given index to 0 in side, occupied and piece bitboard.
/// </summary>
void Board::clearPiece(int piece, int square, int side) {
	pieces[piece] &= clearMask[square];
	color[side] &= clearMask[square];
	occupied &= clearMask[square];
}

/// <summary>
/// Set bit at given index to 1 in side, occupied and piece bitboard.
/// </summary>
void Board::setPiece(int piece, int square, int side) {
	pieces[piece] |= setMask[square];
	color[side] |= setMask[square];
	occupied |= setMask[square];
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
	zobristKey = 0ULL;

	color[0] = 0ULL;
	color[1] = 0ULL;

	for (int i = NO_PIECE; i <= KING; i++) {
		pieces[i] = 0ULL;
	}

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
			}
			else {
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
			sq = 8 * rank + file;
			piece = pieceAt(sq);
			printf("%2c", pieceChar[piece]);
		}
		printf("\n");
	}

	printf("\n   ");
	for (file = FILE_A; file <= FILE_H; file++) {
		printf("%2c", 'a' + file);
	}

}

void Board::parseFen(string fen) {

	int file = FILE_A;
	int rank = RANK_8;
	int index = 0;
	int piece = 0;
	int emptySquares;

	while (rank >= RANK_1) {

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
				piece = EMPTY;
				emptySquares = fen[index] - '0';
				break;

			case '/':
			case ' ':
				rank--;
				file = FILE_A;
				index++;
				continue;

			default:
				printf("FEN error \n");
				return -1;
		}

		for (i = 0; i < emptySquares; i++) {
			sq64 = rank * 8 + file;
			sq120 = SQ_120(sq64);
			if (piece != EMPTY) {
				pos->pieces[sq120] = piece;
			}
			file++;
		}
		fen++;
	}


}

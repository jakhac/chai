#include "Board.h"

void Board::init120To64() {
	int index = 0;
	int file = FILE_A;
	int rank = RANK_1;
	int sq = A1;
	int squareCounter = 0;

	// Set both arrays non-legal values to prevent errors
	for (index = 0; index < NUM_SQUARES; index++) {
		sq120ToSq64[index] = 65;
	}

	for (rank = RANK_1; rank <= RANK_8; rank++) {
		sq64ToSq120[rank] = 120;
	}

	// Fill arrays with indices
	for (rank = RANK_1; rank <= RANK_8; ++rank) {
		for (file = FILE_A; file <= FILE_H; ++file) {
			sq = file_rank_2_sq(file, rank);

			sq120ToSq64[sq] = squareCounter;
			sq64ToSq120[squareCounter] = sq;

			squareCounter++;
		}
	}
}

void Board::printBitBoard(U64 bb) {

	U64 shiftBit = 1ULL;

	int rank = 0;
	int file = 0;
	int sq = 0;
	int sq64= 0;

	std::cout << std::endl;
	for (rank = RANK_8; rank >= RANK_1; rank--) {
		for (file = FILE_A; file <= FILE_H; file++) {
			sq = file_rank_2_sq(file, rank); // 120 based index
			sq64 = sq120ToSq64[sq]; // 63 based index

			if (shiftBit << sq64 & bb) {
				std::cout << "1 ";
			}
			else {
				std::cout << ". ";
			}
		}
		std::cout << std::endl;
	}
}

int Board::checkBoard(Board* board) {

	// set up temporary variables to capture current state
	int t_pieceNumber[13] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	int t_bigPiece[2] = { 0, 0 };
	int t_majPiece[2] = { 0, 0 };
	int t_minPiece[2] = { 0, 0 };
	int t_material[2] = { 0, 0 };

	int sq64, t_piece, t_pce_num, sq120, color, pcount;

	U64 t_pawns[3] = { 0ULL, 0ULL, 0ULL };

	t_pawns[WHITE] = board->pawns[WHITE];
	t_pawns[BLACK] = board->pawns[BLACK];
	t_pawns[BOTH] = board->pawns[BOTH];

	// check piece lists
	for (t_piece = P; t_piece <= k; t_piece++) {
		for (t_pce_num = 0; t_pce_num < board->pieceNumber[t_piece]; t_pce_num++) {
			sq120 = board->pieceList[t_piece][t_pce_num];
			ASSERT(board->pieces[sq120]==t_piece);
		}
	}

	// check piece counters
	for (sq64 = 0; sq64 < 64; sq64++) {
		sq120 = SQ_120(sq64);
		t_piece = board->pieces[sq120];
		t_pieceNumber[t_piece]++;
		color = pieceCol[t_piece];

		if (pieceBig[t_piece]) t_bigPiece[color]++;
		if (pieceMaj[t_piece]) t_majPiece[color]++;
		if (pieceMin[t_piece]) t_minPiece[color]++;

		t_material[color] += pieceVal[t_piece];
	}

	// check if counted pieces of each type are equal to stored pieces on board
	for (t_piece = P; t_piece <= k; t_piece++) {
		ASSERT(t_pieceNumber[t_piece]==board->pieceNumber[t_piece]);
	}

	// check pawn bitboards
	ASSERT(countBits(t_pawns[WHITE]) == board->pieceNumber[P]);
	ASSERT(countBits(t_pawns[BLACK]) == board->pieceNumber[p]);
	ASSERT(countBits(t_pawns[BOTH])==board->pieceNumber[p] + board->pieceNumber[P]);

	// check pawn bits and squares
	while (t_pawns[WHITE]) {
		sq64 = popBit(&t_pawns[WHITE]);
		ASSERT(P==board->pieces[SQ_120(sq64)]);
	}
	while (t_pawns[BLACK]) {
		sq64 = popBit(&t_pawns[BLACK]);
		ASSERT(p==board->pieces[SQ_120(sq64)]);
	}
	while (t_pawns[BOTH]) {
		sq64 = popBit(&t_pawns[BOTH]);
		ASSERT(P==board->pieces[SQ_120(sq64)] || p==board->pieces[SQ_120(sq64)]);
	}

	// check material counter and piece counts of each type
	ASSERT(t_material[WHITE] == board->material[WHITE] && t_material[BLACK] == board->material[BLACK]);
	ASSERT(t_minPiece[WHITE] == board->minPieces[WHITE] && t_minPiece[BLACK] == board->minPieces[BLACK]);
	ASSERT(t_majPiece[WHITE] == board->majPieces[WHITE] && t_majPiece[BLACK] == board->majPieces[BLACK]);
	ASSERT(t_bigPiece[WHITE] == board->bigPieces[WHITE] && t_bigPiece[BLACK] == board->bigPieces[BLACK]);

	// check defined side
	ASSERT(board->side == WHITE || board->side == BLACK);

	// check equal zobrist hash
	ASSERT(generateZobristHash(board) == board->posKey);

	// check if enPas is not valid or on valid rank
	ASSERT(board->enPas == NO_SQ || (rankBoard[board->enPas] == RANK_6 && board->side == WHITE) || (rankBoard[board->enPas] == RANK_3 && board->side == BLACK));

	// check king squares
	ASSERT(board->pieces[board->KingSquares[WHITE]] == K);
	ASSERT(board->pieces[board->KingSquares[BLACK]] == k);

	return true;
}

int Board::file_rank_2_sq(int f, int r) {
	return (21 + f) + (r * 10);
}

int Board::squareConstantsTo64(int square) {
	return sq120ToSq64[square];
}

int Board::countBits(U64 bb) {
	int cnt = 0;
	while (bb) {
		cnt += bb & 1;
		bb >>= 1;
	}
	return cnt;
}

int Board::popBit(U64 *bb) {
	U64 b = *bb ^ (*bb - 1);
	unsigned int fold = (unsigned)((b & 0xffffffff) ^ (b >> 32));
	*bb &= (*bb - 1);
	return bitTable[(fold * 0x783a9b23) >> 26];
}

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

void Board::clearBit(U64 bb, int square) {
	bb &= clearMask[square];
}

void Board::setBit(U64* bb, int square) {
	*bb |= setMask[square];
}

/*
Update the list of materials on the referenced board.
*/
void Board::updateListsMaterial(Board* b) {
	int piece, color;

	for (int sq = 0; sq < NUM_SQUARES; sq++) {
		piece = b->pieces[sq];
		if (piece != OFFBOARD && piece != EMPTY) {
			color = pieceCol[piece];

			// count the mterials in Pieces array according to side
			if (pieceBig[piece]) b->bigPieces[color]++;
			if (pieceMin[piece]) b->minPieces[color]++;
			if (pieceMaj[piece]) b->majPieces[color]++;

			// add up material values
			b->material[color] += pieceVal[piece];

			// store found piece in pieceList and increment for next storage
			b->pieceList[piece][b->pieceNumber[piece]] = sq;
			b->pieceNumber[piece]++;

			if (piece == K) b->KingSquares[color] = sq;
			if (piece == k) b->KingSquares[color] = sq;

			//set bits for pawns
			if (piece == P) {
				setBit(&b->pawns[WHITE], SQ_64(sq));
				setBit(&b->pawns[BOTH], SQ_64(sq));
			}
			if (piece == p) {
				setBit(&b->pawns[BLACK], SQ_64(sq));
				setBit(&b->pawns[BOTH], SQ_64(sq));
			}
		}
	}
}

/*
Initialize rank and file arrays.
*/
void Board::initRankFileArrays() {
	int square = A1;

	for (int i = 0; i < NUM_SQUARES; i++) {
		fileBoard[i] = OFFBOARD;
		rankBoard[i] = OFFBOARD;
	}

	for (int rank = RANK_1; rank <= RANK_8; rank++) {
		for (int file = FILE_A; file <= FILE_H; file++) {
			square = file_rank_2_sq(file, rank);
			fileBoard[square] = file;
			rankBoard[square] = rank;
		}
	}
}

/*
Convert 120 based index into 64 based index.
*/
int Board::SQ_64(int sq120) {
	return sq120ToSq64[(sq120)];
}

/*
Convert 64 based index into 120 based index.
*/
int Board::SQ_120(int sq64) {
	return sq64ToSq120[(sq64)];
}


/*
Generate a random 64bit integer for zobrist hashing
*/
U64 Board::rand64() {
	return (U64)rand() |
		((U64)rand() << 15) | 
		((U64)rand() << 30) | 
		((U64)rand() << 45) | 
		(((U64)rand() & 0xf) << 60);
}

void Board::initHashKeys() {

	// every piece on every square with a random 64bit number
	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < 120; j++) {
			pieceKeys[i][j] = rand64();
		}
	}

	sideKey = rand64();
	for (int i = 0; i < 16; i++) {
		castleKeys[i] = rand64();
	}

}

U64 Board::generateZobristHash(Board *b) {

	int sq = 0;
	U64 finalKey = 0;
	int piece = EMPTY;

	// pieces
	for (sq = 0; sq < NUM_SQUARES; ++sq) {
		piece = b->pieces[sq];
		if (piece != NO_SQ && piece != EMPTY && piece != OFFBOARD) {
			ASSERT(piece >= P && piece <= k);
			finalKey ^= pieceKeys[piece][sq];
		}
	}

	if (b->side == WHITE) {
		finalKey ^= sideKey;
	}

	if (b->enPas != NO_SQ) {
		ASSERT(b->enPas >= 0 && b->enPas < NUM_SQUARES);
		finalKey ^= pieceKeys[EMPTY][b->enPas];
	}

	ASSERT(b->castlePermission >= 0 && b->castlePermission <= 15);

	finalKey ^= castleKeys[b->castlePermission];

	return finalKey;
}

void Board::resetBoard(Board *b) {

	int index = 0;

	for (index = 0; index < NUM_SQUARES; ++index) {
		b->pieces[index] = OFFBOARD;
	}

	for (index = 0; index < 64; ++index) {
		b->pieces[sq64ToSq120[index]] = EMPTY;
	}

	for (index = 0; index < 2; ++index) {
		b->bigPieces[index] = 0;
		b->majPieces[index] = 0;
		b->minPieces[index] = 0;
	}

	for (index = 0; index < 3; ++index) {
		b->pawns[index] = 0ULL;
	}

	for (index = 0; index < 13; ++index) {
		b->pieceNumber[index] = 0;
	}

	b->KingSquares[WHITE] = b->KingSquares[BLACK] = NO_SQ;

	b->side = BOTH;
	b->enPas = NO_SQ;
	b->fiftyMove = 0;

	b->ply = 0;
	b->hisPly = 0;

	b->castlePermission = 0;

	b->posKey = 0ULL;
}

int Board::parseFen(char* fen, Board* pos) {

	ASSERT(fen != NULL);
	ASSERT(pos != NULL);

	int  rank = RANK_8;
	int  file = FILE_A;
	int  piece = 0;
	int  count = 0;
	int  i = 0;
	int  sq64 = 0;
	int  sq120 = 0;

	resetBoard(pos);

	while ((rank >= RANK_1) && *fen) {
		count = 1;
		switch (*fen) {
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
				count = *fen - '0';
				break;

			case '/':
			case ' ':
				rank--;
				file = FILE_A;
				fen++;
				continue;

			default:
				printf("FEN error \n");
				return -1;
		}

		for (i = 0; i < count; i++) {
			sq64 = rank * 8 + file;
			sq120 = SQ_120(sq64);
			if (piece != EMPTY) {
				pos->pieces[sq120] = piece;
			}
			file++;
		}
		fen++;
	}

	ASSERT(*fen == 'w' || *fen == 'b');

	pos->side = (*fen == 'w') ? WHITE : BLACK;
	fen += 2;

	for (i = 0; i < 4; i++) {
		if (*fen == ' ') {
			break;
		}
		switch (*fen) {
			case 'K': pos->castlePermission |= K_CASTLE; break;
			case 'Q': pos->castlePermission |= Q_CASTLE; break;
			case 'k': pos->castlePermission |= k_CASTLE; break;
			case 'q': pos->castlePermission |= q_CASTLE; break;
			default: break;
		}
		fen++;
	}
	fen++;

	ASSERT(pos->castlePermission >= 0 && pos->castlePermission <= 15);

	if (*fen != '-') {
		file = fen[0] - 'a';
		rank = fen[1] - '1';

		ASSERT(file >= FILE_A && file <= FILE_H);
		ASSERT(rank >= RANK_1 && rank <= RANK_8);

		pos->enPas = file_rank_2_sq(file, rank);
	}

	pos->posKey = generateZobristHash(pos);

	updateListsMaterial(pos);

	return 0;
}

void Board::printBoard(const Board* board) {

	int sq, file, rank, piece;

	// print board
	for (rank = RANK_8; rank >= RANK_1; rank--) {
		printf("%d  ", rank + 1);
		for (file = FILE_A; file <= FILE_H; file++) {
			sq = file_rank_2_sq(file, rank);
			piece = board->pieces[sq];
			printf("%3c", pieceChar[piece]);
		}
		printf("\n");
	}

	printf("\n   ");
	for (file = FILE_A; file <= FILE_H; file++) {
		printf("%3c", 'a' + file);
	}

	// print stats
	printf("\n");
	printf("side:%c\n", sideChar[board->side]);
	printf("enPas:%d\n", board->enPas);
	printf("castle:%c%c%c%c\n",
		board->castlePermission & K_CASTLE ? 'K' : '-',
		board->castlePermission & Q_CASTLE ? 'Q' : '-',
		board->castlePermission & k_CASTLE ? 'k' : '-',
		board->castlePermission & q_CASTLE ? 'q' : '-'
	);
	printf("ZobristHash:%llX\n", board->posKey);
}


/*
Initialize all arrays and masks. Must be called before using board.
*/
void Board::init() {
	init120To64();
	initClearSetMask();
	initHashKeys();
	initRankFileArrays();
}

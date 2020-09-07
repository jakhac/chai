#include "Board.h"

char* Board::printMove(const int move) {

	static char moveStr[6];

	int ff = fileBoard[FROMSQ(move)];
	int rf = rankBoard[FROMSQ(move)];
	int ft = fileBoard[TOSQ(move)];
	int rt = rankBoard[TOSQ(move)];

	int promoted = PROMOTED(move);

	if (promoted) {
		char pchar = 'q';
		if (bd.isN(promoted)) {
			pchar = 'n';
		}
		else if (bd.isRQ(promoted) && !bd.isBQ(promoted)) {
			pchar = 'r';
		}
		else if (!bd.isRQ(promoted) && bd.isBQ(promoted)) {
			pchar = 'b';
		}
		sprintf_s(moveStr, "%c%c%c%c%c", ('a' + ff), ('1' + rf), ('a' + ft), ('1' + rt), pchar);
	}
	else {
		sprintf_s(moveStr, "%c%c%c%c", ('a' + ff), ('1' + rf), ('a' + ft), ('1' + rt));
	}

	return moveStr;
}

int Board::sqOnBoard(int sq) {
	return fileBoard[sq] == OFFBOARD ? 0 : 1;
}

void Board::init() {
	bd.init120To64();
	bb.initClearSetMask();
	bd.initRankFileArrays();
	initHashKeys();
}

int Board::checkBoard(Board* board) {

	// set up temporary variables to capture current state
	int t_pieceNumber[13] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	int t_bigPiece[2] = { 0, 0 };
	int t_majPiece[2] = { 0, 0 };
	int t_minPiece[2] = { 0, 0 };
	int t_material[2] = { 0, 0 };

	int sq64, t_piece, t_pce_num, sq120, color;

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
		sq120 = bd.SQ_120(sq64);
		t_piece = board->pieces[sq120];
		t_pieceNumber[t_piece]++;
		color = pieceCol[t_piece];

		if (bd.pieceBig[t_piece]) t_bigPiece[color]++;
		if (bd.pieceMaj[t_piece]) t_majPiece[color]++;
		if (bd.pieceMin[t_piece]) t_minPiece[color]++;

		t_material[color] += bd.pieceVal[t_piece];
	}

	// check if counted pieces of each type are equal to stored pieces on board
	for (t_piece = P; t_piece <= k; t_piece++) {
		ASSERT(t_pieceNumber[t_piece]==board->pieceNumber[t_piece]);
	}

	// check pawn bitboards
	ASSERT(bb.countBits(t_pawns[WHITE]) == board->pieceNumber[P]);
	ASSERT(bb.countBits(t_pawns[BLACK]) == board->pieceNumber[p]);
	ASSERT(bb.countBits(t_pawns[BOTH])==board->pieceNumber[p] + board->pieceNumber[P]);

	// check pawn bits and squares
	while (t_pawns[WHITE]) {
		sq64 = bb.popBit(&t_pawns[WHITE]);
		ASSERT(P==board->pieces[bd.SQ_120(sq64)]);
	}
	while (t_pawns[BLACK]) {
		sq64 = bb.popBit(&t_pawns[BLACK]);
		ASSERT(p==board->pieces[bd.SQ_120(sq64)]);
	}
	while (t_pawns[BOTH]) {
		sq64 = bb.popBit(&t_pawns[BOTH]);
		ASSERT(P==board->pieces[bd.SQ_120(sq64)] || p==board->pieces[bd.SQ_120(sq64)]);
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
			if (bd.pieceBig[piece]) b->bigPieces[color]++;
			if (bd.pieceMin[piece]) b->minPieces[color]++;
			if (bd.pieceMaj[piece]) b->majPieces[color]++;

			// add up material values
			b->material[color] += bd.pieceVal[piece];

			// store found piece in pieceList and increment for next storage
			b->pieceList[piece][b->pieceNumber[piece]] = sq;
			b->pieceNumber[piece]++;

			if (piece == K) b->KingSquares[color] = sq;
			if (piece == k) b->KingSquares[color] = sq;

			//set bits for pawns
			if (piece == P) {
				bb.setBit(&b->pawns[WHITE], bd.SQ_64(sq));
				bb.setBit(&b->pawns[BOTH], bd.SQ_64(sq));
			}
			if (piece == p) {
				bb.setBit(&b->pawns[BLACK], bd.SQ_64(sq));
				bb.setBit(&b->pawns[BOTH], bd.SQ_64(sq));
			}
		}
	}
}


/*
Initiliaze hash keys for zobrist hashing
*/
void Board::initHashKeys() {

	// every piece on every square with a random 64bit number
	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < 120; j++) {
			pieceKeys[i][j] = bd.rand64();
		}
	}

	sideKey = bd.rand64();
	for (int i = 0; i < 16; i++) {
		castleKeys[i] = bd.rand64();
	}

}

/*
Generate a zobrist hash for referenced board
*/
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

/*
Reset all board variables
*/
void Board::resetBoard(Board *b) {

	int index = 0;

	for (index = 0; index < NUM_SQUARES; ++index) {
		b->pieces[index] = OFFBOARD;
	}

	for (index = 0; index < 64; ++index) {
		b->pieces[bd.SQ_120(index)] = EMPTY;
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

/*
Parse a referenced fen into the boards pieces arrays.
*/
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
			sq120 = bd.SQ_120(sq64);
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

		pos->enPas = bd.file_rank_2_sq(file, rank);
	}

	pos->posKey = generateZobristHash(pos);

	updateListsMaterial(pos);

	return 0;
}

/*
Print a referenced board.
*/
void Board::printBoard(const Board* board) {

	int sq, file, rank, piece;

	// print board
	for (rank = RANK_8; rank >= RANK_1; rank--) {
		printf("%d  ", rank + 1);
		for (file = FILE_A; file <= FILE_H; file++) {
			sq = bd.file_rank_2_sq(file, rank);
			piece = board->pieces[sq];
			printf("%3c", bd.pieceChar[piece]);
		}
		printf("\n");
	}

	printf("\n   ");
	for (file = FILE_A; file <= FILE_H; file++) {
		printf("%3c", 'a' + file);
	}

	// print stats
	printf("\n");
	printf("side:%c\n", bd.sideChar[board->side]);
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
Returns true if the given squared is attacked by side.
*/
int Board::squareAttacked(const int sq, const int side, Board* board) {

	int pce, index, t_sq, dir;

	ASSERT(board->sqOnBoard(sq));
	ASSERT(sideValid(side));
	ASSERT(checkBoard(board));

	// pawns
	if (side == WHITE) {
		if (board->pieces[sq - 11] == P || board->pieces[sq - 9] == P) {
			return 1;
		}
	}
	else {
		if (board->pieces[sq + 11] == p || board->pieces[sq + 9] == p) {
			return 1;
		}
	}

	// knights
	for (index = 0; index < 8; ++index) {
		pce = board->pieces[sq + bd.kDir[index]];
		if (pce != OFFBOARD && bd.isK(pce) && pieceCol[pce] == side) {
			return 1;
		}
	}

	// rooks, queens
	for (index = 0; index < 4; ++index) {
		dir = bd.rDir[index];
		t_sq = sq + dir;
		pce = board->pieces[t_sq];
		while (pce != OFFBOARD) {
			if (pce != EMPTY) {
				if (bd.isRQ(pce) && pieceCol[pce] == side) {
					return 1;
				}
				break;
			}
			t_sq += dir;
			pce = board->pieces[t_sq];
		}
	}

	// bishops, queens
	for (index = 0; index < 4; ++index) {
		dir = bd.bDir[index];
		t_sq = sq + dir;
		pce = board->pieces[t_sq];
		while (pce != OFFBOARD) {
			if (pce != EMPTY) {
				if (bd.isBQ(pce) && pieceCol[pce] == side) {
					return 1;
				}
				break;
			}
			t_sq += dir;
			pce = board->pieces[t_sq];
		}
	}

	// kings
	for (index = 0; index < 8; ++index) {
		pce = board->pieces[sq + bd.kDir[index]];
		if (pce != OFFBOARD && bd.isK(pce) && pieceCol[pce] == side) {
			return 1;
		}
	}

	return 0;
}

/*
Print attackers for given side.
*/
void Board::printAttackers(const int side, Board* b) {

	int rank = 0;
	int file = 0;
	int square = 0;

	std::cout << "Attacked squares by " << side << endl;

	for (rank = RANK_8; rank >= RANK_1; rank--) {
		for (file = FILE_A; file <= FILE_H; file++) {
			square = b->bd.file_rank_2_sq(file, rank);
			if (squareAttacked(square, side, b)) {
				std::cout << "1 ";
			}
			else {
				std::cout << ". ";
			}
		}
		std::cout << "\n";
	}
	std::cout << "\n";
}

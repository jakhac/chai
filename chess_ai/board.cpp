#include "Board.h"

/*
Init 120 to 63 indexed arrays
*/
void Board::init120To64() {
	int sq, squareCounter = 0;

	// Set both arrays non-legal values to prevent errors
	for (int i = 0; i < NUM_SQUARES; i++) {
		sq120ToSq64[i] = 65;
	}

	for (int rank = RANK_1; rank <= RANK_8; rank++) {
		sq64ToSq120[rank] = 120;
	}

	// Fill arrays with indices
	for (int rank = RANK_1; rank <= RANK_8; ++rank) {
		for (int file = FILE_A; file <= FILE_H; ++file) {
			sq = file_rank_2_sq(file, rank);

			// TODO use methods
			sq120ToSq64[sq] = squareCounter;
			sq64ToSq120[squareCounter] = sq;

			squareCounter++;
		}
	}
}
/*
Initialize rank and file arrays.
*/
void Board::initRankFileArrays() {
	int square;

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
Conert 120 based into 64 based index.
*/
int Board::SQ_64(int sq120) {
	return sq120ToSq64[sq120];
}

/*
Conert 64 based into 120 based index.
*/
int Board::SQ_120(int sq64) {
	return sq64ToSq120[sq64];
}


char* Board::printMove(const int move) {

	static char moveStr[6];

	int ff = fileBoard[FROMSQ(move)];
	int rf = rankBoard[FROMSQ(move)];
	int ft = fileBoard[TOSQ(move)];
	int rt = rankBoard[TOSQ(move)];

	int promoted = PROMOTED(move);

	if (promoted) {
		char pchar = 'q';
		if (isN(promoted)) {
			pchar = 'n';
		}
		else if (isRQ(promoted) && !isBQ(promoted)) {
			pchar = 'r';
		}
		else if (!isRQ(promoted) && isBQ(promoted)) {
			pchar = 'b';
		}
		sprintf_s(moveStr, "%c%c%c%c%c", ('a' + ff), ('1' + rf), ('a' + ft), ('1' + rt), pchar);
	}
	else {
		sprintf_s(moveStr, "%c%c%c%c", ('a' + ff), ('1' + rf), ('a' + ft), ('1' + rt));
	}

	return moveStr;
}

char* Board::getSquareStr(const int sq) {

	static char SqStr[3];

	int file = fileBoard[sq];
	int rank = rankBoard[sq];
	sprintf_s(SqStr, "%c%c", ('a' + file), ('1' + rank));

	return SqStr;

}

int Board::sqOnBoard(int sq) {
	return fileBoard[sq] == OFFBOARD ? 0 : 1;
}

void Board::init() {
	init120To64();
	bb.initClearSetMask();
	initRankFileArrays();
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
			ASSERT(board->pieces[sq120] == t_piece);
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
		ASSERT(t_pieceNumber[t_piece] == board->pieceNumber[t_piece]);
	}

	// check pawn bitboards
	ASSERT(bb.countBits(&t_pawns[WHITE]) == board->pieceNumber[P]);
	ASSERT(bb.countBits(&t_pawns[BLACK]) == board->pieceNumber[p]);
	ASSERT(bb.countBits(&t_pawns[BOTH]) == board->pieceNumber[p] + board->pieceNumber[P]);

	// check pawn bits and squares
	while (t_pawns[WHITE]) {
		sq64 = bb.popBit(&t_pawns[WHITE]);
		ASSERT(P == board->pieces[SQ_120(sq64)]);
	}
	while (t_pawns[BLACK]) {
		sq64 = bb.popBit(&t_pawns[BLACK]);
		ASSERT(p == board->pieces[SQ_120(sq64)]);
	}
	while (t_pawns[BOTH]) {
		sq64 = bb.popBit(&t_pawns[BOTH]);
		ASSERT(P == board->pieces[SQ_120(sq64)] || p == board->pieces[SQ_120(sq64)]);
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
				bb.setBit(&b->pawns[WHITE], SQ_64(sq));
				bb.setBit(&b->pawns[BOTH], SQ_64(sq));
			}
			if (piece == p) {
				bb.setBit(&b->pawns[BLACK], SQ_64(sq));
				bb.setBit(&b->pawns[BOTH], SQ_64(sq));
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
			pieceKeys[i][j] = rand64();
		}
	}

	sideKey = rand64();
	for (int i = 0; i < 16; i++) {
		castleKeys[i] = rand64();
	}

}

/*
Generate a zobrist hash for referenced board
*/
U64 Board::generateZobristHash(Board* b) {

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
void Board::resetBoard(Board* b) {

	int index = 0;

	for (index = 0; index < NUM_SQUARES; ++index) {
		b->pieces[index] = OFFBOARD;
	}

	for (index = 0; index < 64; ++index) {
		b->pieces[SQ_120(index)] = EMPTY;
	}

	for (index = 0; index < 2; ++index) {
		b->bigPieces[index] = 0;
		b->majPieces[index] = 0;
		b->minPieces[index] = 0;
		b->material[index] = 0;
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

/*
Print a referenced board.
*/
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
		pce = board->pieces[sq + kDir[index]];
		if (pce != OFFBOARD && isK(pce) && pieceCol[pce] == side) {
			return 1;
		}
	}

	// rooks, queens
	for (index = 0; index < 4; ++index) {
		dir = rDir[index];
		t_sq = sq + dir;
		pce = board->pieces[t_sq];
		while (pce != OFFBOARD) {
			if (pce != EMPTY) {
				if (isRQ(pce) && pieceCol[pce] == side) {
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
		dir = bDir[index];
		t_sq = sq + dir;
		pce = board->pieces[t_sq];
		while (pce != OFFBOARD) {
			if (pce != EMPTY) {
				if (isBQ(pce) && pieceCol[pce] == side) {
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
		pce = board->pieces[sq + kDir[index]];
		if (pce != OFFBOARD && isK(pce) && pieceCol[pce] == side) {
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
			square = file_rank_2_sq(file, rank);
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

/*
Clear piece of board and update arrays and zobrist key
*/
void Board::clearPiece(Board* b, const int sq) {

	ASSERT(sqOnBoard(sq));
	int piece = b->pieces[sq];
	ASSERT(pieceValid(piece));

	int side = pieceCol[piece];
	int t_pieceNum = -1;

	// hash piece out of zobrist key
	b->posKey ^= pieceKeys[piece][sq];

	b->pieces[sq] = EMPTY;
	b->material[side] -= pieceVal[piece];

	// update counters for big pieces
	if (pieceBig[piece]) {
		b->bigPieces[side]--;
		if (pieceMaj[piece]) b->majPieces[side]--;
		if (pieceMin[piece]) b->minPieces[side]--;
	}
	else {
		// clear bits from pawn bitboards
		bb.clearBit(&b->pawns[side], SQ_64(sq));
		bb.clearBit(&b->pawns[BOTH], SQ_64(sq));
	}

	for (int i = 0; i < b->pieceNumber[piece]; i++) {
		if (b->pieceList[piece][i] == sq) {
			t_pieceNum = i;
			break;
		}
	}

	ASSERT(t_pieceNum != -1);


	// decrement piece counter of deleted piece
	b->pieceNumber[piece]--; 

	// overwrite deleted index with last elem from pieceList
	b->pieceList[piece][t_pieceNum] = b->pieceList[piece][b->pieceNumber[piece]]; 
}

/*
Add piece on board and update pieceLists and hash.
*/
void Board::addPiece(Board* b, const int sq, const int piece) {

	ASSERT(pieceValid(piece));
	ASSERT(sqOnBoard(sq));

	int side = pieceCol[piece];

	// add piece to pieces
	b->pieces[sq] = piece;

	// update counters for big pieces
	if (pieceBig[piece]) {
		b->bigPieces[side]++;
		if (pieceMaj[piece]) b->majPieces[side]++;
		if (pieceMin[piece]) b->minPieces[side]++;
	}
	else {
		// clear bits from pawn bitboards
		bb.setBit(&b->pawns[side], SQ_64(sq));
		bb.setBit(&b->pawns[BOTH], SQ_64(sq));
	}

	b->material[side] += pieceVal[piece]; // add material values
	b->pieceList[piece][b->pieceNumber[piece]++] = sq; // add and increment after

}

/*
Move piece on board and update pieceLists and hash.
*/
void Board::movePiece(Board* b, const int from, const int to) {

	ASSERT(sqOnBoard(from));
	ASSERT(sqOnBoard(to));

	const int piece = b->pieces[from];
	const int side = pieceCol[piece];

	// hash from sq out of key
	b->posKey ^= b->pieceKeys[piece][from];
	b->pieces[from] = EMPTY;

	// hash piece into key
	b->posKey ^= b->pieceKeys[piece][to];
	b->pieces[to] = piece;

#ifdef DEBUG
	int t_pieceNum = 0;
#endif

	if (!pieceBig[piece]) {
		//  clear bits from pawn bitboards
		bb.clearBit(&b->pawns[side], SQ_64(from));
		bb.clearBit(&b->pawns[BOTH], SQ_64(from));

		// add bits to pawn bitboards
		bb.setBit(&b->pawns[side], SQ_64(to));
		bb.setBit(&b->pawns[BOTH], SQ_64(to));
	}

	for (int i = 0; i < b->pieceNumber[piece]; i++) {
		if (b->pieceList[piece][i] == from) {
			b->pieceList[piece][i] = to;

#ifdef DEBUG
			t_pieceNum = 1;
#endif
			break;
		}
	}

	// assert that moved piece is found in piece list
	ASSERT(t_pieceNum);
}

/*
Print a bitboard.
*/
void Board::printBitBoard(U64 bb) {

	U64 shiftBit = 1ULL;

	int rank = 0;
	int file = 0;
	int sq = 0;
	int sq64 = 0;

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

/*
Push a move on board. Return 0 if move would leave the moved side in check, else 1 for valid move.
*/
int Board::push(Board* b, const int move)
{
	ASSERT(checkBoard(b));

	int from = FROMSQ(move);
	int to = TOSQ(move);
	int side = b->side;

	ASSERT(sqOnBoard(from));
	ASSERT(sqOnBoard(to));
	ASSERT(sideValid(side));
	ASSERT(pieceValid(b->pieces[from]));

	b->history[b->hisPly].posKey = b->posKey;

	// if move is en passant capture
	if (move & MFLAGEP) {
		if (side == WHITE) clearPiece(b, to - 10);
		if (side == BLACK) clearPiece(b, to + 10);
	}

	// castle move
	else if (move & MFLAGCA) {
		switch (to)
		{
			case C1: movePiece(b, A1, D1); break;
			case C8: movePiece(b, A8, D8); break;
			case G1: movePiece(b, H1, F1); break;
			case G8: movePiece(b, H8, F8); break;
			default: ASSERT(0) break;
		}
	}

	// if en passant square is set, hash out
	if (b->enPas != NO_SQ) {
		b->posKey ^= b->pieceKeys[EMPTY][b->enPas];
	}

	// hash castle out
	b->posKey ^= castleKeys[b->castlePermission];

	// store data in history array
	b->history[b->hisPly].move = move;
	b->history[b->hisPly].fiftyMove = b->fiftyMove;
	b->history[b->hisPly].enPas = b->enPas;
	b->history[b->hisPly].castlePermission = b->castlePermission;

	b->castlePermission &= castlePerm[from];
	b->castlePermission &= castlePerm[to];
	b->enPas = NO_SQ;

	// hash castle in
	b->posKey ^= castleKeys[b->castlePermission];

	int cap = CAPTURED(move);

	b->fiftyMove++;
	if (cap != EMPTY) {
		ASSERT(pieceValid(cap));
		clearPiece(b, to);
		b->fiftyMove = 0;
	}

	b->hisPly++;
	b->ply++;

	if (piecePawn[b->pieces[from]]) {
		b->fiftyMove = 0;
		if (move & MFLAGPS) {
			cout << "Push move: PS Flag " << printMove(move) << endl;
			printBoard(b);
			if (b->side == WHITE) {
				b->enPas = from + 10;
				ASSERT(rankBoard[b->enPas] == RANK_3);
			}
			else {
				b->enPas = from - 10;
				ASSERT(rankBoard[b->enPas] == RANK_6);
			}
			// hash en passant in key
			b->posKey ^= pieceKeys[EMPTY][b->enPas];
		}
	}

	movePiece(b, from, to);

	// handle promotions
	int promPiece = PROMOTED(move);
	if (promPiece != EMPTY) {
		ASSERT(pieceValid(promPiece) && !piecePawn[promPiece]);
		clearPiece(b, to);
		addPiece(b, to, promPiece);
	}

	// update king square
	if (pieceKing[b->pieces[to]]) {
		b->KingSquares[b->side] = to;
	}

	b->side ^= 1;
	b->posKey ^= sideKey;

	ASSERT(checkBoard(b));

	// if side to move now is giving check, move is not valid
	if (squareAttacked(b->KingSquares[side], b->side, b)) {
		pop(b);
		return 0;
	}
	
	return 1;
}

/*
Undo a move and pop from the history. TODO maybe use zobrist hash from stored undmove object
*/
void Board::pop(Board* b) {

	ASSERT(checkBoard(b));

	b->hisPly--;
	b->ply--;

	int move = b->history[b->hisPly].move;
	int from = FROMSQ(move);
	int to = TOSQ(move);

	ASSERT(sqOnBoard(from));
	ASSERT(sqOnBoard(to));

	// if en pas is set, hash out 
	if (b->enPas != NO_SQ)  {
		b->posKey ^= pieceKeys[EMPTY][b->enPas];
	}
	// hash out castle
	b->posKey ^= castleKeys[b->castlePermission];

	// set new castle and en pas permissions in board
	b->castlePermission = b->history[b->hisPly].castlePermission;
	b->fiftyMove = b->history[b->hisPly].fiftyMove;
	b->enPas= b->history[b->hisPly].enPas;

	// if en pas is set, hash in 
	if (b->enPas != NO_SQ) {
		b->posKey ^= pieceKeys[EMPTY][b->enPas];
	}
	// hash in new castle perms
	b->posKey ^= castleKeys[b->castlePermission];

	b->side ^= 1;
	b->posKey ^= sideKey;

	// if en pas
	if (MFLAGEP & move) {
		if (b->side == WHITE) {
			addPiece(b, to - 10, p);
		}
		else {
			addPiece(b, to + 10, P);
		}
	}
	else if (MFLAGCA & move) {
		switch (to) {
			// rook moves
			case C1: movePiece(b, D1, A1); break;
			case C8: movePiece(b, D8, A8); break;
			case G1: movePiece(b, F1, H1); break;
			case G8: movePiece(b, F8, H8); break;
			default: ASSERT(0); break;
		}
	}

	// move capturer back to from square
	movePiece(b, to, from);

	// update king squares
	if (pieceKing[b->pieces[from]]) {
		b->KingSquares[b->side] = from;
	}

	// add captured piece if possible
	int cap = CAPTURED(move);
	if (cap != EMPTY) {
		ASSERT(pieceValid(cap));
		addPiece(b, to, cap);
	}

	// add promoted pawn if promotion
	int prom = PROMOTED(move);
	if (prom != EMPTY) {
		ASSERT(pieceValid(prom) && !piecePawn[prom]);
		clearPiece(b, from);
		int pawn = (pieceCol[prom]) == WHITE ? P : p;
		addPiece(b, from, pawn);
	}

	ASSERT(checkBoard(b));
}

#include "bitboard.h"


bitboard_t getPinner(board_t* b, int kSq, color_t kSide) {

	bitboard_t kingSlider = lookUpRookMoves(kSq, b->occupied);
	bitboard_t potPinned  = kingSlider & b->color[kSide];
	bitboard_t xrays      = kingSlider ^ lookUpRookMoves(kSq, b->occupied ^ potPinned);
	bitboard_t pinner     = xrays & (getPieces(b, cQUEEN, !kSide) | (getPieces(b, cROOK, !kSide)));

	kingSlider = lookUpBishopMoves(kSq, b->occupied);
	potPinned  = kingSlider & b->color[kSide];
	xrays      = kingSlider ^ lookUpBishopMoves(kSq, b->occupied ^ potPinned);
	pinner    |= xrays & (getPieces(b, cQUEEN, !kSide) | (getPieces(b, cBISHOP, !kSide)));

	return pinner;
}

bitboard_t getPinned(board_t* b, int kSq, color_t kSide) {

	bitboard_t pinned = 0;

	bitboard_t kingSlider = lookUpRookMoves(kSq, b->occupied);
	bitboard_t potPinned  = kingSlider & b->color[kSide];
	bitboard_t xrays      = kingSlider ^ lookUpRookMoves(kSq, b->occupied ^ potPinned);
	bitboard_t pinner     = xrays & (getPieces(b, cQUEEN, !kSide) | (getPieces(b, cROOK, !kSide)));

	while (pinner) {
		int sq  = popLSB(&pinner);
		pinned |= obstructed(sq, kSq) & b->color[kSide];
	}

	kingSlider = lookUpBishopMoves(kSq, b->occupied);
	potPinned  = kingSlider & b->color[kSide];
	xrays      = kingSlider ^ lookUpBishopMoves(kSq, b->occupied ^ potPinned);
	pinner     = xrays & (getPieces(b, cQUEEN, !kSide) | (getPieces(b, cBISHOP, !kSide)));

	while (pinner) {
		int sq  = popLSB(&pinner);
		pinned |= obstructed(sq, kSq) & b->color[kSide];
	}

	return pinned;
}

bitboard_t getDiscoveredAttacks(board_t* b, int sq, color_t color) {

    bitboard_t rAttacks = lookUpRookMoves(sq, b->occupied);
    bitboard_t bAttacks = lookUpBishopMoves(sq, b->occupied);
    bitboard_t rooks    = getPieces(b, cROOK, !color) & ~rAttacks;
    bitboard_t bishops  = getPieces(b, cBISHOP, !color) & ~bAttacks;

	return (rooks & lookUpRookMoves(sq, b->occupied & ~rAttacks))
         | (bishops & lookUpBishopMoves(sq, b->occupied & ~bAttacks));
}

template<pType_t pType>
bitboard_t attackerSetFor(board_t* b, color_t color) {

    int sq;
	bitboard_t atks  = 0;
    bitboard_t piece = getPieces(b, pType, color);

	while (piece) {
		sq    = popLSB(&piece);
		atks |= getMoveMask<pType>(sq, b->occupied, color);
	}

    return atks;
}

bitboard_t attackerSet(board_t* b, color_t color) {

	bitboard_t attackerSet = 0;

    attackerSet |= attackerSetFor<cKING>(b, color);
    attackerSet |= attackerSetFor<cPAWN>(b, color);
    attackerSet |= attackerSetFor<cKNIGHT>(b, color);
    attackerSet |= attackerSetFor<cBISHOP>(b, color);
    attackerSet |= attackerSetFor<cROOK>(b, color);
    attackerSet |= attackerSetFor<cQUEEN>(b, color);

	return attackerSet;
}

template<pType_t pType>
bitboard_t matchingBlocksFrom(board_t* b, bitboard_t block, color_t color) {

    int sq;
    bitboard_t blockerSet = 0;
    bitboard_t piece      = getPieces(b, pType, color);

    while (piece) {
        sq = popLSB(&piece);
        if (getMoveMask<pType>(sq, b->occupied) & block) {
            blockerSet |= (1ULL << sq);
        }
    }

    return blockerSet;
}

bitboard_t blockerSet(board_t* b, color_t color, int blockSq) {
	bitboard_t piece, blockerSet = 0ULL;
	bitboard_t blockSqBoard = (1ULL << blockSq);

	// Find pawn pushes, that block the square
	piece = getPieces(b, cPAWN, color);
	bitboard_t pushedPawns;
	if (color == WHITE) {

		// Single-Push
		pushedPawns = ~b->occupied 
                    & blockSqBoard
                    & (piece << 8);
		blockerSet |= (pushedPawns >> 8);

		// Double-Push
		pushedPawns = ~b->occupied
                    & blockSqBoard
                    & ((((piece & RANK_2_HEX) << 8) & ~b->occupied) << 8);
		blockerSet |= (pushedPawns >> 16);

		// Enpas
		if (b->enPas == blockSq) {

			if ((piece << 7 & ~FILE_A_HEX) & (1ULL << b->enPas))
                blockerSet |= blockSqBoard >> 7;

			if ((piece << 9 & ~FILE_H_HEX) & (1ULL << b->enPas))
				blockerSet |= blockSqBoard >> 9;

		}

	} else {

		pushedPawns = ~b->occupied 
                    & blockSqBoard
                    & (piece >> 8);
		blockerSet |= (pushedPawns << 8);

		pushedPawns = ~b->occupied 
                    & blockSqBoard
                    & ((((piece & RANK_7_HEX) >> 8) & ~b->occupied) >> 8);
		blockerSet |= (pushedPawns << 16);

		if (b->enPas == blockSq) {

			if ((piece >> 7 & ~FILE_A_HEX) & (1ULL << b->enPas))
				blockerSet |= blockSqBoard << 7;

			if ((piece >> 9 & ~FILE_H_HEX) & (1ULL << b->enPas))
				blockerSet |= blockSqBoard << 9;

		}
	}

    // Check for non-pawn moves that can reach a blocking square 
    blockerSet |= matchingBlocksFrom<cKNIGHT>(b, blockSqBoard, color);
    blockerSet |= matchingBlocksFrom<cBISHOP>(b, blockSqBoard, color);
    blockerSet |= matchingBlocksFrom<cBISHOP>(b, blockSqBoard, color);
    blockerSet |=   matchingBlocksFrom<cROOK>(b, blockSqBoard, color);
    blockerSet |=  matchingBlocksFrom<cQUEEN>(b, blockSqBoard, color);

	return blockerSet;
}

bitboard_t squareAttackedBy(board_t* b, int square, color_t color) {

	bitboard_t attacker = 0;

	attacker |= pawnAtkMask[!color][square] & getPieces(b, cPAWN, color);
	attacker |= knightAtkMask[square]       & getPieces(b, cKNIGHT, color);
	attacker |= lookUpBishopMoves(square, b->occupied) & (getDiagPieces(b, color));
	attacker |= lookUpRookMoves(square, b->occupied)   & (getVertPieces(b, color));

	return attacker;
}

bitboard_t squareAtkDef(board_t* b, int square) {

    Assert((pawnAtkMask[!b->stm][square] | pawnAtkMask[b->stm][square]) == xMask[square]);
	bitboard_t attacker = 0;

	attacker |= b->pieces[cPAWN]   & xMask[square];
	attacker |= b->pieces[cKNIGHT] & knightAtkMask[square];
	attacker |= lookUpBishopMoves(square, b->occupied) & (b->pieces[cQUEEN] | b->pieces[cBISHOP]);
	attacker |= lookUpRookMoves(square, b->occupied)   & (b->pieces[cQUEEN] | b->pieces[cROOK]);

	return attacker;
}

bitboard_t squareAtkDefOcc(board_t* b, bitboard_t occupied, int square) {

    Assert((pawnAtkMask[!b->stm][square] | pawnAtkMask[b->stm][square]) == xMask[square]);
	bitboard_t attacker = 0;

	attacker |= b->pieces[cPAWN]   & xMask[square];
	attacker |= b->pieces[cKNIGHT] & knightAtkMask[square];
	attacker |= lookUpBishopMoves(square, occupied) & (b->pieces[cQUEEN] | b->pieces[cBISHOP]);
	attacker |= lookUpRookMoves(square, occupied)   & (b->pieces[cQUEEN] | b->pieces[cROOK]);

	return attacker;
}

bitboard_t getBlockedPawns(board_t* b, color_t color) {

	int sq;
	int shift = (color == WHITE) ? 8 : -8;

	bitboard_t pawns = getPieces(b, cPAWN, color);
	bitboard_t potBlockers = getPieces(b, cPAWN, !color);
	bitboard_t blocked = 0ULL;
	
	while (pawns) {
		sq = popLSB(&pawns);
		if ((1ULL << (sq + shift)) & potBlockers) {
			blocked |= (1ULL << sq);
		}
	}

	return blocked;
}

bitboard_t getPassers(board_t* b, color_t color) {
    
	bitboard_t passers = 0ULL;
	bitboard_t pawns = getPieces(b, cPAWN, color);
	bitboard_t pawnsDef = getPieces(b, cPAWN, !color);

	int sq;
	while (pawns) {
		sq = popLSB(&pawns);

		if (!(pawnPassedMask[color][sq] & pawnsDef)) {
			passers |= (1ULL << sq);
		}
	}

	return passers;
}


void printBoard(board_t* board) {

#ifdef INFO
	int sq, file, rank, piece;

	// print board
	for (rank = RANK_8; rank >= RANK_1; rank--) {
		printf("%d  ", rank + 1);
		for (file = FILE_A; file <= FILE_H; file++) {
			sq = fileRankToSq(file, rank);
			piece = pieceAt(board, sq);
			printf("%2c", pieceChar[piece]);
		}
		printf("\n");
	}

	printf("\n   ");
	for (file = FILE_A; file <= FILE_H; file++) {
		printf("%2c", 'a' + file);
	}

	cout << "\n\nSide to move\t\t= " 
		 << ((board->stm == WHITE) ? "WHITE" : "BLACK") << endl;
	cout << "Position-Hashkey\t= 0x" << std::hex << board->zobristKey << std::dec << endl;
	cout << "Pawn-Hashkey\t\t= 0x" << std::hex << board->zobristPawnKey << std::dec << endl;
	cout << "EP Square\t\t= " << board->enPas << endl;
	cout << "FEN\t\t\t= " << getFEN(board) << endl;
	cout << endl;
#endif // INFO

}
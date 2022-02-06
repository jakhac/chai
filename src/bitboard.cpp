#include "bitboard.h"


Bitboard getPinner(Board* b, int kSq, Color kSide) {

    Bitboard kingSlider = lookUpRookMoves(kSq, b->occupied);
    Bitboard potPinned  = kingSlider & b->color[kSide];
    Bitboard xrays      = kingSlider ^ lookUpRookMoves(kSq, b->occupied ^ potPinned);
    Bitboard pinner     = xrays & (getPieces(b, QUEEN, !kSide) | (getPieces(b, ROOK, !kSide)));

    kingSlider = lookUpBishopMoves(kSq, b->occupied);
    potPinned  = kingSlider & b->color[kSide];
    xrays      = kingSlider ^ lookUpBishopMoves(kSq, b->occupied ^ potPinned);
    pinner    |= xrays & (getPieces(b, QUEEN, !kSide) | (getPieces(b, BISHOP, !kSide)));

    return pinner;
}

Bitboard getPinned(Board* b, int kSq, Color kSide) {

    Bitboard pinned = 0;

    Bitboard kingSlider = lookUpRookMoves(kSq, b->occupied);
    Bitboard potPinned  = kingSlider & b->color[kSide];
    Bitboard xrays      = kingSlider ^ lookUpRookMoves(kSq, b->occupied ^ potPinned);
    Bitboard pinner     = xrays & (getPieces(b, QUEEN, !kSide) | (getPieces(b, ROOK, !kSide)));

    while (pinner) {
        int sq  = popLSB(&pinner);
        pinned |= obstructed(sq, kSq) & b->color[kSide];
    }

    kingSlider = lookUpBishopMoves(kSq, b->occupied);
    potPinned  = kingSlider & b->color[kSide];
    xrays      = kingSlider ^ lookUpBishopMoves(kSq, b->occupied ^ potPinned);
    pinner     = xrays & (getPieces(b, QUEEN, !kSide) | (getPieces(b, BISHOP, !kSide)));

    while (pinner) {
        int sq  = popLSB(&pinner);
        pinned |= obstructed(sq, kSq) & b->color[kSide];
    }

    return pinned;
}

Bitboard getDiscoveredAttacks(Board* b, int sq, Color color) {

    Bitboard rAttacks = lookUpRookMoves(sq, b->occupied);
    Bitboard bAttacks = lookUpBishopMoves(sq, b->occupied);
    Bitboard rooks    = getPieces(b, ROOK, !color) & ~rAttacks;
    Bitboard bishops  = getPieces(b, BISHOP, !color) & ~bAttacks;

    return (rooks & lookUpRookMoves(sq, b->occupied & ~rAttacks))
         | (bishops & lookUpBishopMoves(sq, b->occupied & ~bAttacks));
}

template<PieceType pType>
Bitboard attackerSetFor(Board* b, Color color) {

    int sq;
    Bitboard atks  = 0;
    Bitboard piece = getPieces(b, pType, color);

    while (piece) {
        sq    = popLSB(&piece);
        atks |= getMoveMask<pType>(sq, b->occupied, color);
    }

    return atks;
}

Bitboard attackerSet(Board* b, Color color) {

    Bitboard attackerSet = 0;

    attackerSet |= attackerSetFor<KING>(b, color);
    attackerSet |= attackerSetFor<PAWN>(b, color);
    attackerSet |= attackerSetFor<KNIGHT>(b, color);
    attackerSet |= attackerSetFor<BISHOP>(b, color);
    attackerSet |= attackerSetFor<ROOK>(b, color);
    attackerSet |= attackerSetFor<QUEEN>(b, color);

    return attackerSet;
}

template<PieceType pType>
Bitboard matchingBlocksFrom(Board* b, Bitboard block, Color color) {

    int sq;
    Bitboard blockerSet = 0;
    Bitboard piece      = getPieces(b, pType, color);

    while (piece) {
        sq = popLSB(&piece);
        if (getMoveMask<pType>(sq, b->occupied) & block) {
            blockerSet |= (1ULL << sq);
        }
    }

    return blockerSet;
}

Bitboard blockerSet(Board* b, Color color, int blockSq) {
    Bitboard piece, blockerSet = 0ULL;
    Bitboard blockSqBoard = (1ULL << blockSq);

    // Find pawn pushes, that block the square
    piece = getPieces(b, PAWN, color);
    Bitboard pushedPawns;
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
    blockerSet |= matchingBlocksFrom<KNIGHT>(b, blockSqBoard, color);
    blockerSet |= matchingBlocksFrom<BISHOP>(b, blockSqBoard, color);
    blockerSet |= matchingBlocksFrom<BISHOP>(b, blockSqBoard, color);
    blockerSet |=   matchingBlocksFrom<ROOK>(b, blockSqBoard, color);
    blockerSet |=  matchingBlocksFrom<QUEEN>(b, blockSqBoard, color);

    return blockerSet;
}

Bitboard squareAttackedBy(Board* b, int square, Color color) {

    Bitboard attacker = 0;

    attacker |= pawnAtkMask[!color][square] & getPieces(b, PAWN, color);
    attacker |= knightAtkMask[square]       & getPieces(b, KNIGHT, color);
    attacker |= lookUpBishopMoves(square, b->occupied) & (getDiagPieces(b, color));
    attacker |= lookUpRookMoves(square, b->occupied)   & (getVertPieces(b, color));

    return attacker;
}

Bitboard squareAtkDef(Board* b, int square) {

    Bitboard attacker = 0;

    attacker |= b->pieces[PAWN]   & xMask[square];
    attacker |= b->pieces[KNIGHT] & knightAtkMask[square];
    attacker |= lookUpBishopMoves(square, b->occupied) & (b->pieces[QUEEN] | b->pieces[BISHOP]);
    attacker |= lookUpRookMoves(square, b->occupied)   & (b->pieces[QUEEN] | b->pieces[ROOK]);

    return attacker;
}

Bitboard squareAtkDefOcc(Board* b, Bitboard occupied, int square) {

    Bitboard attacker = 0;

    attacker |= b->pieces[PAWN]   & xMask[square];
    attacker |= b->pieces[KNIGHT] & knightAtkMask[square];
    attacker |= lookUpBishopMoves(square, occupied) & (b->pieces[QUEEN] | b->pieces[BISHOP]);
    attacker |= lookUpRookMoves(square, occupied)   & (b->pieces[QUEEN] | b->pieces[ROOK]);

    return attacker;
}

Bitboard getBlockedPawns(Board* b, Color color) {

    int sq;
    int shift = (color == WHITE) ? 8 : -8;

    Bitboard pawns = getPieces(b, PAWN, color);
    Bitboard potBlockers = getPieces(b, PAWN, !color);
    Bitboard blocked = 0ULL;
    
    while (pawns) {
        sq = popLSB(&pawns);
        if ((1ULL << (sq + shift)) & potBlockers) {
            blocked |= (1ULL << sq);
        }
    }

    return blocked;
}

Bitboard getPassers(Board* b, Color color) {
    
    Bitboard passers = 0ULL;
    Bitboard pawns = getPieces(b, PAWN, color);
    Bitboard pawnsDef = getPieces(b, PAWN, !color);

    int sq;
    while (pawns) {
        sq = popLSB(&pawns);

        if (!(pawnPassedMask[color][sq] & pawnsDef)) {
            passers |= (1ULL << sq);
        }
    }

    return passers;
}


void printBoard(Board* board) {

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
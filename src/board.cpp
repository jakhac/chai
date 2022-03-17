#include "board.h"


int capPiece(Board* b, Move move) {
    return pieceAt(b, toSq(move));
}

bool isCapture(Board* b, Move move) {
    return pieceAt(b, toSq(move));
}

bool isCaptureOrPromotion(Board* b, Move move) {
    return isCapture(b, move) || isPromotion(move);
}

Bitboard getDiagPieces(Board* b, Color color) {
    return getPieces(b, BISHOP, color) | getPieces(b, QUEEN, color);
}

Bitboard getVertPieces(Board* b, Color color) {
    return getPieces(b, ROOK, color) | getPieces(b, QUEEN, color);
}

unsigned int relativeSq(int sq, Color color) {
    return (color == WHITE) ? sq : mirror64[sq];
}


void setPiece(Board* b, int piece, int square, Color color) {

    PieceType p      = pieceType[piece];
    b->pieces[p]    ^= (1ULL << square);
    b->color[color] ^= (1ULL << square);
    b->occupied     ^= (1ULL << square);
}

void delPiece(Board* b, int piece, int square, Color color) {

    PieceType p      = pieceType[piece];
    b->pieces[p]    ^= (1ULL << square);
    b->color[color] ^= (1ULL << square);
    b->occupied     ^= (1ULL << square);
}

void reset(Board* b) {

    b->stm       = WHITE;
    b->enPas     = DEFAULT_EP_SQ;
    b->halfMoves = 0;
    b->ply       = 0;
    b->undoPly   = 0;
    b->fiftyMove = 0;

    b->psqtEndgame      = 0;
    b->psqtOpening      = 0;
    b->material         = 0;
    b->castlePermission = 0;

    b->zobristKey     = 0x0;
    b->zobristPawnKey = 0x0;

    for (int i = NO_PTYPE; i <= KING; i++) {
        b->pieces[i] = 0ULL;
    } 

    b->color[0] = 0ULL;
    b->color[1] = 0ULL;
    b->occupied = 0ULL;
}

Key generateZobristKey(Board* b) {

    Key key    = 0;
    int square = 0;
    int piece  = 0;

    // Hash all pieces on their current square
    Bitboard occ = b->occupied;
    while (occ) {
        square = popLSB(&occ);
        piece  = pieceAt(b, square);
        key   ^= pieceKeys[piece][square];
    }

    // Hash in sideKey if white plays
    if (b->stm == WHITE) {
        key ^= sideKey;
    }

    key ^= pieceKeys[Piece::NO_PIECE][b->enPas];
    key ^= castleKeys[b->castlePermission];

    Assert(key);
    return key;
}

Key generatePawnHashKey(Board* b) {
    
    int sq;
    Key key    = 0x0;
    Bitboard whitePawns = getPieces(b, PAWN, WHITE);
    Bitboard blackPawns = getPieces(b, PAWN, BLACK);

    while (whitePawns) {
        sq   = popLSB(&whitePawns);
        key ^= pieceKeys[Piece::P][sq];
    }

    while (blackPawns) {
        sq   = popLSB(&blackPawns);
        key ^= pieceKeys[Piece::p][sq];
    }

    sq   = getKingSquare(b, WHITE);
    key ^= pieceKeys[Piece::K][sq];
    sq   = getKingSquare(b, BLACK);
    key ^= pieceKeys[Piece::k][sq];

    return key;
}


void clearCastleRights(Board* b, Color stm) {

    if (stm == WHITE) {
        b->castlePermission &= ~K_CASTLE;
        b->castlePermission &= ~Q_CASTLE;

    } else {
        b->castlePermission &= ~k_CASTLE;
        b->castlePermission &= ~q_CASTLE;

    }
}

/**
 * @brief Updates the PSQT values according to a piece moving fromSq -> toSq.
 */
static void updatePSQTValue(Board* b, int fromSq, int toSq, int piece, Color color) {

    int relFromSq = relativeSq(fromSq, color);
    int relToSq   = relativeSq(toSq, color);

    PieceType type = pieceType[piece];
    int sign       = (color == WHITE) ? 1 : -1;

    b->psqtOpening -= sign * PSQT_OPENING[type][relFromSq];
    b->psqtOpening += sign * PSQT_OPENING[type][relToSq];
    b->psqtEndgame -= sign * PSQT_ENDGAME[type][relFromSq];
    b->psqtEndgame += sign * PSQT_ENDGAME[type][relToSq];
}

/**
 * @brief Removes the PSQT value from piece on sq.
 */
static void delPSQTValue(Board* b, int sq, int piece, Color color) {

    PieceType type = pieceType[piece];
    int relFromSq  = relativeSq(sq, color);
    int sign       = (color == WHITE) ? 1 : -1;

    b->psqtOpening -= sign * PSQT_OPENING[type][relFromSq];
    b->psqtEndgame -= sign * PSQT_ENDGAME[type][relFromSq];
}

/**
 * @brief Adds the PSQT value on sq from piece.
 */
static void addPSQTValue(Board* b, int sq, int piece, Color color) {

    PieceType type = pieceType[piece];
    int relFromSq  = relativeSq(sq, color);
    int sign       = (color == WHITE) ? 1 : -1;

    b->psqtOpening += sign * PSQT_OPENING[type][relFromSq];
    b->psqtEndgame += sign * PSQT_ENDGAME[type][relFromSq];
}

static void delMaterial(Board* b, int piece, Color color) {
    int sign = (color == WHITE) ? 1 : -1;
    b->material -= sign * pieceValues[piece];
}

static void addMaterial(Board* b, int piece, Color color) {
    int sign = (color == WHITE) ? 1 : -1;
    b->material += sign * pieceValues[piece];
}

void pushEnPas(Board* b, Move move) {

    Assert(b->enPas == toSq(move));
    int fromSquare  = fromSq(move);
    int toSquare    = b->enPas;
    int clearSquare = toSquare + 8 - (b->stm << 4);

    int fromPiece  = stmPiece[PAWN][b->stm];
    int enPasPiece = stmPiece[PAWN][!b->stm];

#if defined(USE_NNUE)
    Dirty* dp = &b->dp[b->ply + 1];

    // Moving pawn
    dp->piece[0] = fromPiece;
    dp->from[0]  = fromSquare;
    dp->to[0]    = toSquare;
    dp->changedPieces++;

    // Moving captured pawn to NO_SQ
    dp->piece[1] = enPasPiece;
    dp->from[1]  = clearSquare;
    dp->to[1]    = NO_SQ;
    dp->changedPieces++;
#endif // USE_NNUE

    // Update pawn key
    b->zobristPawnKey ^= pieceKeys[fromPiece][fromSquare]		// Remove fromSq
                      ^  pieceKeys[fromPiece][toSquare]			// Add toSq
                      ^  pieceKeys[enPasPiece][clearSquare];	// Clear captured pawn

    // Update zobrist key
    b->zobristKey ^= pieceKeys[fromPiece][fromSquare]
                  ^  pieceKeys[fromPiece][toSquare]
                  ^  pieceKeys[enPasPiece][clearSquare];

    delPiece(b, PAWN, clearSquare, !b->stm);
    delPiece(b, PAWN, fromSquare, b->stm);
    setPiece(b, PAWN, b->enPas, b->stm);

    // Calculate PSQT values on-the-fly
    updatePSQTValue(b, fromSquare, toSquare, fromPiece, b->stm);
    delPSQTValue(b, clearSquare, enPasPiece, !b->stm);
    delMaterial(b, enPasPiece, !b->stm);

    b->fiftyMove = 0;
    b->enPas = DEFAULT_EP_SQ;
}

void pushPromotion(Board* b, Move move) {

    int fromSquare = fromSq(move);
    int toSquare   = toSq(move);

    int fromPiece     = stmPiece[PAWN][b->stm];
    int toPiece       = pieceAt(b, toSquare);
    int promotedPiece = promPiece(b, move);
    Assert(pieceValidPromotion(promotedPiece));


#if defined(USE_NNUE)
    Dirty* dp = &b->dp[b->ply + 1];

    // Moving the pawn to NO_SQ
    dp->piece[0] = fromPiece;
    dp->from[0]  = fromSquare;
    dp->to[0]    = NO_SQ;
    dp->changedPieces++;

    // Place promoted piece on toSquare
    dp->piece[1] = promotedPiece;
    dp->from[1]  = NO_SQ;
    dp->to[1]    = toSquare;
    dp->changedPieces++;
#endif


    b->zobristPawnKey ^= pieceKeys[fromPiece][fromSquare];
    b->zobristKey     ^= pieceKeys[fromPiece][fromSquare]
                      ^  pieceKeys[promotedPiece][toSquare];

    // In case of promoting capture
    if (toPiece) {
        b->undoHistory[b->undoPly].cap = toPiece;
        b->zobristKey ^= pieceKeys[toPiece][toSquare];
        delPiece(b, toPiece, toSquare, !b->stm);

        // Clear from PSQT values
        delPSQTValue(b, toSquare, toPiece, !b->stm);
        delMaterial(b, toPiece, !b->stm);


#if defined(USE_NNUE)
        // Remove captured piece to NO_SQ
        dp->piece[2] = toPiece;
        dp->from[2]  = toSquare;
        dp->to[2]    = NO_SQ;
        dp->changedPieces++;
#endif
    }

    delPiece(b, fromPiece, fromSquare, b->stm);
    setPiece(b, promotedPiece, toSquare, b->stm);

    // Calculate PSQT values on-the-fly
    delPSQTValue(b, fromSquare, fromPiece, b->stm);
    addPSQTValue(b, toSquare, promotedPiece, b->stm);
    delMaterial(b, fromPiece, b->stm);
    addMaterial(b, promotedPiece, b->stm);

    b->fiftyMove = 0;
    b->enPas = DEFAULT_EP_SQ;
}

void pushCastle(Board* b, Move move) {

    int fromSquare = fromSq(move);
    int toSquare   = toSq(move);
    int rDelSq   = NO_SQ, rSetSq = NO_SQ;

    int movingRook = stmPiece[ROOK][b->stm];
    int movingKing = stmPiece[KING][b->stm];

    switch (toSquare) {
        case C1:
            rDelSq = A1;
            rSetSq = D1;
            break;
        case G1:
            rDelSq = H1;
            rSetSq = F1;
            break;
        case C8:
            rDelSq = A8;
            rSetSq = D8;
            break;
        case G8:
            rDelSq = H8;
            rSetSq = F8;
            break;
        default:
            Assert(false); exit(1);
    }

#if defined(USE_NNUE)
    Dirty* dp = &b->dp[b->ply + 1];

    // Moving the rook
    dp->piece[0] = movingRook;
    dp->from[0]  = rDelSq;
    dp->to[0]    = rSetSq;
    dp->changedPieces++;

    // Moving the king
    dp->piece[1] = movingKing;
    dp->from[1]  = fromSquare;
    dp->to[1]    = toSquare;
    dp->changedPieces++;

    dp->isKingMove = true;
#endif

    b->zobristKey ^= pieceKeys[movingRook][rDelSq]
                  ^  pieceKeys[movingRook][rSetSq]
                  ^  pieceKeys[movingKing][fromSquare]
                  ^  pieceKeys[movingKing][toSquare];

    b->zobristPawnKey ^= pieceKeys[movingKing][fromSquare]
                      ^  pieceKeys[movingKing][toSquare];

    delPiece(b, ROOK, rDelSq, b->stm);
    setPiece(b, ROOK, rSetSq, b->stm);

    delPiece(b, KING, fromSquare, b->stm);
    setPiece(b, KING, toSquare, b->stm);

    b->zobristKey ^= castleKeys[b->castlePermission];
    clearCastleRights(b, b->stm);
    b->zobristKey ^= castleKeys[b->castlePermission];

    // Update PSQT values on-the-fly
    updatePSQTValue(b, fromSquare, toSquare, movingKing, b->stm);	// King update
    updatePSQTValue(b, rDelSq, rSetSq, movingRook, b->stm);		// Rook update

    b->enPas = DEFAULT_EP_SQ;
}

void pushNormal(Board* b, Move move) {

    int fromSquare = fromSq(move);
    int toSquare   = toSq(move);

    int fromPiece     = pieceAt(b, fromSquare);
    int capturedPiece = capPiece(b, move);

#if defined(USE_NNUE)
    // NNUE
    Dirty* dp = &b->dp[b->ply + 1];

    dp->piece[0] = fromPiece;
    dp->from[0]  = fromSquare;
    dp->to[0]    = toSquare;
    dp->changedPieces++;
#endif

    // Update captured piece
    if (capturedPiece) {
        b->zobristKey ^= pieceKeys[capturedPiece][toSquare];
        delPiece(b, capturedPiece, toSquare, !b->stm);

        if (piecePawn[capturedPiece])
            b->zobristPawnKey ^= pieceKeys[capturedPiece][toSquare];


        b->undoHistory[b->undoPly].cap = capturedPiece;
        b->fiftyMove = 0;

        // Remove captured piece from PSQT values and update material
        delPSQTValue(b, toSquare, capturedPiece, !b->stm);
        delMaterial(b, capturedPiece, !b->stm);

#if defined(USE_NNUE)
        // NNUE: capture removes piece from toSq to NO_SQ
        dp->piece[1] = capturedPiece;
        dp->from[1]  = toSquare;
        dp->to[1]    = NO_SQ;
        dp->changedPieces++;
#endif
    }

    // Update normal move
    b->zobristKey ^= pieceKeys[fromPiece][fromSquare]
                  ^  pieceKeys[fromPiece][toSquare];

    delPiece(b, fromPiece, fromSquare, b->stm);
    setPiece(b, fromPiece, toSquare, b->stm);

    // Pawn start changes enPas square
    b->enPas = DEFAULT_EP_SQ;
    if (   piecePawn[fromPiece] 
        && (toSquare ^ fromSquare) == 16) 
        b->enPas = (b->stm == WHITE) ? toSquare - 8 : toSquare + 8;


    // Pawn moves reset 50-Move-Rule and change pawnKey
    if (piecePawn[fromPiece]) {
        b->fiftyMove = 0;
        b->zobristPawnKey ^= pieceKeys[fromPiece][fromSquare]
                          ^  pieceKeys[fromPiece][toSquare];
    }

    // Moving rooks loose their right to castle
    if (pieceRook[fromPiece]) {
        b->zobristKey ^= castleKeys[b->castlePermission];
        switch (fromSquare) {
            case A1: b->castlePermission &= ~Q_CASTLE; break;
            case H1: b->castlePermission &= ~K_CASTLE; break;
            case A8: b->castlePermission &= ~q_CASTLE; break;
            case H8: b->castlePermission &= ~k_CASTLE; break;
            default: break;
        }
        b->zobristKey ^= castleKeys[b->castlePermission];
    }

    // Moving kings loose their right to castle
    if (pieceKing[fromPiece]) {
        b->zobristKey ^= castleKeys[b->castlePermission];
        clearCastleRights(b, b->stm);
        b->zobristKey ^= castleKeys[b->castlePermission];

        b->zobristPawnKey ^= pieceKeys[fromPiece][fromSquare]
                          ^  pieceKeys[fromPiece][toSquare];


#if defined(USE_NNUE)
        dp->isKingMove = true;
#endif
    }

    // Update PSQT on-the-fly
    updatePSQTValue(b, fromSquare, toSquare, fromPiece, b->stm);
}

bool push(Board* b, Move move) {

    Assert(b->enPas == DEFAULT_EP_SQ || validEnPasSq(b->enPas));
    Assert(b->undoPly >= 0 && b->undoPly <= MAX_GAME_MOVES);

#ifdef USE_NNUE
    Dirty* dp = &b->dp[b->ply + 1];
    dp->changedPieces = 0;
    dp->isKingMove    = false;
    b->accum[b->ply + 1].compState[WHITE] = EMPTY;
    b->accum[b->ply + 1].compState[BLACK] = EMPTY;
#endif // USE_NNUE

    // Store data that is not worth recomputing
    Undo* undo    = &b->undoHistory[b->undoPly];
    undo->enPas   = b->enPas;
    undo->castle  = b->castlePermission;
    undo->zobKey  = b->zobristKey;
    undo->pawnKey = b->zobristPawnKey;
    undo->move    = move;
    undo->cap     = Piece::NO_PIECE;

    undo->fiftyMove   = b->fiftyMove;
    undo->psqtOpening = b->psqtOpening;
    undo->psqtEndgame = b->psqtEndgame;
    undo->material    = b->material;

    // Always let helper functions determine next ep square
    b->zobristKey ^= pieceKeys[Piece::NO_PIECE][b->enPas];

    // Always increment 50-move counter: Reset is taken care of in helper functions
    b->fiftyMove++;

    int moveType = move & (3 << 12);

    // Execute helper function depending on move types
    if (moveType == NORMAL_MOVE)
        pushNormal(b, move);

    if (moveType == EP_MOVE)
        pushEnPas(b, move);

    if (moveType == CASTLE_MOVE)
        pushCastle(b, move);

    if (moveType == PROM_MOVE)
        pushPromotion(b, move);

    // Hash in new EP square
    b->zobristKey ^= pieceKeys[Piece::NO_PIECE][b->enPas];

    // Update game state variables after (!) execution of move
    b->stm = !b->stm;
    b->zobristKey ^= sideKey;
    b->halfMoves++;
    b->undoPly++;
    b->ply++;

    Assert3(b->zobristPawnKey == generatePawnHashKey(b), getStringMove(b, move), getFEN(b));
    Assert(b->zobristKey == generateZobristKey(b));
    Assert(b->material == materialScore(b));
    Assert(b->psqtOpening == calcPSQT(b, PSQT_OPENING));
    Assert(b->psqtEndgame == calcPSQT(b, PSQT_ENDGAME));

    // Check if move leaves king in check
    if (isCheck(b, !b->stm)) {
        pop(b);
        return false;
    }

    return true;
}

void pushNull(Board* b) {

    Assert(!isCheck(b, b->stm));

    Undo* undo    = &b->undoHistory[b->undoPly];
    undo->enPas   = b->enPas;
    undo->castle  = b->castlePermission;
    undo->zobKey  = b->zobristKey;
    undo->pawnKey = b->zobristPawnKey;
    undo->move    = MOVE_NULL;
    undo->cap     = NO_PTYPE;

    undo->psqtOpening = b->psqtOpening;
    undo->psqtEndgame = b->psqtEndgame;
    undo->material    = b->material;

    b->zobristKey ^= pieceKeys[Piece::NO_PIECE][b->enPas]; // ep out
    b->enPas = DEFAULT_EP_SQ;
    b->zobristKey ^= pieceKeys[Piece::NO_PIECE][b->enPas]; // ep in

    // update game state variables
    b->stm = !b->stm;
    b->zobristKey ^= sideKey;

    Assert(b->zobristKey     == generateZobristKey(b));
    Assert(b->zobristPawnKey == generatePawnHashKey(b));

    // update gameState variable and store in undo struct
    undo->fiftyMove = b->fiftyMove;

    b->fiftyMove++;
    b->halfMoves++;
    b->undoPly++;
    b->ply++;
    
#if defined (USE_NNUE)
    Dirty* dp = &b->dp[b->ply]; // ply already incremented
    dp->changedPieces = 0;
    dp->piece[0] = Piece::NO_PIECE;

    b->accum[b->ply].compState[WHITE] = EMPTY;
    b->accum[b->ply].compState[BLACK] = EMPTY;
#endif
}

void clearCastlePermission(Board* b, int side) {

    if (side == WHITE) {
        b->castlePermission &= ~K_CASTLE;
        b->castlePermission &= ~Q_CASTLE;
    } else {
        b->castlePermission &= ~k_CASTLE;
        b->castlePermission &= ~q_CASTLE;
    }
}

Undo pop(Board* b) {
    
    b->halfMoves--;
    b->undoPly--;
    b->ply--;

    // Flip side before clear and set pieces
    b->stm = !b->stm;

    // reset board variables
    Undo* undo        = &b->undoHistory[b->undoPly];
    b->castlePermission = undo->castle;
    b->fiftyMove        = undo->fiftyMove;
    b->zobristKey       = undo->zobKey;
    b->zobristPawnKey   = undo->pawnKey;
    b->enPas            = undo->enPas;

    b->psqtOpening = undo->psqtOpening;
    b->psqtEndgame = undo->psqtEndgame;
    b->material    = undo->material;

    int fromSquare    = fromSq(undo->move);
    int toSquare      = toSq(undo->move);
    int movingPiece   = pieceAt(b, toSquare);
    int capturedPiece = undo->cap;

    // Move back moving piece
    setPiece(b, movingPiece, fromSquare, b->stm);
    delPiece(b, movingPiece, toSquare, b->stm);

    // Reset captured piece
    if (capturedPiece) {
        setPiece(b, capturedPiece, toSquare, !b->stm);
    }

    // EP captures
    if (isEnPassant(undo->move)) {
        if (b->stm == WHITE) setPiece(b, PAWN, toSquare - 8, !b->stm);
        else setPiece(b, PAWN, toSquare + 8, !b->stm);
    }

    // Promotions
    if (promPiece(b, undo->move)) {
        delPiece(b, movingPiece, fromSquare, b->stm);
        setPiece(b, PAWN, fromSquare, b->stm);
    }

    // undo castles
    if (isCastling(undo->move)) {
        switch (toSquare) {
            case C1: popCastle(b, D1, A1, WHITE); break;
            case G1: popCastle(b, F1, H1, WHITE); break;
            case C8: popCastle(b, D8, A8, BLACK); break;
            case G8: popCastle(b, F8, H8, BLACK); break;
            default: Assert(0);
        }
    }

    Assert(b->undoPly >= 0);
    Assert(validEnPasSq(b->enPas) || b->enPas == DEFAULT_EP_SQ);
    Assert(checkBoard(b));
    Assert(b->material == materialScore(b));
    Assert(b->psqtOpening == calcPSQT(b, PSQT_OPENING));
    Assert(b->psqtEndgame == calcPSQT(b, PSQT_ENDGAME));

    return *undo;
}

void popCastle(Board* b, int clearRookSq, int setRookSq, Color color) {

    Assert(pieceAt(b, clearRookSq) == Piece::R || pieceAt(b, clearRookSq) == Piece::r);

    delPiece(b, ROOK, clearRookSq, color);
    setPiece(b, ROOK, setRookSq, color);
}

Undo popNull(Board* b) {
    
    b->halfMoves--;
    b->undoPly--;
    b->ply--;
    Assert(b->undoPly >= 0);

    // Flip side before clear and set pieces
    b->stm = !b->stm;

    // Reset board variables
    Undo* undo          = &b->undoHistory[b->undoPly];
    b->castlePermission = undo->castle;
    b->fiftyMove        = undo->fiftyMove;
    b->zobristKey       = undo->zobKey;
    b->zobristPawnKey   = undo->pawnKey;
    b->enPas            = undo->enPas;

    Assert(undo->move == MOVE_NULL);

    return *undo;
}

Move getCurrentMove(Board* b) {
    return b->undoPly > 0 ? b->undoHistory[b->undoPly - 1].move
                          : MOVE_NONE;
}

bool isCheck(Board* b, Color color) {

    int kSq = getKingSquare(b, color);

    if (   pawnAtkMask[color][kSq] & getPieces(b, PAWN, !color)
        || knightAtkMask[kSq]      & getPieces(b, KNIGHT, !color)
        || lookUpBishopMoves(kSq, b->occupied) & getDiagPieces(b, !color)
        || lookUpRookMoves(kSq, b->occupied)   & getVertPieces(b, !color))
        return true;

    return false;
}

bool sqIsBlockerForKing(Board* b, int kSq, Color movingSide, int potBlockerSq) {

    int sq;
    Bitboard pinner = 0;

    Bitboard kingSlider = lookUpRookMoves(kSq, b->occupied);
    Bitboard potBlocker = kingSlider & b->color[movingSide];
    Bitboard xrays      = kingSlider ^ lookUpRookMoves(kSq, b->occupied ^ potBlocker);

    pinner |= xrays & (getVertPieces(b, b->stm));
    while (pinner) {
        sq = popLSB(&pinner);
        if (obstructed(kSq, sq) & (1ULL << potBlockerSq)) return true;
    }

    kingSlider = lookUpBishopMoves(kSq, b->occupied);
    potBlocker = kingSlider & b->color[movingSide];
    xrays      = kingSlider ^ lookUpBishopMoves(kSq, b->occupied ^ potBlocker);

    pinner |= xrays & (getDiagPieces(b, b->stm));
    while (pinner) {
        sq = popLSB(&pinner);
        if (obstructed(kSq, sq) & (1ULL << potBlockerSq)) return true;
    }

    return false;
}

bool checkingMove(Board* b, Move move) {
    int from = fromSq(move);
    int to = toSq(move);
    int movingPiece = pieceAt(b, from);
    int kingSq = getKingSquare(b, !b->stm);
    Bitboard kingMask = (1ULL << kingSq);

    // 1. Direct check: atkMask hits kingSq
    Assert(pieceValid(movingPiece))
    switch (movingPiece) {
        case Piece::P:
        case Piece::p:
            if (kingMask & pawnAtkMask[b->stm][to]) 
                return true;
            break;
        case Piece::N:
        case Piece::n:
            if (kingMask & knightAtkMask[to]) 
                return true; 
            break;
        case Piece::B:
        case Piece::b:
            if (kingMask & lookUpBishopMoves(to, b->occupied)) 
                return true; 
            break;
        case Piece::R:
        case Piece::r:
            if (kingMask & lookUpRookMoves(to, b->occupied)) 
                return true; 
            break;
        case Piece::Q:
        case Piece::q:
            if (   lookUpBishopMoves(to, b->occupied) & kingMask
                || lookUpRookMoves(to, b->occupied)   & kingMask) 
                return true; 
            break;
    }

    // TODO Maybe use getDiscoveredAttacks with params w-b, n, q here.
    //
    // 2. Discovered attack: Slider can hit kingSq 
    // If (from, to, kSq) are not aligned, check if xray from king hits any sliders
    if (!aligned(kingSq, from, to)
        && (sqIsBlockerForKing(b, kingSq, b->stm, from))) {
        return true;
    }

    // 3. Special pawn checks 
    if (piecePawn[movingPiece]) {
        // 3.1 Promoted piece gives check
        switch (promPiece(b, move)) {
            case Piece::N:
            case Piece::n:
                return (knightAtkMask[to] & kingMask);
            case Piece::B:
            case Piece::b:
                return (lookUpBishopMoves(to, b->occupied ^ (1ULL << from)) & kingMask);
            case Piece::R:
            case Piece::r:
                return (lookUpRookMoves(to, b->occupied ^ (1ULL << from)) & kingMask);
            case Piece::Q:
            case Piece::q:
                return (lookUpBishopMoves(to, b->occupied ^ (1ULL << from)) & kingMask)
                    || (lookUpRookMoves(to, b->occupied ^ (1ULL << from)) & kingMask);
        }

        // 3.2 EnPassant check
        if (isEnPassant(move)) {
            int capSq = b->enPas + ((b->stm == WHITE) ? -8 : 8);

            // Hack enpas-piece temporary away and restore after check
            b->occupied ^= (1ULL << capSq);
            b->occupied ^= (1ULL << from);
            b->occupied ^= (1ULL << to);
            bool check = squareAttackedBy(b, kingSq, b->stm);
            b->occupied ^= (1ULL << capSq);
            b->occupied ^= (1ULL << to);
            b->occupied ^= (1ULL << from);

            return check;
        }
        return false;
    }

    // 4. Castled rook gives check
    if (isCastling(move)) {
        switch (to) {
            case G1: // => rook moves to F1
                return lookUpRookMoves(F1, b->occupied ^ (1ULL << E1)) & kingMask;
            case C1: // => rook moves to D1
                return lookUpRookMoves(D1, b->occupied ^ (1ULL << E1)) & kingMask;
            case G8: // => rook moves to F8
                return lookUpRookMoves(F8, b->occupied ^ (1ULL << E8)) & kingMask;
            case C8: // => rook moves to D8
                return lookUpRookMoves(D8, b->occupied ^ (1ULL << E8)) & kingMask;
            default: Assert(false); break;
        }
    }

    return false;
}

bool castleValid(Board* b, int castle, Bitboard* attackerSet) {
    if (   isCheck(b, b->stm)
        || !(b->castlePermission & castle))
        return false;

    switch (castle) {
        case K_CASTLE:
            if (   ((setMask[F1] | setMask[G1]) & b->occupied)
                || (pieceAt(b, H1) != Piece::R)
                || (*attackerSet & setMask[F1]) | (*attackerSet & setMask[G1]))
                return false;
            break;

        case Q_CASTLE:
            if (   ((setMask[D1] | setMask[C1] | setMask[B1]) & b->occupied)
                || (pieceAt(b, A1) != Piece::R)
                || (*attackerSet & setMask[D1]) | (*attackerSet & setMask[C1]))
                return false;
            break;

        case k_CASTLE:
            if (   ((setMask[F8] | setMask[G8]) & b->occupied)
                || (pieceAt(b, H8) != Piece::r)
                || (*attackerSet & setMask[F8]) | (*attackerSet & setMask[G8]))
                return false;
            break;

        case q_CASTLE:
            if (   ((setMask[D8] | setMask[C8] | setMask[B8]) & b->occupied)
                || (pieceAt(b, A8) != Piece::r)
                || (*attackerSet & setMask[D8]) | (*attackerSet & setMask[C8]))
                return false;
            break;

        default: return false; break;
    }

    return true;
}

bool checkBoard(Board* board) {

    Assert(board->castlePermission >= 0 && board->castlePermission <= 15);
    Assert(popCount(board->occupied) >= 2 && popCount(board->occupied) <= 32);
    Assert(validEnPasSq(board->enPas) || board->enPas == DEFAULT_EP_SQ);
    Assert(board->zobristKey == generateZobristKey(board));
    Assert(board->zobristPawnKey == generatePawnHashKey(board));

    return true;
}
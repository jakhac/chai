#include "perft.h"


long long FastPerft::perftRoot(Board* b, int depth) {

    Assert(checkBoard(b));
    cout << endl << "Perft to depth " << depth << endl;
    auto start = std::chrono::high_resolution_clock::now();

    leafNodes = 0;
    bool inCheck = isCheck(b, b->stm);
    MoveList _moveList[1];
    generateMoves(b, _moveList, inCheck);

    int move;
    for (int i = 0; i < _moveList->cnt; i++) {
        move = _moveList->moves[i];

#ifdef ASSERT
        bool moveGivesCheck = checkingMove(b, move);
#endif // ASSERT

        // Skip illegal moves
        if (!push(b, move)) {
            Assert(!inCheck);
            continue;
        }

        Assert(isCheck(b, b->stm) == moveGivesCheck);

        long long cumnodes = leafNodes;
        perft(b, depth - 1);
        pop(b);
        long long oldnodes = leafNodes - cumnodes;

        cout << getStringMove(b, move) << " : " << oldnodes << endl;
    }

    cout << "\nRun finished : " << leafNodes << " leaves visited" << endl;
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    cout << duration.count() << "ms\n";
    return leafNodes;
}

void FastPerft::perft(Board* b, int depth) {

    Assert(checkBoard(b));

    if (depth == 0) {
        leafNodes++;
        return;
    }

    bool inCheck = isCheck(b, b->stm);
    MoveList _moveList[1];
    generateMoves(b, _moveList, inCheck);

    Assert(attackerSet(b, !b->stm) == _moveList->attackedSquares);

    int move;
    for (int i = 0; i < _moveList->cnt; i++) {
        move = _moveList->moves[i];

#ifdef ASSERT
        bool moveGivesCheck = checkingMove(b, move);
#endif // ASSERT

        // Skip illegal moves
        if (!push(b, move)) {
            Assert(!inCheck);
            continue;
        }

            Assert(isCheck(b, b->stm) == moveGivesCheck);

            perft(b, depth - 1);
            pop(b);
    }
}


long long StatPerft::perftRoot(Board* b, int depth) {

    Assert(checkBoard(b));
    cout << endl << "Perft to depth " << depth << endl;
    auto start = std::chrono::high_resolution_clock::now();

    leafNodes = 0;
    bool inCheck = isCheck(b, b->stm);

    MoveList _moveList[1];
    generateMoves(b, _moveList, inCheck);

    int move;
    for (int i = 0; i < _moveList->cnt; i++) {
        move = _moveList->moves[i];

        bool moveGivesCheck = checkingMove(b, move);
        bool capture = capPiece(b, move);
        bool prom = isPromotion(move);
        bool ep = isEnPassant(move);
        bool castle = isCastling(move);

        // Skip illegal moves
        if (!push(b, move)) {
            Assert(!inCheck);
            continue;
        }

        Assert(isCheck(b, b->stm) == moveGivesCheck);

        if (depth == 1) {
            if (moveGivesCheck) checks++;
            if (ep || capture) captures++;
            if (ep) enPas++;
            if (prom) proms++;
            if (castle) castles++;
        }

        long long cumnodes = leafNodes;
        perft(b, depth - 1);
        pop(b);
        long long oldnodes = leafNodes - cumnodes;

        cout << getStringMove(b, move) << " : " << oldnodes << endl;
    }

    cout << "\nRun finished : " << leafNodes << " leaves visited" << endl;

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    cout << duration.count() << "ms\n";
    return leafNodes;
}

void StatPerft::perft(Board* b, int depth) {

    Assert(checkBoard(b));

    if (depth == 0) {
        leafNodes++;
        if (isMate(b)) mates++;

        return;
    }

    bool inCheck = isCheck(b, b->stm);
    MoveList _moveList[1];
    generateMoves(b, _moveList, inCheck);


    Assert(attackerSet(b, !b->stm) == _moveList->attackedSquares);

    int move;
    for (int i = 0; i < _moveList->cnt; i++) {
        move = _moveList->moves[i];

        bool moveGivesCheck = checkingMove(b, move);
        bool capture = isCapture(b, move);
        bool prom = isPromotion(move);
        bool ep = isEnPassant(move);
        bool castle = isCastling(move);

        // Skip illegal moves
        if (!push(b, move)) {
            Assert(!inCheck);
            continue;
        }

        Assert(isCheck(b, b->stm) == moveGivesCheck);

        // Collect statistics at leaf nodes
        if (depth == 1) {
            if (moveGivesCheck) checks++;
            if (ep ||capture) captures++;
            if (ep) enPas++;
            if (prom) proms++;
            if (castle) castles++;
        }

        perft(b, depth - 1);
        pop(b);
    }

}

void StatPerft::printStats() {
    
    cout << "\nStatistics:" << endl
         << "Leaves: " << leafNodes << endl
         << "Caps: \t" << captures << endl
         << "EnPas: \t" << enPas << endl
         << "Proms: \t" << proms << endl
         << "Checks: " << checks << endl
         << "Mates: \t" << mates << endl
         << "Castle: " << castles << endl
         << endl;

}
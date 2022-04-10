#include "perft.h"


long long StatPerft::perftRoot(Board* b, int depth) {

    Assert(checkBoard(b));
    cout << endl << "Perft to depth " << depth << endl;
    auto start = std::chrono::high_resolution_clock::now();

    leafNodes = 0;
    bool inCheck = isCheck(b, b->stm);

    MoveList moveList[1];
    generateMoves(b, moveList, inCheck);

    int move;
    for (int i = 0; i < moveList->cnt; i++) {
        move = moveList->moves[i];

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

        perft(b, depth - 1, &leafList[i]);
        pop(b);

        cout << getStringMove(b, move) << " : " << leafList[i] << endl;
        leafNodes += leafList[i];
    }

    cout << "\nRun finished : " << leafNodes << " leaves visited" << endl;

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    cout << duration.count() << "ms\n";
    return leafNodes;
}

void StatPerft::perft(Board* b, int depth, long long* leaves) {

    Assert(checkBoard(b));

    if (depth == 0) {
        (*leaves)++;
        if (isMate(b)) mates++;

        return;
    }

    bool inCheck = isCheck(b, b->stm);
    MoveList moveList[1];
    generateMoves(b, moveList, inCheck);


    Assert(attackerSet(b, !b->stm) == moveList->attackedSquares);

    int move;
    for (int i = 0; i < moveList->cnt; i++) {
        move = moveList->moves[i];

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

        perft(b, depth - 1, leaves);
        pop(b);
    }

}

long long FastPerft::perftRoot(Board* b, int depth) {

    cout << endl << "Perft to depth " << depth << endl;

    auto start = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> perfter(MAX_THREADS);
    for (unsigned int i = 0; i < MAX_THREADS; i++)
        perfter[i] = std::thread(&FastPerft::perftJob, this, *b, depth, i);

    for (unsigned int i = 0; i < MAX_THREADS; i++)
        perfter[i].join();

    for (int i = 0; i < MAX_POSITION_MOVES; i++)
        leafNodes += leafList[i];

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    cout << "\nRun finished in "
         << duration.count() << "ms : " 
         << leafNodes << " leaves visited" << endl;
    return leafNodes;

}

void FastPerft::perftJob(Board b, int depth, int idx) {

    std::stringstream stream;
    MoveList moveList[1];
    generateMoves(&b, moveList, isCheck(&b, b.stm));

    for (int i = idx; i < moveList->cnt; i += MAX_THREADS) {
        int move = moveList->moves[i];

        // Skip illegal moves
        if (!push(&b, move)) {
            continue;
        }

        perft(&b, depth - 1, &leafList[i]);
        pop(&b);

        stream << "T" << idx << " has " << getStringMove(&b, move) 
               << " : " << leafList[i] << "\n"; 
        cout << stream.str();
    }

}

void FastPerft::perft(Board* b, int depth, long long* leaves) {
    
    if (depth == 0) {
        (*leaves)++;
        return;
    }

    bool inCheck = isCheck(b, b->stm);
    MoveList moveList[1];
    generateMoves(b, moveList, inCheck);

    for (int i = 0; i < moveList->cnt; i++) {
        int move = moveList->moves[i];

        // Skip illegal moves
        if (!push(b, move)) {
            continue;
        }

        perft(b, depth - 1, leaves);
        pop(b);
    }

}

void StatPerft::printStats() {
    
    cout << "\nStats:" << endl
         << "Caps: \t" << captures << endl
         << "EnPas: \t" << enPas << endl
         << "Proms: \t" << proms << endl
         << "Checks: " << checks << endl
         << "Mates: \t" << mates << endl
         << "Castle: " << castles << endl
         << endl;

}
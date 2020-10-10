#include "perft.h"

long long Perft::perftRoot(Board* b, int depth) {
    ASSERT(b->checkBoard());
    printf("\nPerft to depth %d\n", depth);
    auto start = std::chrono::high_resolution_clock::now();

    leafNodes = 0;
    MOVE_S _moveList[1];
    generateMoves(b, _moveList);

    int move;
    int moveNum = 0;
    for (int i = 0; i < _moveList->moveCounter; i++) {
        move = _moveList->moveList[i];

        // skip illegal moves
        if (!b->push(move)) continue;

        long long cumnodes = leafNodes;
        perft(b, depth - 1);
        b->pop();
        long long oldnodes = leafNodes - cumnodes;

        cout << getStringMove(move) << " : " << oldnodes << endl;
    }

    cout << "\nTest Complete : " << leafNodes << " nodes visited" << endl;
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    cout << duration.count() << "ms\n";
    return leafNodes;
}

void Perft::perft(Board* b, int depth) {

    ASSERT(b->checkBoard());

    if (depth == 0) {
        leafNodes++;
        return;
    }

    MOVE_S _moveList[1];
    generateMoves(b, _moveList);

    int move;
    //for (auto m : moveGenerator.getAllMoves()) {
        //move = m.move;
    for (int i = 0; i < _moveList->moveCounter; i++) {
        move = _moveList->moveList[i];

        // skip illegal moves
        if (!b->push(move)) continue;

        perft(b, depth - 1);
        b->pop();
    }
}

long long Perft::perftBulkRoot(Board* b, int depth) {
    ASSERT(b->checkBoard());
    printf("\nPerft to depth %d\n", depth);
    auto start = std::chrono::high_resolution_clock::now();

    leafNodes = 0;
    MOVE_S _moveList[1];
    _generateMoves(b, _moveList);

    int move;
    int moveNum = 0;
    for (int i = 0; i < _moveList->moveCounter; i++) {
        move = _moveList->moveList[i];

        b->push(move);

        long long cumnodes = leafNodes;
        leafNodes += perftBulk(b, depth - 1);
        b->pop();
        long long oldnodes = leafNodes - cumnodes;

        cout << getStringMove(move) << " : " << oldnodes << endl;
    }

    cout << "\nTest Complete : " << leafNodes << " nodes visited" << endl;
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    cout << duration.count() << "ms\n";
    return leafNodes;
}


long long Perft::perftBulk(Board* b, int depth) {

    long long nodes = 0;

    MOVE_S move_s[1];
    _generateMoves(b, move_s);

    if (depth == 1)
        return move_s->moveCounter;

    for (int i = 0; i < move_s->moveCounter; i++) {
        b->push(move_s->moveList[i]);
        nodes += perftBulk(b, depth - 1);
        b->pop();
    }

    return nodes;
}
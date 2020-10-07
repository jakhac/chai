#include "perft.h"

long Perft::perftRoot(Board b, int depth) {
    ASSERT(b.checkBoard());
    printf("\nPerft to depth %d\n", depth);

    leafNodes = 0;
    MoveGenerator moveGenerator;
    moveGenerator.generateMoves(b);

    int move;
    int moveNum = 0;
    for (auto m : moveGenerator.getAllMoves()) {
        move = m.move;

        // skip illegal moves
        if (!b.push(move)) continue;

        long cumnodes = leafNodes;
        perft(b, depth - 1);
        b.pop();
        long oldnodes = leafNodes - cumnodes;

        cout << getStringMove(move) << " : " << oldnodes << endl;
    }

    printf("\nTest Complete : %ld nodes visited\n", leafNodes);
    return leafNodes;
}

void Perft::perft(Board b, int depth) {

    ASSERT(b.checkBoard());

    if (depth == 0) {
        leafNodes++;
        return;
    }

    MoveGenerator moveGenerator;
    moveGenerator.generateMoves(b);

    int move;
    for (auto m : moveGenerator.getAllMoves()) {
        move = m.move;

        // skip illegal moves
        if (!b.push(move)) continue;

        perft(b, depth - 1);
        b.pop();
    }
}

long Perft::perftBulkRoot(Board b, int depth) {
    ASSERT(b.checkBoard());
    printf("\nPerft to depth %d\n", depth);

    leafNodes = 0;
    MoveGenerator moveGenerator;
    moveGenerator.generateMoves(b);

    int move;
    int moveNum = 0;
    for (auto m : moveGenerator.getAllMoves()) {
        move = m.move;
        
        b.push(move);
        long cumnodes = leafNodes;
        perft(b, depth - 1);
        b.pop();
        long oldnodes = leafNodes - cumnodes;

        cout << getStringMove(move) << " : " << oldnodes << endl;
    }

    printf("\nTest Complete : %ld nodes visited\n", leafNodes);
    return leafNodes;
}

long Perft::perftBulk(Board b, int depth) {

    ASSERT(b.checkBoard());

    MoveGenerator moveGenerator;
    moveGenerator.generateMoves(b);
    vector<Move> allMoves = moveGenerator.getAllMoves();

    if (depth == 1) {
        return allMoves.size();
    }

    int move;
    for (auto m : allMoves) {
        move = m.move;

        b.push(move);
        perft(b, depth - 1);
        b.pop();
    }
}

// legal move gen
//      |
//      v
long Perft::perftRootLegal(Board b, int depth) {
    ASSERT(b.checkBoard());
    printf("\nPerft to depth %d\n", depth);

    leafNodes = 0;
    LegalMoveGenerator lmg;
    lmg._generateMoves(b);

    int move;
    int moveNum = 0;
    for (auto m : lmg._getAllMoves(b)) {
        move = m.move;

        b.push(move);
        long cumnodes = leafNodes;
        perftLegal(b, depth - 1);
        b.pop();
        long oldnodes = leafNodes - cumnodes;

        cout << getStringMove(move) << " : " << oldnodes << endl;
    }

    printf("\nTest Complete : %ld nodes visited\n", leafNodes);
    return leafNodes;
}

void Perft::perftLegal(Board b, int depth) {

    ASSERT(b.checkBoard());

    if (depth == 0) {
        leafNodes++;
        return;
    }

    LegalMoveGenerator lmg;
    lmg._generateMoves(b);

    int move;
    for (auto m : lmg._getAllMoves(b)) {
        move = m.move;

        b.push(move);
        perftLegal(b, depth - 1);
        b.pop();
    }
}

long Perft::perftBulkRootLegal(Board b, int depth) {
    ASSERT(b.checkBoard());
    printf("\nPerft to depth %d\n", depth);

    leafNodes = 0;
    LegalMoveGenerator lmg;
    lmg._generateMoves(b);

    int move;
    int moveNum = 0;
    for (auto m : lmg._getAllMoves(b)) {
        move = m.move;

        b.push(move);
        long cumnodes = leafNodes;
        perft(b, depth - 1);
        b.pop();
        long oldnodes = leafNodes - cumnodes;

        cout << getStringMove(move) << " : " << oldnodes << endl;
    }

    printf("\nTest Complete : %ld nodes visited\n", leafNodes);
    return leafNodes;
}

long Perft::perftBulkLegal(Board b, int depth) {

    ASSERT(b.checkBoard());

    LegalMoveGenerator lmg;
    lmg._generateMoves(b);
    vector<Move> allMoves = lmg._getAllMoves(b);

    if (depth == 1) {
        return allMoves.size();
    }

    int move;
    for (auto m : allMoves) {
        move = m.move;

        b.push(move);
        perftBulkLegal(b, depth - 1);
        b.pop();
    }
}

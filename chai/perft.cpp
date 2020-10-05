#include "perft.h"

template<typename T>
std::vector<T> slice(std::vector<T>& v, int m, int n) {
    std::vector<T> vec(n - m + 1);
    std::copy(v.begin() + m, v.begin() + n + 1, vec.begin());
    return vec;
}

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
        if (!b.push(move)) {
            continue;
        }

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

U64 Perft::perftBulk(Board b, int depth) {
    return -1;
    int n_moves, i;
    U64 nodes = 0;

    MoveGenerator mg;
    mg.generateMoves(b);
    n_moves = mg.capMoveList.size() + mg.quietMoveList.size();

    if (depth == 1) {
         return (U64)n_moves;
    }

    for (auto move : mg.getAllMoves()) {
        if (!b.push(move.move)) continue;
        nodes += perftBulk(b, depth - 1);
        b.pop();
    }
    return nodes;
}

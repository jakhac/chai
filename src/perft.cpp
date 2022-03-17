#include "perft.h"

void dividePerft(Perft* p, Board* b, int depth) {
    std::string move = "";

    while (depth) {

        if (move != "") {
            int parsedMove = parseMove(b, move);
            push(b, parsedMove);
            depth--;
        }

        p->perftRoot(b, depth);

        cout << "\nDivide at move ";
        cin >> move;

        if (move == "quit") {
            return;
        }

    }

    cout << "Reached depth 0" << endl;
    return;

}

long long Perft::perftRoot(Board* b, int depth) {
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

    cout << "\nTest Complete : " << leafNodes << " nodes visited" << endl;
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    cout << duration.count() << "ms\n";
    return leafNodes;
}

void Perft::perft(Board* b, int depth) {
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

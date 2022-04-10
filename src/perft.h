#pragma once

#include "moveGenerator.h"
#include "thread.h"

/**
 * Generic class for perfting applications.
 */
class GenericPerft {
    
    /**
     * Standard perft root call.
     *
     * @param  b	 The Board reference.
     * @param  depth Perft depth.
     *
     * @returns The number of leaf nodes counted at given depth.
     */
    virtual long long perftRoot(Board* b, int depth) = 0;

    /**
     * Recursive call for perft using {@link #isLegal(Board, Move)} isLegal function.
     *
     * @param  b	 The board reference.
     * @param  depth Depth remaining.
     */
    virtual void perft(Board* b, int depth, long long* leaves) = 0;

};

class StatPerft : public GenericPerft {

private:

    long long leafList[MAX_POSITION_MOVES] = { 0 };
    long long leafNodes = 0;
    long long captures  = 0;
    long long proms     = 0;
    long long enPas     = 0;
    long long checks    = 0;
    long long mates     = 0;
    long long castles   = 0;

public:

    long long perftRoot(Board* b, int depth) override;
    void perft(Board* b, int depth, long long* leaves) override;

    void printStats();
    void resetStats() {
        std::fill(std::begin(leafList), std::end(leafList), 0);
        leafNodes = 0;
        captures  = 0;
        proms     = 0;
        enPas     = 0;
        checks    = 0;
        mates     = 0;
        castles   = 0;
    }
};

class FastPerft : public GenericPerft {

private:

    long long leafNodes = 0;
    long long leafList[MAX_POSITION_MOVES] = { 0 };

public:

    long long perftRoot(Board* b, int depth) override;
    void perft(Board* b, int depth, long long* leaves) override;
    void perftJob(Board b, int depth, int idx);

    void printStats() {};
    void resetStats() {
        leafNodes = 0;
        std::fill(std::begin(leafList), std::end(leafList), 0);
    }

};


/**
 * Function to divide with cmd-line move after each perft.
 *
 * @param  b	 Board reference.
 * @param  depth The initial depth.
 */
template<class PerftClass>
void dividePerft(Board* b, int depth) {
    
    PerftClass p;
    std::string move;

    int pushedMoves = 0;

    p.resetStats();
    p.perftRoot(b, depth--);
    p.printStats();

    while (depth) {

        cout << "\nNext depth " << depth << endl
             << "Enter next move (or q to quit): ";
        cin >> move;
        cin.ignore();

        if (move == "q") {
            cout << endl;
            return;
        }

        int parsedMove = parseMove(b, move);
        Assert(parsedMove != MOVE_NONE);
        Assert(parsedMove != MOVE_NULL);

        push(b, parsedMove);
        pushedMoves++;

        p.resetStats();
        p.perftRoot(b, depth--);
        p.printStats();
    }

    while (pushedMoves--) pop(b);

    cout << "\nReached depth 0" << endl << endl;
}
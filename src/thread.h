#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>

#include "move.h"


#define MAX_THREADS std::max((unsigned int)1, std::thread::hardware_concurrency())


extern int NUM_THREADS;
extern bool TERMINATE_THREADS;
extern bool ABORT_SEARCH;



class ThreadWrapper {

private:
    std::mutex mtx;
    std::condition_variable cond;

public:

    ThreadWrapper(int p_ID) {
        id = p_ID;
    }

    ~ThreadWrapper() {
        if (t.joinable()) {
            t.join();
        }
    }

    int id;
	bool searching;
    
    int depth;
    int selDepth;

	long nodes;
	long qnodes;

    std::thread t;

	Board b;
	Stats s;
    Instructions instr;

	// Search heuristics for each thread
	Move killer[2][MAX_GAME_MOVES];
	Move mateKiller[MAX_GAME_MOVES];
	Move counterHeuristic[64][64][2];
	int histHeuristic[2][64][64];
	int histMax = 0;

	// Search parameters
	Value bestScore;
	Move bestMove;
	SearchStack ss[MAX_DEPTH + 1];
	Move pvLine[MAX_DEPTH + 1];

    void startIdle() {
        t = std::thread(&ThreadWrapper::idle, this);
    }

    void idle();

    void resetThreadStates(Board* board, Stats* search, Instructions* instructions);

    void startThread();
    void waitThread();

};

typedef std::shared_ptr<ThreadWrapper> Thread;

extern std::vector<Thread> threadPool;


namespace Threads {

void initPool();
void deletePool();

// Return true if numThreads is valid, else false
bool resizePool(size_t numThreads);

void resetAllThreadStates(Board* b, Stats* s, Instructions* i);
void startAllThreads();
void waitAllThreads();

int selectBestThreadIndex();

int totalNodeCount();

// Forward declaration
void iid(Thread thread);

}

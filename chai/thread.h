#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>

#include "move.h"

const int MAX_THREADS = std::thread::hardware_concurrency() - 1;

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

	board_t b;
	stats_t s;
    instr_t instr;

	// Search heuristics for each thread
	move_t killer[2][MAX_GAME_MOVES];
	move_t mateKiller[MAX_GAME_MOVES];
	move_t counterHeuristic[64][64][2];
	int histHeuristic[2][64][64];
	int histMax = 0;

	// Search parameters
	value_t bestScore;
	move_t bestMove;
	searchStack_t ss[MAX_DEPTH + 1];
	move_t pvLine[MAX_DEPTH + 1];

    void startIdle() {
        t = std::thread(&ThreadWrapper::idle, this);
    }

    void idle();

    void resetThreadStates(board_t* board, stats_t* search, instr_t* instructions);

    void startThread();
    void waitThread();

};

typedef std::shared_ptr<ThreadWrapper> Thread;

extern std::vector<Thread> threadPool;

// Forward declaration
void iid(Thread thread);

void initThreadPool();
void deleteThreadPool();

// Return true if numThreads is valid, else false
bool resizeThreadPool(size_t numThreads);

void resetAllThreadStates(board_t* b, stats_t* s, instr_t* i);
void startAllThreads();
void waitAllThreads();

int selectBestThreadIndex();

int totalNodeCount();
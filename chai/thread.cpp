#include "thread.h"

// Makefile might set number of threads. If not, set to maximum.
#ifdef CUSTOM_THREADS
int NUM_THREADS = std::min(CUSTOM_THREADS, (int)std::thread::hardware_concurrency() - 1);
#else
int NUM_THREADS = MAX_THREADS; 
#endif // CUSTOM_THREADS


bool TERMINATE_THREADS = false;
bool ABORT_SEARCH = false;


std::vector<Thread> threadPool;


void ThreadWrapper::resetThreadStates(board_t* board, search_t* search) {
    b = *board;
    s = *search;

    // Reset heuristics: Matekiller, Killer, Counter, History
    std::fill_n(mateKiller, 
        MAX_GAME_MOVES, MOVE_NONE);

    for (int i = 0; i < 2; i++) {
        std::fill_n(killer[i], MAX_DEPTH, 0);
    }

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < NUM_SQUARES; j++) {
            for (int l = 0; l < NUM_SQUARES; l++) {
                histHeuristic[i][j][l] = 0;
                counterHeuristic[j][l][i] = MOVE_NONE;
            }
        }
    }
    histMax = 0;

    std::fill_n(pvLine, MAX_DEPTH + 1, MOVE_NONE);
}

void ThreadWrapper::idle() {
    while (!TERMINATE_THREADS) {
        // Block this thread, until notified by search to resume
        {
            std::unique_lock<std::mutex> lock(mtx);
            searching = false;
            // cout << "T" << id << ": conditionally blocked." << endl;
            cond.notify_one();
            cond.wait(lock, [&] {
                return searching;
            });
        }

        if (TERMINATE_THREADS)
            break;
        
        // Returned from conditional wait: Start searching
        // cout << "T" << id << " searches." << endl;
        searching = true;
        iid(threadPool[this->id]);
        searching = false;
        // cout << "T" << id << " finished search." << endl;
    }

    // cout << "T" << id << " break from idle" << endl;
}

void ThreadWrapper::startThread() {
    std::unique_lock<std::mutex> lock(mtx);
    cond.notify_one();
}

void ThreadWrapper::waitThread() {
    std::unique_lock<std::mutex> lock(mtx);
    cond.wait(lock, [&]{
        return !searching;
    });
}

void initThreadPool() {

    threadPool.clear();
    TERMINATE_THREADS = false;
    ABORT_SEARCH = false;

    // Main thread, does not start a thread
    auto a = std::make_shared<ThreadWrapper>(0);
    threadPool.push_back(a);

    // Remaining workers are instantiated set to idle
    for (int i = 1; i < NUM_THREADS; i++) {
        a = std::make_shared<ThreadWrapper>(i);
        threadPool.push_back(a);
        threadPool[i]->startIdle();
    }

}

void deleteThreadPool() {

    // Let all threads run into termination
    TERMINATE_THREADS = true;
    ABORT_SEARCH = true;
    startAllThreads();
    
    // Join all threads
    for (int t = 1; t < NUM_THREADS; t++) {
        threadPool[t]->t.join();
        // cout << "Joined T" << t << endl;
    }

    threadPool.clear();
    // cout << "Joined all threads" << endl;
}

bool resizeThreadPool(size_t numWorkers) {
    if (numWorkers < 1) {
        return false;
    }

    deleteThreadPool();
    NUM_THREADS = std::min(numWorkers, (size_t)MAX_THREADS);
    initThreadPool();
    return true;
}

void resetAllThreadStates(board_t* b, search_t* s) {
    for (int i = 0; i < NUM_THREADS; i++) {
        threadPool[i]->resetThreadStates(b, s);
    }
}

void startAllThreads() {
    for (int i = 1; i < NUM_THREADS; i++) {
        threadPool[i]->searching = true;
        threadPool[i]->startThread();
    }
}

void waitAllThreads() {
    for (int i = 1; i < NUM_THREADS; i++) {
        threadPool[i]->waitThread();
    }
}

int totalNodeCount() {
    int n = 0;
    for (auto t : threadPool) {
        n += t->nodes + t->qnodes;
    }
    return n;
}

Thread selectBestThread() { }

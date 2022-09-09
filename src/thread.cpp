#include "thread.h"



#ifdef _WIN32
    #include <windows.h>
#elif MACOS
    #include <sys/param.h>
    #include <sys/sysctl.h>
#else
    #include <unistd.h>
#endif

#ifdef _WIN32
SYSTEM_INFO systeminfo;
GetSystemInfo( &systeminfo );
unsigned logicalcpucount = systeminfo.dwNumberOfProcessors;
#else
unsigned logicalcpucount = sysconf( _SC_NPROCESSORS_ONLN );
#endif

unsigned int getMaxPhysicalCores() {
    uint32_t registers[4];
    unsigned int physicalcpucount;

    __asm__ __volatile__ ("cpuid " :
        "=a" (registers[0]),
        "=b" (registers[1]),
        "=c" (registers[2]),
        "=d" (registers[3])
        : "a" (1), "c" (0)
    );

    unsigned CPUFeatureSet = registers[3];
    bool hyperthreading = CPUFeatureSet & (1 << 28);
    if (hyperthreading){
        physicalcpucount = logicalcpucount / 2;
    } else {
        physicalcpucount = logicalcpucount;
    }

    return physicalcpucount;
}


// Makefile might set number of threads. If not, set default (1 thread).
#ifdef CUSTOM_THREADS
int NUM_THREADS = std::min(CUSTOM_THREADS, (int)std::thread::hardware_concurrency());
#else
int NUM_THREADS = 1; 
#endif // CUSTOM_THREADS

bool TERMINATE_THREADS = false;
bool ABORT_SEARCH = false;


std::vector<Thread> threadPool;

void ThreadWrapper::resetThreadStates(Board* board, Stats* search, Instructions* instructions) {
    b     = *board;
    s     = *search;
    instr = *instructions;


    // Reset heuristics: Matekiller, Killer, Counter, History
    std::fill_n(mateKiller, MAX_GAME_MOVES, MOVE_NONE);

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
        Threads::iid(threadPool[this->id]);
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


namespace Threads {

void initPool() {

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

void deletePool() {

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

bool resizePool(size_t numWorkers) {

    if (numWorkers < 1)
        return false;

    deletePool();
    NUM_THREADS = std::min(numWorkers, (size_t)getMaxPhysicalCores());
    initPool();

    return true;
}

void resetAllThreadStates(Board* board, Stats* search, Instructions* instructions) {
    for (int i = 0; i < NUM_THREADS; i++) {
        threadPool[i]->resetThreadStates(board, search, instructions);
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

int selectBestThreadIndex() {
    
    int bestIdx = 0;
    Value bestScore = threadPool[bestIdx]->bestScore;
    int bestDepth = threadPool[bestIdx]->depth;

    for (size_t i = 1; i < threadPool.size(); i++) {
        if (threadPool[i]->depth > bestDepth
            && threadPool[i]->bestScore > bestScore) {
            
            // Found a thread with at least equal 
            // depth but better score
            bestIdx = i;
            bestDepth = threadPool[i]->depth;
            bestScore = threadPool[i]->bestScore;
        }
    }

    return bestIdx;
}


}
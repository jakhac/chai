#include <iostream>

#include "testFixtures.h"


int main(int argc, char **argv) {

#if defined(USE_NNUE) && defined(CUSTOM_EVALFILE)
    cout << "Init custom evalfile" << endl;
    NNUE::initIncNet();
#endif
    
    ::testing::InitGoogleTest(&argc, argv);

#if defined(__linux__) 
    ::testing::FLAGS_gtest_color = "yes";
#endif

    // Disable std output to prevent spam in cmd-interface
    std::cout.rdbuf(nullptr);

    return RUN_ALL_TESTS();
}
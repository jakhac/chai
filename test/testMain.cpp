#include <iostream>

#include "testFixtures.h"


int main(int argc, char **argv) {

#if defined(USE_NNUE) && defined(CUSTOM_EVALFILE)
	cout << "Init custom evalfile" << endl;
	initIncNet();
#endif
    
    ::testing::InitGoogleTest(&argc, argv);
    // ::testing::GTEST_FLAG(filter) = "SearchTest*";
    // ::testing::FLAGS_gtest_color = "yes"; // TODO if linux

    // Disable std output to prevent spam in cmd-interface
	std::cout.rdbuf(nullptr);

    return RUN_ALL_TESTS();
}
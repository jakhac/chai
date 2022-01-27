#include <iostream>

#include "testFixtures.h"


int main(int argc, char **argv) {
    
    ::testing::InitGoogleTest(&argc, argv);
    // ::testing::GTEST_FLAG(filter) = "SearchTest*";
    ::testing::FLAGS_gtest_color = "yes"; // TODO if linux

    // Disable std output to prevent spam in cmd-interface
	std::cout.rdbuf(nullptr);

    return RUN_ALL_TESTS();
}
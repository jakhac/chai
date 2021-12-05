#include <iostream>
#include "gtest/gtest.h"

#include "testFixtures.h"

int main(int argc, char **argv) 
{
    ::testing::InitGoogleTest(&argc, argv);

	auto old_buffer = std::cout.rdbuf(nullptr);

    // ::testing::GTEST_FLAG(filter) = "EvalTest*";

    // 0 if passed, else 1
    return RUN_ALL_TESTS();
}
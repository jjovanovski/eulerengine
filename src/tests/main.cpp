#include <iostream>
#include "gtest/gtest.h"

TEST(Test1, Subtest1) {
	ASSERT_TRUE(1 == 1);
}

int main(int argc, char** argv)
{
	testing::InitGoogleTest();
	return RUN_ALL_TESTS();
}
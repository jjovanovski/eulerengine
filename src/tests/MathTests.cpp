#include "gtest/gtest.h"

#include "math/Math.h"
#include <iostream>

using namespace Euler::Math;

TEST(MathTests, Abs) {
	ASSERT_EQ(Abs(123.456f), 123.456f);
	ASSERT_EQ(Abs(-123.456f), 123.456f);
	ASSERT_EQ(Abs(0.0f), 0.0f);
	ASSERT_EQ(Abs(-0.0f), 0.0f);
}

TEST(MathTests, EpsilonNotZero) {
	ASSERT_NE(Epsilon(), 0.0f);
}

TEST(MathTests, Sin) {
	ASSERT_TRUE(AlmostEqual(Sin(0.0f), 0.0f));
	ASSERT_TRUE(AlmostEqual(Sin(PI / 2.0f), 1.0f));
	ASSERT_TRUE(AlmostEqual(Sin(PI), 0.0f));
	ASSERT_TRUE(AlmostEqual(Sin(3.0f * PI / 2.0f), -1.0f));
	ASSERT_TRUE(AlmostEqual(Sin(5.0f * PI / 2.0f), 1.0f));
}

TEST(MathTests, Cos) {
	ASSERT_TRUE(AlmostEqual(Cos(0.0f), 1.0f));
	ASSERT_TRUE(AlmostEqual(Cos(PI / 2.0f), 0.0f));
	ASSERT_TRUE(AlmostEqual(Cos(PI), -1.0f));
	ASSERT_TRUE(AlmostEqual(Cos(3.0f * PI / 2.0f), 0.0f));
	ASSERT_TRUE(AlmostEqual(Cos(5.0f * PI / 2.0f), -0.0f));
}
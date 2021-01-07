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

TEST(MathTests, DegToRad) {
	ASSERT_TRUE(AlmostEqual(Rad(0.0f), 0.0f));
	ASSERT_TRUE(AlmostEqual(Rad(45.0f), PI / 4.0f));
	ASSERT_TRUE(AlmostEqual(Rad(90.0f), PI / 2.0f));
	ASSERT_TRUE(AlmostEqual(Rad(180.0f), PI));
	ASSERT_TRUE(AlmostEqual(Rad(270.0f), 3.0f * PI / 2.0f));
	ASSERT_TRUE(AlmostEqual(Rad(360.0f), 2.0f * PI));
	ASSERT_TRUE(AlmostEqual(Rad(-45.0f), -PI / 4.0f));
}

TEST(MathTests, RadToDeg) {
	ASSERT_TRUE(AlmostEqual(Deg(0.0f), 0.0f));
	ASSERT_TRUE(AlmostEqual(Deg(PI / 4.0f), 45.0f));
	ASSERT_TRUE(AlmostEqual(Deg(PI / 2.0f), 90.0f));
	ASSERT_TRUE(AlmostEqual(Deg(PI), 180.0f));
	ASSERT_TRUE(AlmostEqual(Deg(3.0f * PI / 2.0f), 270.0f));
	ASSERT_TRUE(AlmostEqual(Deg(2.0f * PI), 360.0f));
	ASSERT_TRUE(AlmostEqual(Deg(-PI / 4.0f), -45.0f));
}

TEST(MathTests, Clamp) {
	ASSERT_EQ(Clamp(0.5f, 0.0f, 1.0f), 0.5f);
	ASSERT_EQ(Clamp(-0.5f, 0.0f, 1.0f), 0.0f);
	ASSERT_EQ(Clamp(1.5f, 0.0f, 1.0f), 1.0f);
	ASSERT_EQ(Clamp(0.0f, 0.0f, 1.0f), 0.0f);
	ASSERT_EQ(Clamp(1.0f, 0.0f, 1.0f), 1.0f);
	ASSERT_EQ(Clamp(-1.5f, -2.0f, -1.0f), -1.5f);
	ASSERT_EQ(Clamp(-2.5f, -2.0f, -1.0f), -2.0f);
	ASSERT_EQ(Clamp(-0.5f, -2.0f, -1.0f), -1.0f);
}
#include "gtest/gtest.h"

#include "math/Math.h"
#include "math/Matrices.h"
#include "math/Vec4.h"
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

TEST(MathTests, ClampInt) {
	ASSERT_EQ(Clamp(1, 0, 2), 1);
	ASSERT_EQ(Clamp(-1, 0, 2), 0);
	ASSERT_EQ(Clamp(3, 0, 2), 2);
	ASSERT_EQ(Clamp(0, 0, 2), 0);
	ASSERT_EQ(Clamp(1, 0, 1), 1);
	ASSERT_EQ(Clamp(-2, -3, -1), -2);
	ASSERT_EQ(Clamp(-4, -3, -1), -3);
	ASSERT_EQ(Clamp(-1, -3, -1), -1);
}

TEST(MathTests, MatrixVectorMultiplicationIdentity) {
	Mat4 m = Matrices::Identity();
	Vec4 v(1, 2, 3, 1);

	Vec4 r = m * v;

	ASSERT_EQ(r.x, v.x);
	ASSERT_EQ(r.y, v.y);
	ASSERT_EQ(r.z, v.z);
	ASSERT_EQ(r.w, v.w);
}

TEST(MathTests, MatrixVectorMultiplicationTranslation) {
	float tx = 1;
	float ty = -2;
	float tz = 3;
	Mat4 m = Matrices::Translate(tx, ty, tz);
	Vec4 v(4, 5, 6, 1);

	Vec4 r = m * v;

	ASSERT_EQ(r.x, v.x + tx);
	ASSERT_EQ(r.y, v.y + ty);
	ASSERT_EQ(r.z, v.z + tz);
	ASSERT_EQ(r.w, v.w + 0);
}

TEST(MathTests, MatrixVectorMultiplicationScale) {
	float sx = 1.0f;
	float sy = -2.0f;
	float sz = 3.0f;
	Mat4 m = Matrices::Scale(sx, sy, sz);
	Vec4 v(4, 5, 6, 1);

	Vec4 r = m * v;

	ASSERT_EQ(r.x, v.x * sx);
	ASSERT_EQ(r.y, v.y * sy);
	ASSERT_EQ(r.z, v.z * sz);
	ASSERT_EQ(r.w, v.w);
}

TEST(MathTests, MatrixVectorMultiplicationScaleUniform) {
	float s = 0.5f;
	Mat4 m = Matrices::Scale(s);
	Vec4 v(4, 5, 6, 1);

	Vec4 r = m * v;

	ASSERT_EQ(r.x, v.x * s);
	ASSERT_EQ(r.y, v.y * s);
	ASSERT_EQ(r.z, v.z * s);
	ASSERT_EQ(r.w, v.w);
}
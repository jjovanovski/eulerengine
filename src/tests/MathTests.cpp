#include "gtest/gtest.h"

#include "math/Math.h"
#include "math/Matrices.h"
#include "math/Vec4.h"
#include "math/Quaternion.h"
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

TEST(MathTests, MatrixVectorMultiplicationRotateZ) {
	Mat4 m = Matrices::RotateZ(Rad(90.0f));
	Vec4 v(1.0f, 0.0f, 0.0f, 1.0f);
	
	Vec4 r = m * v;

	ASSERT_TRUE(AlmostEqual(r.x, 0.0f));
	ASSERT_TRUE(AlmostEqual(r.y, 1.0f));
	ASSERT_TRUE(AlmostEqual(r.z, v.z));
	ASSERT_TRUE(AlmostEqual(r.w, v.w));
}

TEST(MathTests, MatrixVectorMultiplicationRotateY) {
	Mat4 m = Matrices::RotateY(Rad(90.0f));
	Vec4 v(0.0f, 0.0f, 1.0f, 1.0f);

	Vec4 r = m * v;

	ASSERT_TRUE(AlmostEqual(r.x, 1.0f));
	ASSERT_TRUE(AlmostEqual(r.y, 0.0f));
	ASSERT_TRUE(AlmostEqual(r.z, 0.0f));
	ASSERT_TRUE(AlmostEqual(r.w, v.w));
}

TEST(MathTests, MatrixVectorMultiplicationRotateX) {
	Mat4 m = Matrices::RotateX(Rad(90.0f));
	Vec4 v(0.0f, 1.0f, 0.0f, 1.0f);

	Vec4 r = m * v;

	ASSERT_TRUE(AlmostEqual(r.x, 0.0f));
	ASSERT_TRUE(AlmostEqual(r.y, 0.0f));
	ASSERT_TRUE(AlmostEqual(r.z, 1.0f));
	ASSERT_TRUE(AlmostEqual(r.w, v.w));
}

TEST(MathTests, QuaternionIdentity) {
	Euler::Quaternion q = Euler::Quaternion(1, 0, 0, 0);
	Vec4 v(1, 2, 3, 1);

	Vec4 r = q.GetMatrix() * v;

	ASSERT_EQ(v.x, r.x);
	ASSERT_EQ(v.y, r.y);
	ASSERT_EQ(v.z, r.z);
	ASSERT_EQ(v.w, r.w);
}

TEST(MathTests, QuaternionRotateX) {
	Euler::Quaternion q = Euler::Quaternion::Euler(Rad(90.0f), Vec3(1, 0, 0));
	Vec4 v(0, 1, 0, 1);

	Vec4 r = q.GetMatrix() * v;

	ASSERT_TRUE(AlmostEqual(r.x, 0.0f));
	ASSERT_TRUE(AlmostEqual(r.y, 0.0f));
	ASSERT_TRUE(AlmostEqual(r.z, 1.0f));
	ASSERT_TRUE(AlmostEqual(r.w, v.w));
}

TEST(MathTests, QuaternionRotateY) {
	Euler::Quaternion q = Euler::Quaternion::Euler(Rad(90.0f), Vec3(0, 1, 0));
	Vec4 v(0.0f, 0.0f, 1.0f, 1.0f);

	Vec4 r = q.GetMatrix() * v;

	ASSERT_TRUE(AlmostEqual(r.x, 1.0f));
	ASSERT_TRUE(AlmostEqual(r.y, 0.0f));
	ASSERT_TRUE(AlmostEqual(r.z, 0.0f));
	ASSERT_TRUE(AlmostEqual(r.w, v.w));
}

TEST(MathTests, QuaternionRotateZ) {
	Euler::Quaternion q = Euler::Quaternion::Euler(Rad(90.0f), Vec3(0, 0, 1));
	Vec4 v(1.0f, 0.0f, 0.0f, 1.0f);

	Vec4 r = q.GetMatrix() * v;

	ASSERT_TRUE(AlmostEqual(r.x, 0.0f));
	ASSERT_TRUE(AlmostEqual(r.y, 1.0f));
	ASSERT_TRUE(AlmostEqual(r.z, 0.0f));
	ASSERT_TRUE(AlmostEqual(r.w, v.w));
}

TEST(MathTests, PerspectiveTests) {
	Mat4 m = Matrices::Perspective(800, 800, 60.0f, 0.01f, 10.0f);
	Vec4 p1(1.0f, -1.0f, 0.5f, 1.0f);		// point inside frustam
	Vec4 p2(-1.0f, 1.0f, 1.0f, 1.0f);		// point inside frustam, after p1 (greater z than p1)
	Vec4 p3(-1.0f, 1.0f, 11.0f, 1.0f);		// point outside frustam (too far away, greater z)
	Vec4 p4(-1.0f, 1.0f, -1.0f, 1.0f);		// point outside frustam (behind camera, lower z)

	Vec4 r1 = m * p1;
	r1.x /= r1.w; r1.y /= r1.w; r1.z /= r1.w; r1.w /= r1.w;
	
	Vec4 r2 = m * p2;
	r2.x /= r2.w; r2.y /= r2.w; r2.z /= r2.w; r2.w /= r2.w;
	
	Vec4 r3 = m * p3;
	r3.x /= r3.w; r3.y /= r3.w; r3.z /= r3.w; r3.w /= r3.w;

	Vec4 r4 = m * p4;
	r3.x /= r3.w; r3.y /= r3.w; r3.z /= r3.w; r3.w /= r3.w;

	ASSERT_GT(r1.x, 0.0f);
	ASSERT_LT(r1.y, 0.0f);
	ASSERT_GE(r1.z, 0.0f);
	ASSERT_LE(r1.z, 1.0f);

	ASSERT_LT(r2.x, 0.0f);
	ASSERT_GT(r2.y, 0.0f);
	ASSERT_GE(r2.z, 0.0f);
	ASSERT_LE(r2.z, 1.0f);

	ASSERT_LT(r1.z, r2.z);

	ASSERT_GT(r3.z, 1.0f);
	ASSERT_LT(r4.z, 0.0f);
}
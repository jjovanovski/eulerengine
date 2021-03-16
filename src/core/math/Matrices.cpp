#include "Matrices.h"

#include <math.h>

#define PI 3.14159265359f
#define Rad2Deg 180.0f/PI
#define Deg2Rad PI/180.0f

using namespace Euler::Math;

Mat4 Matrices::Identity()
{
	Mat4 mat;
	mat.Set(0, 0, 1);
	mat.Set(1, 1, 1);
	mat.Set(2, 2, 1);
	mat.Set(3, 3, 1);

	return mat;
}


Mat4 Matrices::Translate(float x, float y, float z)
{
	Mat4 mat;
	mat.Set(0, 3, x);
	mat.Set(1, 3, y);
	mat.Set(2, 3, z);

	mat.Set(0, 0, 1);
	mat.Set(1, 1, 1);
	mat.Set(2, 2, 1);
	mat.Set(3, 3, 1);

	return mat;
}

Mat4 Matrices::Scale(float s)
{
	Mat4 mat;
	mat.Set(0, 0, s);
	mat.Set(1, 1, s);
	mat.Set(2, 2, s);
	mat.Set(3, 3, 1);

	return mat;
}

Mat4 Matrices::Scale(float x, float y, float z)
{
	Mat4 mat;
	mat.Set(0, 0, x);
	mat.Set(1, 1, y);
	mat.Set(2, 2, z);
	mat.Set(3, 3, 1);

	return mat;
}

Mat4 Matrices::RotateX(float rad)
{
	Mat4 mat;
	mat.Set(0, 0, 1);
	mat.Set(3, 3, 1);

	mat.Set(1, 1, cosf(rad));
	mat.Set(1, 2, -sinf(rad));

	mat.Set(2, 1, sinf(rad));
	mat.Set(2, 2, cosf(rad));

	return mat;
}

Mat4 Matrices::RotateY(float rad)
{
	Mat4 mat;
	mat.Set(0, 0, cosf(rad));
	mat.Set(0, 2, sinf(rad));

	mat.Set(1, 1, 1);
	mat.Set(3, 3, 1);

	mat.Set(2, 0, -sinf(rad));
	mat.Set(2, 2, cosf(rad));

	return mat;
}

Mat4 Matrices::RotateZ(float rad)
{
	Mat4 mat;
	mat.Set(0, 0, cosf(rad));
	mat.Set(0, 1, -sinf(rad));

	mat.Set(1, 0, sinf(rad));
	mat.Set(1, 1, cosf(rad));

	mat.Set(2, 2, 1);
	mat.Set(3, 3, 1);

	return mat;
}

Mat4 Matrices::Perspective(uint32_t width, uint32_t height, float fieldOfView, float nearZ, float farZ)
{
	float aspectRatio = (float)width / height;
	float halfAngleFOV = (fieldOfView * Deg2Rad) / 2.0f;
	float tanHalfAngleFOV = tanf(halfAngleFOV);
	
	Mat4 mat;
	mat.Set(0, 0, 1.0f / (aspectRatio * tanHalfAngleFOV));
	mat.Set(0, 1, 0);
	mat.Set(0, 2, 0);
	mat.Set(0, 3, 0);

	mat.Set(1, 0, 0);
	mat.Set(1, 1, 1.0f / tanHalfAngleFOV);
	mat.Set(1, 2, 0);
	mat.Set(1, 3, 0);

	mat.Set(2, 0, 0);
	mat.Set(2, 1, 0);
	mat.Set(2, 2, -farZ / (nearZ - farZ));
	mat.Set(2, 3, (nearZ * farZ) / (nearZ - farZ));

	mat.Set(3, 0, 0);
	mat.Set(3, 1, 0);
	mat.Set(3, 2, 1);
	mat.Set(3, 3, 0);

	return mat;
}

Mat4 Matrices::Orthographic(uint32_t width, uint32_t height, float size)
{
	float aspectRatio = (float)width / height;
	float top = size;
	float bottom = -size;
	float left = -size * aspectRatio;
	float right = size * aspectRatio;
	float far = 50.0f;
	float near = -50.0f;

	Mat4 mat;

	mat.Set(0, 0, 2.0f / (right - left));
	mat.Set(1, 1, 2.0f / (top - bottom));
	mat.Set(2, 2, 1.0f / (far - near));
	mat.Set(3, 3, 1.0f);

	mat.Set(0, 3, -(2.0f * left) / (right - left) - 1.0f);
	mat.Set(1, 3, -(2.0f * bottom) / (top - bottom) - 1.0f);
	mat.Set(2, 3, -near / (far - near));

	return mat;
}
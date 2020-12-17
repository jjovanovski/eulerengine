#include "Matrices.h"

#include <math.h>

#define PI 3.14159265359f
#define Rad2Deg 180.0f/PI
#define Deg2Rad PI/180.0f

using namespace Euler::Math;

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
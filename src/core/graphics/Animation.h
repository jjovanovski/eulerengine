#pragma once

#include "../API.h"
#include "../math/Vec3.h"
#include "../math/Quaternion.h"

#include <vector>

#define MAX_BONES 32

namespace Euler
{
	struct EULER_API BoneTransform
	{
		Vec3 Position;
		Quaternion Rotation;
	};

	class EULER_API KeyFrame
	{
	public:
		BoneTransform BoneTransforms[MAX_BONES];
		int BoneTransformCount;
		float Timestamp;

		KeyFrame();
		~KeyFrame();
	};

	class EULER_API Animation
	{
	public:
		KeyFrame* KeyFrames;
		int KeyFrameCount;
		float Duration;
		
		Animation(int keyFrameCount);
		~Animation();
	};
}
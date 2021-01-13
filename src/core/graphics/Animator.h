#pragma once

#include "../API.h"

#include "Animation.h"
#include "BufferGroup.h"

#include <chrono>

namespace Euler
{
	class EULER_API Animator
	{
	public:
		float Time;
		Animation* Animation;
		std::chrono::steady_clock::time_point TimeAtStart;
		int CurrentFrameIndex;
		std::vector<Mat4> BoneMatrices;

		void Start();
		void Update();
	};
}
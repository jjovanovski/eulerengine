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
		std::vector<int> BoneParents;
		std::vector<Mat4> BoneOffsetMatrices;

		bool Running = false;

		void Start();
		void Pause();
		void Resume();
		void Update();

		void CalculateMatrix(int index, std::vector<bool> visited, KeyFrame* prevFrame, KeyFrame* currentFrame, float t);
	};
}
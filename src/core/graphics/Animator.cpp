#include "Animator.h"

#include "../math/Matrices.h"
#include "../math/Math.h"

using namespace Euler;

void Animator::Start()
{
	TimeAtStart = std::chrono::steady_clock::now();
	CurrentFrameIndex = 1;
	Running = true;
}

void Animator::Pause()
{
	Running = false;
}

void Animator::Resume()
{
	Running = true;
}

void Animator::Update()
{
	if (!Running)
		return;

	// update time
	auto now = std::chrono::steady_clock::now();
	auto timeDiff = now - this->TimeAtStart;
	auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(timeDiff).count();
	Time = millis / 1000.0f;

	// calculate normalized time between last two frames
	if (CurrentFrameIndex == 0)
	{
		CurrentFrameIndex = 1;
	}

	KeyFrame currentFrame = Animation->KeyFrames[CurrentFrameIndex];
	KeyFrame prevFrame = Animation->KeyFrames[CurrentFrameIndex - 1];
	float t = Math::Clamp((Time - prevFrame.Timestamp) / (currentFrame.Timestamp - prevFrame.Timestamp), 0.0f, 1.0f);

	// if time is after last frame timestamp, find next frame
	if (t >= 1.0f)
	{
		CurrentFrameIndex = CurrentFrameIndex + 1;
		if (CurrentFrameIndex >= Animation->KeyFrameCount)
		{
			Start();
		}
	}

	// interpolate between two last frames
	BoneMatrices.clear();
	BoneMatrices.resize(MAX_BONES);
	std::vector<bool> visited(MAX_BONES, false);
	for (int i = 0; i < currentFrame.BoneTransformCount; i++)
	{
		if(!visited[i])
			CalculateMatrix(i, visited, &prevFrame, &currentFrame, t);
	}

	for (int i = 0; i < BoneMatrices.size(); i++)
	{
		BoneMatrices[i] = BoneMatrices[i].Multiply(BoneOffsetMatrices[i]);
		BoneMatrices[i].Transpose();
	}

	// write interpolated transform to buffers
}

void Animator::CalculateMatrix(int index, std::vector<bool> visited, KeyFrame* prevFrame, KeyFrame* currentFrame, float t)
{
	Vec3 pos = Vec3::Lerp(prevFrame->BoneTransforms[index].Position, currentFrame->BoneTransforms[index].Position, t);

	Mat4 translate = Math::Matrices::Translate(pos.x, pos.y, pos.z);
	//Mat4 translate = Math::Matrices::Translate(currentFrame->BoneTransforms[index].Position.x, currentFrame->BoneTransforms[index].Position.y, currentFrame->BoneTransforms[index].Position.z);
	Quaternion q1 = prevFrame->BoneTransforms[index].Rotation;
	Quaternion q2 = currentFrame->BoneTransforms[index].Rotation;
	Quaternion q = Quaternion::Slerp(q1, q2, t);
	Mat4 rotate = q.GetMatrix();
	Mat4 scale = Math::Matrices::Scale(1.0f);
	Mat4 m = translate.Multiply(rotate);

	if (BoneParents[index] >= 0)
	{
		int parentIndex = BoneParents[index];
		if (!visited[parentIndex])
		{
			CalculateMatrix(parentIndex, visited, prevFrame, currentFrame, t);
		}

		m = BoneMatrices[parentIndex].Multiply(m);
	}

	BoneMatrices[index] = m;
	visited[index] = true;
}
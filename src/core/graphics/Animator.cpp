#include "Animator.h"

#include "../math/Matrices.h"
#include "../math/Math.h"

using namespace Euler;

void Animator::Start()
{
	TimeAtStart = std::chrono::steady_clock::now();
	CurrentFrameIndex = 1;
}

void Animator::Update()
{
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
	for (int i = 0; i < currentFrame.BoneTransformCount; i++)
	{
		Vec3 pos = Vec3::Lerp(prevFrame.BoneTransforms[i].Position, currentFrame.BoneTransforms[i].Position, t);

		Mat4 translate = Math::Matrices::Translate(pos.x, pos.y, pos.z);
		Quaternion q1 = prevFrame.BoneTransforms[i].Rotation;
		Quaternion q2 = currentFrame.BoneTransforms[i].Rotation;
		Quaternion q = Quaternion::Slerp(q1, q2, t);
		Mat4 rotate = q.GetMatrix();
		Mat4 m = translate.Multiply(rotate);
		m.Transpose();

		BoneMatrices.push_back(m);

	}

	// write interpolated transform to buffers
}
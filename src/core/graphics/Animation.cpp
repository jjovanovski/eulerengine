#include "Animation.h"

using namespace Euler;

Animation::Animation(int keyFrameCount)
{
	KeyFrameCount = keyFrameCount;
	KeyFrames = new KeyFrame[KeyFrameCount];
	Duration = 0;
}

Animation::~Animation()
{
	delete[] KeyFrames;
}

KeyFrame::KeyFrame()
{
	BoneTransformCount = MAX_BONES;
	Timestamp = 0;
}

KeyFrame::~KeyFrame()
{
}
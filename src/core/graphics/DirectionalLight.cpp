#include "DirectionalLight.h"

using namespace Euler::Graphics;

DirectionalLight::DirectionalLight()
{
}

DirectionalLight::DirectionalLight(Vec3 direction, Vec3 color, float intensity)
{
	Direction = direction;
	Color = color;
	Intensity = intensity;
}
#include "AmbientLight.h"

using namespace Euler::Graphics;

AmbientLight::AmbientLight()
{

}

AmbientLight::AmbientLight(Vec3 cameraPosition, Vec3 color, float intensity)
{
	CameraPosition = cameraPosition;
	Color = color;
	Intensity = intensity;
}
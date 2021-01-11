#include "TextureResource.h"

#include "stb_image.h"

using namespace Euler;

void TextureResource::Load(const char* filePath, TextureChannels textureChannels)
{
	int width, height, channels;
	_data = stbi_load(filePath, &width, &height, &channels, TextureChannelsToStbDesiredChannels(textureChannels));

	// TODO: Check for errors

	_width = width;
	_height = height;
	_channels = channels;
}

void TextureResource::Unload()
{
	stbi_image_free(_data);
	_width = 0;
	_height = 0;
	_channels = 0;
}

uint32_t TextureResource::GetWidth()
{
	return _width;
}

uint32_t TextureResource::GetHeight()
{
	return _height;
}

uint32_t TextureResource::GetChannels()
{
	return _channels;
}

unsigned char* TextureResource::GetData()
{
	return _data;
}

int TextureResource::TextureChannelsToStbDesiredChannels(TextureChannels textureChannels)
{
	switch (textureChannels)
	{
	case Euler::TEXTURE_CHANNELS_GRAY:
		return STBI_grey;

	case Euler::TEXTURE_CHANNELS_GRAYA:
		return STBI_grey_alpha;

	case Euler::TEXTURE_CHANNELS_RGB:
		return STBI_rgb;

	default:
	case Euler::TEXTURE_CHANNELS_RGBA:
		return STBI_rgb_alpha;
	}
}
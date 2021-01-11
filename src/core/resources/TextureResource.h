#pragma once

#include "../API.h"

#include <stdint.h>

namespace Euler
{
	enum TextureChannels
	{
		TEXTURE_CHANNELS_GRAY,
		TEXTURE_CHANNELS_GRAYA,
		TEXTURE_CHANNELS_RGB,
		TEXTURE_CHANNELS_RGBA
	};

	class EULER_API TextureResource
	{
	private:
		uint32_t _width = 0;
		uint32_t _height = 0;
		uint32_t _channels = 0;
		unsigned char* _data = nullptr;

	public:
		void Load(const char* filePath, TextureChannels textureChannels);
		void Unload();

		uint32_t GetWidth();
		uint32_t GetHeight();
		uint32_t GetChannels();
		unsigned char* GetData();

	private:
		int TextureChannelsToStbDesiredChannels(TextureChannels textureChannels);
	};
}
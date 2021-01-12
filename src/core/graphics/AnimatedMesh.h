#pragma once

#include "../API.h"
#include "AnimatedVertex.h"
#include "Texture.h"
#include "vulkan/Vulkan.h"

#include <vector>

namespace Euler
{
	class EULER_API AnimatedMesh
	{
	public:
		std::vector<AnimatedVertex> Vertices;
		std::vector<uint32_t> Indices;
		// TODO: Material
		Graphics::Texture* Texture;

		// vulkan specific
		Graphics::Buffer VertexBuffer;
		Graphics::Buffer IndexBuffer;

		AnimatedMesh();

		void Create(Graphics::Vulkan* vulkan);
		void Destroy(Graphics::Vulkan* vulkan);
		void RecordDrawCommands(Graphics::Vulkan* vulkan, VkCommandBuffer commandBuffer);
	};
};
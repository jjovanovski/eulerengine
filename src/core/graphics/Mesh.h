#pragma once

#include "../API.h"
#include "Vertex.h"
#include "Texture.h"
#include "vulkan/Vulkan.h"

#include <vector>

namespace Euler
{
	class EULER_API Mesh
	{
	public:
		std::vector<Vertex> Vertices;
		std::vector<uint32_t> Indices;
		// TODO: Material
		Graphics::Texture* Texture;

		// vulkan specific
		Graphics::Buffer VertexBuffer;
		Graphics::Buffer IndexBuffer;

		Mesh();

		void Create(Graphics::Vulkan* vulkan);
		void Destroy(Graphics::Vulkan* vulkan);
		void RecordDrawCommands(Graphics::Vulkan* vulkan, VkCommandBuffer commandBuffer);
	};
};
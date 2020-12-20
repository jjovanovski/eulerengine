#include "Mesh.h"

using namespace Euler;

Mesh::Mesh()
{

}

void Mesh::Create(Graphics::Vulkan* vulkan)
{
	vulkan->CreateVertexBuffer(sizeof(Vertices[0]), Vertices.size(), Vertices.data(), &VertexBuffer);
	vulkan->CreateIndexBuffer(sizeof(Indices[0]), Indices.size(), Indices.data(), &IndexBuffer);
}

void Mesh::Destroy(Graphics::Vulkan* vulkan)
{
	vulkan->DestroyBuffer(VertexBuffer.Buffer, VertexBuffer.Memory);
	vulkan->DestroyBuffer(IndexBuffer.Buffer, IndexBuffer.Memory);
}

void Mesh::RecordDrawCommands(Graphics::Vulkan* vulkan, VkCommandBuffer commandBuffer)
{
	vulkan->DrawMesh(commandBuffer, &VertexBuffer, &IndexBuffer, Indices.size());
}


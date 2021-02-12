#include "Shadows.h"

#include "../io/Utils.h"

using namespace Euler::Graphics;

void Shadows::Create(Vulkan* vulkan, ModelPipeline* modelPipeline, float viewportWidth, float viewportHeight)
{
	_vulkan = vulkan;
	_modelPipeline = modelPipeline;

	/* === READ SHADER CODE === */

	std::vector<char> vertexShaderCode = ReadFile("shaders/out/shadow_vertex.spv");
	std::vector<char> fragmentShaderCode = ReadFile("shaders/out/shadow_fragment.spv");

	/* === CREATE PIPELINE === */

	PipelineInfo pipelineInfo{};

	pipelineInfo.VertexShaderCode = vertexShaderCode.data();
	pipelineInfo.VertexShaderCodeSize = vertexShaderCode.size();

	pipelineInfo.FragmentShaderCode = fragmentShaderCode.data();
	pipelineInfo.FragmentShaderCodeSize = fragmentShaderCode.size();

	pipelineInfo.VertexStride = sizeof(Vertex);
	pipelineInfo.VertexAttributes = modelPipeline->GetVertexAttributes();

	pipelineInfo.Topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	pipelineInfo.DepthTestEnabled = true;

	pipelineInfo.ViewportWidth = viewportWidth;
	pipelineInfo.ViewportHeight = viewportHeight;

	_vulkan->CreateShadowRenderPass(&_shadowRenderPass);
	pipelineInfo.RenderPass = _shadowRenderPass;

	_vulkan->CreatePipeline(&pipelineInfo, &_pipelineLayout, &_pipeline);
}

void Shadows::Destroy()
{
	_vulkan->DestroyPipeline(_pipelineLayout, _pipeline);
	vkDestroyRenderPass(_vulkan->_device, _shadowRenderPass, nullptr);
}

void Shadows::RecordCommands()
{
	vkCmdBindPipeline(*_vulkan->GetMainCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);

	for (int i = 0; i < _modelPipeline->Models.size(); i++)
	{
		Model* model = _modelPipeline->Models[i];

		// set model matrix
		uint32_t offset = _modelPipeline->_modelMatrixAlignment * i;
		vkCmdBindDescriptorSets(
			*_vulkan->GetMainCommandBuffer(),
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			_pipelineLayout,
			1,
			1,
			&_modelPipeline->_modelDescriptorSetGroup.DescriptorSets[_vulkan->_currentImage],
			1,
			&offset
		);

		// draw model meshes
		for (int j = 0; j < model->Drawables.size(); j++)
		{
			vkCmdBindDescriptorSets(
				*_vulkan->GetMainCommandBuffer(),
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				_pipelineLayout,
				2,
				1,
				&model->Drawables[j]->Texture->DescriptorSetGroup.DescriptorSets[_vulkan->_currentImage],
				0,
				nullptr
			);

			_vulkan->DrawMesh(
				*_vulkan->GetMainCommandBuffer(),
				&model->Drawables[j]->Mesh->VertexBuffer,
				&model->Drawables[j]->Mesh->IndexBuffer,
				model->Drawables[j]->Mesh->Indices.size()
			);
		}
	}
}
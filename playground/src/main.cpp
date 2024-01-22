#include <cppVulkanAPI.hpp>

#include "logger.hpp"
#include "game_object.hpp"

struct ModelMatrix_push_constant
{
	glm::mat4 model;
};

int main(void)
{
	logger.setTimestamp(false);
	try
	{
		ft::RenderAPI renderAPI;
		FT_INFO("RenderAPI initialized");


		// load model
		GameObject obj1;
		obj1.transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
		obj1.meshID = renderAPI.loadModel("playground/models/viking_room.obj");

		std::vector<GameObject> gameObjects;
		gameObjects.push_back(obj1);

		// create descriptor set
		VkDescriptorSetLayoutBinding projViewLayoutBinding{};
		projViewLayoutBinding.binding = 0;
		projViewLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		projViewLayoutBinding.descriptorCount = 1;
		projViewLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		ft::Descriptor::ID projViewDescriptorID = renderAPI.createDescriptor(projViewLayoutBinding);

		VkDescriptorSetLayoutBinding textureLayoutBinding{};
		textureLayoutBinding.binding = 0;
		textureLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		textureLayoutBinding.descriptorCount = 1;
		textureLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		ft::Descriptor::ID textureDescriptorID = renderAPI.createDescriptor(textureLayoutBinding);

		// create texture
		ft::Texture::CreateInfo textureInfo{};
		textureInfo.filepath = "playground/textures/viking_room.ppm";
		textureInfo.mipLevel = 1;
		textureInfo.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		ft::Texture::ID textureID = renderAPI.loadTexture(textureInfo);

		// create uniform buffer
		ft::UniformBuffer::CreateInfo uniformBufferInfo{};
		uniformBufferInfo.size = sizeof(ViewProj_UBO);
		uniformBufferInfo.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		ft::UniformBuffer::ID uniformBufferID = renderAPI.createUniformBuffer(uniformBufferInfo);


		// create pipeline
		ft::Pipeline::CreateInfo pipelineCreateInfo{};
		pipelineCreateInfo.vertexShaderPath = "playground/shaders/simple_shader.vert.spv";
		pipelineCreateInfo.fragmentShaderPath = "playground/shaders/simple_shader.frag.spv";
		pipelineCreateInfo.descriptorSetLayouts = {
			renderAPI.m_uniformBufferMap[uniformBufferID]->descriptor()->layout(),
			renderAPI.m_textureMap[textureID]->descriptor()->layout()
		};
		pipelineCreateInfo.pushConstantRanges = {
			{VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ModelMatrix_push_constant)}
		};

		ft::Pipeline::ID simpleShaderPipelineID = renderAPI.createPipeline(pipelineCreateInfo);

		// main loop
		while (renderAPI.m_device->window->shouldClose() == false)
		{
			renderAPI.m_device->windowManager->pollEvents();

			renderAPI.startDraw();
			renderAPI.startRendering();

			//############################################################################

			VkCommandBuffer cmd = renderAPI.m_vkCommandBuffers[renderAPI.m_currentFrame];

			int width, height;
			glfwGetFramebufferSize(renderAPI.m_device->window->getGLFWwindow(), &width, &height);

			ViewProj_UBO ubo{};
			ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			ubo.proj = glm::perspective(glm::radians(45.0f), width / (float) height, 0.1f, 10.0f);

			ubo.proj[1][1] *= -1;

			renderAPI.m_uniformBufferMap[uniformBufferID]->buffer(renderAPI.m_currentFrame)->write(&ubo, sizeof(ubo));

			renderAPI.bindPipeline(simpleShaderPipelineID);

			vkCmdBindDescriptorSets(
				cmd,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				renderAPI.m_pipelineMap[simpleShaderPipelineID]->layout->getVk(),
				0, 1,
				renderAPI.m_uniformBufferMap[uniformBufferID]->descriptor()->pSet(renderAPI.m_currentFrame),
				0, nullptr
			);

			vkCmdBindDescriptorSets(
				cmd,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				renderAPI.m_pipelineMap[simpleShaderPipelineID]->layout->getVk(),
				1, 1,
				renderAPI.m_textureMap[textureID]->descriptor()->pSet(renderAPI.m_currentFrame),
				0, nullptr
			);

			VkViewport viewport{};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = static_cast<float>(renderAPI.m_colorImage->width());
			viewport.height = static_cast<float>(renderAPI.m_colorImage->height());
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			vkCmdSetViewport(cmd, 0, 1, &viewport);

			VkRect2D scissor{};
			scissor.offset = {0, 0};
			scissor.extent = renderAPI.m_swapchain->swapchain->getExtent();
			vkCmdSetScissor(cmd, 0, 1, &scissor);

			for (auto& gameObject : gameObjects)
			{
				ModelMatrix_push_constant pushConstant{};
				pushConstant.model = gameObject.transform.modelMatrix();
				vkCmdPushConstants(
					cmd,
					renderAPI.m_pipelineMap[simpleShaderPipelineID]->layout->getVk(),
					VK_SHADER_STAGE_VERTEX_BIT,
					0,
					sizeof(ModelMatrix_push_constant),
					&pushConstant
				);

				renderAPI.bindMesh(gameObject.meshID);
			}

			//############################################################################

			renderAPI.endRendering();
			renderAPI.endDraw();
		}
	}
	catch(const std::exception& e)
	{
		FT_CRITICAL(e.what());
	}
	
	return 0;
}

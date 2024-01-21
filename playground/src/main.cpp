#include <cppVulkanAPI.hpp>

#include "logger.hpp"
#include "game_object.hpp"

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

		// create pipeline
		ft::Pipeline::CreateInfo pipelineCreateInfo{};
		pipelineCreateInfo.vertexShaderPath = "playground/shaders/simple_shader.vert.spv";
		pipelineCreateInfo.fragmentShaderPath = "playground/shaders/simple_shader.frag.spv";
		pipelineCreateInfo.descriptorSetLayouts = {
			renderAPI.m_uniformDescriptor->layout,
			renderAPI.m_imageDescriptor->layout
		};
		pipelineCreateInfo.pushConstantRanges = {
			{VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ModelMatrix_push_constant)}
		};

		ft::Pipeline::ID pipelineID = renderAPI.createPipeline(pipelineCreateInfo);

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

			renderAPI.m_uniformBuffers[renderAPI.m_currentFrame]->write(&ubo, sizeof(ubo));

			// vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, renderAPI.m_graphicPipeline->pipeline->getVk());
			renderAPI.bindPipeline(pipelineID);

			vkCmdBindDescriptorSets(
				cmd,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				renderAPI.m_pipelineMap[pipelineID]->layout->getVk(),
				0, 1,
				&renderAPI.m_uniformDescriptor->sets[renderAPI.m_currentFrame],
				0, nullptr
			);

			vkCmdBindDescriptorSets(
				cmd,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				renderAPI.m_pipelineMap[pipelineID]->layout->getVk(),
				1, 1,
				&renderAPI.m_imageDescriptor->sets[renderAPI.m_currentFrame],
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
					renderAPI.m_pipelineMap[pipelineID]->layout->getVk(),
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

/*

Validation Error: [ VUID-VkGraphicsPipelineCreateInfo-layout-07988 ]
Object 0: handle = 0xcb1c7c000000001b, type = VK_OBJECT_TYPE_SHADER_MODULE;
Object 1: handle = 0x2cfba2000000001c, type = VK_OBJECT_TYPE_PIPELINE_LAYOUT;

| MessageID = 0x215f02cd | vkCreateGraphicsPipelines():
pCreateInfos[0].pStages[1] SPIR-V (VK_SHADER_STAGE_FRAGMENT_BIT) uses descriptor slot [Set 0 Binding 1] (type `VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER`)
but was not declared in the pipeline layout.

The Vulkan spec states: If a resource variables is declared in a shader, a descriptor slot in layout must match the shader stage
(https://vulkan.lunarg.com/doc/view/1.3.275.0/linux/1.3-extensions/vkspec.html#VUID-VkGraphicsPipelineCreateInfo-layout-07988)

*/
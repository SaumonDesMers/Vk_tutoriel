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
		// parseSpriv("playground/shaders/simple_shader.vert.spv", "playground/shaders/simple_shader.frag.spv");
		// return 0;


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
			renderAPI.getUniformBuffer(uniformBufferID)->descriptor()->layout(),
			renderAPI.getTexture(textureID)->descriptor()->layout()
		};
		pipelineCreateInfo.pushConstantRanges = {
			{VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ModelMatrix_push_constant)}
		};

		ft::Pipeline::ID simpleShaderPipelineID = renderAPI.createPipeline(pipelineCreateInfo);

		// main loop
		while (!glfwWindowShouldClose(renderAPI.getWindow()))
		{
			glfwPollEvents();

			renderAPI.startDraw();
			renderAPI.startRendering();

			//############################################################################

			int width, height;
			glfwGetFramebufferSize(renderAPI.getWindow(), &width, &height);

			ViewProj_UBO ubo{};
			ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			ubo.proj = glm::perspective(glm::radians(45.0f), width / (float) height, 0.1f, 10.0f);
			ubo.proj[1][1] *= -1;

			renderAPI.getUniformBuffer(uniformBufferID)->buffer(renderAPI.currentFrame())->write(&ubo, sizeof(ubo));

			renderAPI.bindPipeline(simpleShaderPipelineID);

			renderAPI.bindDescriptor(
				simpleShaderPipelineID,
				0, 1,
				renderAPI.getUniformBuffer(uniformBufferID)->descriptor()->pSet(renderAPI.currentFrame())
			);

			renderAPI.bindDescriptor(
				simpleShaderPipelineID,
				1, 1,
				renderAPI.getTexture(textureID)->descriptor()->pSet(renderAPI.currentFrame())
			);

			VkViewport viewport{};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = static_cast<float>(width);
			viewport.height = static_cast<float>(height);
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			renderAPI.setViewport(viewport);

			VkRect2D scissor{};
			scissor.offset = { 0, 0 };
			scissor.extent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
			renderAPI.setScissor(scissor);

			for (auto& gameObject : gameObjects)
			{
				ModelMatrix_push_constant pushConstant{};
				pushConstant.model = gameObject.transform.modelMatrix();
				renderAPI.pushConstant(
					simpleShaderPipelineID,
					VK_SHADER_STAGE_VERTEX_BIT,
					sizeof(ModelMatrix_push_constant),
					&pushConstant
				);

				renderAPI.drawMesh(gameObject.meshID);
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

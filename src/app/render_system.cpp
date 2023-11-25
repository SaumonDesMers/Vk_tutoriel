#include "app/render_system.hpp"

#include "ft_glm/ft_glm.hpp"

#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>

namespace ft
{

	struct PushConstantData
	{
		glm::mat4 transform{1.0f};
		alignas(16) glm::mat4 normalMatrix;
	};

	RenderSystem::RenderSystem(Device &device, VkRenderPass renderPass)
		: m_device(device)
	{
		createPipelineLayout();
		createPipeline(renderPass);
	}

	RenderSystem::~RenderSystem()
	{
		vkDestroyPipelineLayout(m_device.device(), m_pipelineLayout, nullptr);
	}

	void RenderSystem::createPipelineLayout()
	{

		VkPushConstantRange pushConstantRange
		{
			.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			.offset = 0,
			.size = sizeof(PushConstantData)
		};

		VkPipelineLayoutCreateInfo pipelineLayoutInfo
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.setLayoutCount = 0,
			.pSetLayouts = nullptr,
			.pushConstantRangeCount = 1,
			.pPushConstantRanges = &pushConstantRange
		};

		if (vkCreatePipelineLayout(m_device.device(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create pipeline layout");
		}
	}

	void RenderSystem::createPipeline(VkRenderPass renderPass)
	{
		assert(m_pipelineLayout != VK_NULL_HANDLE && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		Pipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = m_pipelineLayout;
		m_pipeline = std::make_unique<Pipeline>(
			m_device,
			"shaders/vert.spv",
			"shaders/frag.spv",
			pipelineConfig
		);
	}

	void RenderSystem::rendergameObjects(
		FrameInfo &frameInfo,
		std::vector<GameObject> &gameObjects
	) {
		m_pipeline->bind(frameInfo.commandBuffer);

		auto projectionView = frameInfo.camera.getProjection() * frameInfo.camera.getView();

		for (auto &gameObject : gameObjects)
		{
			PushConstantData push{};
			auto modelMatrix = gameObject.transform.mat4();
			push.transform = projectionView * modelMatrix;
			push.normalMatrix = gameObject.transform.normalMatrix();

			vkCmdPushConstants(
				frameInfo.commandBuffer,
				m_pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(PushConstantData),
				&push
			);

			gameObject.model->bind(frameInfo.commandBuffer);
			gameObject.model->draw(frameInfo.commandBuffer);
		}
	}

}
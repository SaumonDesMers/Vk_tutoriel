#include "point_light_system.hpp"

#include "ft_glm/ft_glm.hpp"

#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>

namespace ft
{

	struct LightPointPushConstants
	{
		glm::vec4 position;
		glm::vec4 color;
		float radius;
	};

	PointLightSystem::PointLightSystem(Device &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout)
		: m_device(device)
	{
		createPipelineLayout(globalSetLayout);
		createPipeline(renderPass);
	}

	PointLightSystem::~PointLightSystem()
	{
		vkDestroyPipelineLayout(m_device.device(), m_pipelineLayout, nullptr);
	}

	void PointLightSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout)
	{

		VkPushConstantRange pushConstantRange
		{
			.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			.offset = 0,
			.size = sizeof(LightPointPushConstants)
		};

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

		VkPipelineLayoutCreateInfo pipelineLayoutInfo
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size()),
			.pSetLayouts = descriptorSetLayouts.data(),
			.pushConstantRangeCount = 1,
			.pPushConstantRanges = &pushConstantRange
		};

		if (vkCreatePipelineLayout(m_device.device(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create pipeline layout");
		}
	}

	void PointLightSystem::createPipeline(VkRenderPass renderPass)
	{
		assert(m_pipelineLayout != VK_NULL_HANDLE && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		Pipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.bindingDescriptions.clear();
		pipelineConfig.attributeDescriptions.clear();
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = m_pipelineLayout;
		m_pipeline = std::make_unique<Pipeline>(
			m_device,
			"shaders/point_light.vert.spv",
			"shaders/point_light.frag.spv",
			pipelineConfig
		);
	}

	void PointLightSystem::update(FrameInfo &frameInfo, GlobalUBO &ubo)
	{
		auto rotateLight = glm::rotate(
			glm::mat4(1.0f),
			frameInfo.frameTime,
			glm::vec3(0.0f, -1.0f, 0.0f)
		);

		int lightIndex = 0;
		for (auto &kv : frameInfo.gameObjects)
		{
			auto &obj = kv.second;
			if (obj.lightPoint == nullptr) continue;

			assert(lightIndex < MAX_LIGHTS && "Too many lights");

			// update light position
			obj.transform.translation = glm::vec3(rotateLight * glm::vec4(obj.transform.translation, 1.0f));

			// copy light data to ubo
			ubo.lights[lightIndex].position = glm::vec4(obj.transform.translation, 1.0f);
			ubo.lights[lightIndex].color = glm::vec4(obj.color, obj.lightPoint->intensity);

			lightIndex++;
		}
		ubo.lightCount = lightIndex;
	}

	void PointLightSystem::render(FrameInfo &frameInfo)
	{
		m_pipeline->bind(frameInfo.commandBuffer);

		vkCmdBindDescriptorSets(
			frameInfo.commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_pipelineLayout,
			0,
			1,
			&frameInfo.globalDescriptorSet,
			0,
			nullptr
		);

		for (auto &kv : frameInfo.gameObjects)
		{
			auto &obj = kv.second;
			if (obj.lightPoint == nullptr) continue;

			LightPointPushConstants push{};
			push.position = glm::vec4(obj.transform.translation, 1.0f);
			push.color = glm::vec4(obj.color, obj.lightPoint->intensity);
			push.radius = obj.transform.scale.x;

			vkCmdPushConstants(
				frameInfo.commandBuffer,
				m_pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(LightPointPushConstants),
				&push
			);

			vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
		}
	}

}
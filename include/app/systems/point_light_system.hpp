#pragma once

#include "pipeline.hpp"
#include "device.hpp"
#include "game_object.hpp"
#include "camera.hpp"
#include "frame_info.hpp"

#include "ft_glm/ft_glm.hpp"

#include <memory>
#include <vector>

namespace ft
{

	class PointLightSystem
	{

	public:

		PointLightSystem(Device &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~PointLightSystem();

		PointLightSystem(const PointLightSystem &) = delete;
		PointLightSystem &operator=(const PointLightSystem &) = delete;
	
		void update(FrameInfo &frameInfo, GlobalUBO &ubo);
		void render(FrameInfo &frameInfo);

	private:

		Device &m_device;

		std::unique_ptr<Pipeline> m_pipeline;
		VkPipelineLayout m_pipelineLayout;

		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

	};

}

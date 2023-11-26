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

	class RenderSystem
	{

	public:

		RenderSystem(Device &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~RenderSystem();

		RenderSystem(const RenderSystem &) = delete;
		RenderSystem &operator=(const RenderSystem &) = delete;
	
		void rendergameObjects(
			FrameInfo &frameInfo,
			std::vector<GameObject> &gameObjects
		);

	private:

		Device &m_device;

		std::unique_ptr<Pipeline> m_pipeline;
		VkPipelineLayout m_pipelineLayout;

		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

	};

}

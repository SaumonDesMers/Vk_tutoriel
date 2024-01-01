#pragma once

#include "defines.hpp"

#include <vulkan/vulkan.h>

namespace LIB_NAMESPACE
{
	class GraphicPipeline
	{
	
	public:

		struct CreateInfo: public VkGraphicsPipelineCreateInfo
		{
			CreateInfo(): VkGraphicsPipelineCreateInfo()
			{
				this->sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			}
		};

		GraphicPipeline(VkDevice device, const CreateInfo& createInfo);
		~GraphicPipeline();

		VkPipeline getVk() const { return m_pipeline; }

	private:

		VkPipeline m_pipeline;

		VkDevice m_device;

	};

}
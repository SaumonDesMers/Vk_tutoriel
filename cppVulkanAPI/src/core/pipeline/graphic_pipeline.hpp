#pragma once

#include "defines.hpp"

#include <vulkan/vulkan.h>

namespace LIB_NAMESPACE
{
	namespace core
	{
		class Pipeline
		{
		
		public:

			struct CreateInfo: public VkGraphicsPipelineCreateInfo
			{
				CreateInfo(): VkGraphicsPipelineCreateInfo()
				{
					this->sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
				}
			};

			Pipeline(VkDevice device, const VkGraphicsPipelineCreateInfo& createInfo);
			~Pipeline();

			VkPipeline getVk() const { return m_pipeline; }

		private:

			VkPipeline m_pipeline;

			VkDevice m_device;

		};
	}
}
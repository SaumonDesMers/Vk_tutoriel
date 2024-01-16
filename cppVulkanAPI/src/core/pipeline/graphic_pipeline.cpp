#include "graphic_pipeline.hpp"

#include <stdexcept>

namespace LIB_NAMESPACE
{
	namespace core
	{
		Pipeline::Pipeline(VkDevice device, const VkGraphicsPipelineCreateInfo& createInfo)
			: m_device(device)
		{
			if (vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &createInfo, nullptr, &m_pipeline) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create graphics pipeline.");
			}
		}

		Pipeline::~Pipeline()
		{
			vkDestroyPipeline(m_device, m_pipeline, nullptr);
		}
	}
}
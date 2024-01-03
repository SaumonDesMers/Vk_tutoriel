#include "graphic_pipeline.hpp"

#include <stdexcept>

namespace LIB_NAMESPACE
{
	GraphicPipeline::GraphicPipeline(VkDevice device, const CreateInfo& createInfo)
		: m_device(device)
	{
		if (vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &createInfo, nullptr, &m_pipeline) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create graphics pipeline.");
		}
	}

	GraphicPipeline::~GraphicPipeline()
	{
		vkDestroyPipeline(m_device, m_pipeline, nullptr);
	}

}
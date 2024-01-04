#include "pipeline_layout.hpp"

#include <stdexcept>

namespace LIB_NAMESPACE
{
	PipelineLayout::PipelineLayout(VkDevice device, const CreateInfo& createInfo)
		: m_device(device)
	{
		if (vkCreatePipelineLayout(m_device, &createInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout.");
		}
	}

	PipelineLayout::~PipelineLayout()
	{
		vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
	}

}
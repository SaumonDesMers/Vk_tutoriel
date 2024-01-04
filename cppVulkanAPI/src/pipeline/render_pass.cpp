#include "render_pass.hpp"

#include <stdexcept>

namespace LIB_NAMESPACE
{
	RenderPass::RenderPass(VkDevice device, const CreateInfo& createInfo)
		: m_device(device)
	{
		if (vkCreateRenderPass(m_device, &createInfo, nullptr, &m_renderPass) != VK_SUCCESS) {
			throw std::runtime_error("failed to create render pass.");
		}
	}

	RenderPass::~RenderPass()
	{
		vkDestroyRenderPass(m_device, m_renderPass, nullptr);
	}
}
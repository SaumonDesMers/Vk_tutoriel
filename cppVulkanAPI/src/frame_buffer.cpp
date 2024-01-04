#include "frame_buffer.hpp"

#include <stdexcept>

namespace LIB_NAMESPACE
{
	Framebuffer::Framebuffer(VkDevice device, const CreateInfo& createInfo)
		: m_device(device)
	{
		if (vkCreateFramebuffer(device, &createInfo, nullptr, &m_framebuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer.");
		}
	}

	Framebuffer::~Framebuffer()
	{
		vkDestroyFramebuffer(m_device, m_framebuffer, nullptr);
	}
}
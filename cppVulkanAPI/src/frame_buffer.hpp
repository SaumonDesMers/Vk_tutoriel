#pragma once

#include "defines.hpp"

#include <vulkan/vulkan.h>

namespace LIB_NAMESPACE
{
	class Framebuffer
	{
		public:

		struct CreateInfo: public VkFramebufferCreateInfo
		{
			CreateInfo(): VkFramebufferCreateInfo()
			{
				this->sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			}
		};

		Framebuffer(VkDevice device, const CreateInfo& createInfo);
		~Framebuffer();

		VkFramebuffer getVk() const { return m_framebuffer; }

	private:

		VkFramebuffer m_framebuffer;

		VkDevice m_device;

	};
}
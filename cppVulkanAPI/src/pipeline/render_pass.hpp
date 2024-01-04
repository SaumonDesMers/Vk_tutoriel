#pragma once

#include "../defines.hpp"

#include <vulkan/vulkan.h>

namespace LIB_NAMESPACE
{
	class RenderPass
	{

	public:

		struct CreateInfo: public VkRenderPassCreateInfo
		{
			CreateInfo(): VkRenderPassCreateInfo()
			{
				this->sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			}
		};

		RenderPass(VkDevice device, const CreateInfo& createInfo);
		~RenderPass();

		VkRenderPass getVk() const { return m_renderPass; }

	private:

		VkRenderPass m_renderPass;

		VkDevice m_device;

	};
}
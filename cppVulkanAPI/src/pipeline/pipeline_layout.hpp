#pragma once

#include "../defines.hpp"

#include <vulkan/vulkan.h>

namespace LIB_NAMESPACE
{
	class PipelineLayout
	{
	
	public:

		struct CreateInfo: public VkPipelineLayoutCreateInfo
		{
			CreateInfo(): VkPipelineLayoutCreateInfo()
			{
				this->sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			}
		};

		PipelineLayout(VkDevice device, const CreateInfo& createInfo);
		~PipelineLayout();

		VkPipelineLayout getVk() const { return m_pipelineLayout; }

	private:

		VkPipelineLayout m_pipelineLayout;

		VkDevice m_device;

	};

}
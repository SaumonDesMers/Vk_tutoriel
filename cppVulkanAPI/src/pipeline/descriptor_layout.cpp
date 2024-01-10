#include "descriptor_layout.hpp"

#include <stdexcept>

namespace LIB_NAMESPACE
{
	DescriptorSetLayout::DescriptorSetLayout(VkDevice device, VkDescriptorSetLayoutCreateInfo& createInfo)
		: m_device(device)
	{
		if (vkCreateDescriptorSetLayout(m_device, &createInfo, nullptr, &m_layout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create descriptor set layout!");
		}
	}

	DescriptorSetLayout::~DescriptorSetLayout()
	{
		vkDestroyDescriptorSetLayout(m_device, m_layout, nullptr);
	}
}
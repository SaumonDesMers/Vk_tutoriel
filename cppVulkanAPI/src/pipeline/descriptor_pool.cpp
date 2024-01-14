#include "descriptor_pool.hpp"

#include <stdexcept>

namespace LIB_NAMESPACE
{
	namespace core
	{
		DescriptorPool::DescriptorPool(VkDevice device, VkDescriptorPoolCreateInfo& createInfo)
			: m_device(device)
		{
			if (vkCreateDescriptorPool(m_device, &createInfo, nullptr, &m_descriptorPool) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create descriptor pool.");
			}
		}

		DescriptorPool::DescriptorPool(VkDevice device, uint32_t maxSets, uint32_t poolSizeCount, VkDescriptorPoolSize* poolSizes)
			: m_device(device)
		{
			VkDescriptorPoolCreateInfo poolInfo = {};
			poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			poolInfo.poolSizeCount = poolSizeCount;
			poolInfo.pPoolSizes = poolSizes;
			poolInfo.maxSets = maxSets;

			if (vkCreateDescriptorPool(m_device, &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create descriptor pool.");
			}
		}

		DescriptorPool::~DescriptorPool()
		{
			vkDestroyDescriptorPool(m_device, m_descriptorPool, nullptr);
		}
	}
}

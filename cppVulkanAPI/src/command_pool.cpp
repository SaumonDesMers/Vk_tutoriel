#include "command_pool.hpp"

#include <stdexcept>

namespace LIB_NAMESPACE
{
	CommandPool::CommandPool(VkDevice device, const CreateInfo& createInfo)
		: m_device(device)
	{
		if (vkCreateCommandPool(device, &createInfo, nullptr, &m_commandPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create command pool.");
		}
	}

	CommandPool::~CommandPool()
	{
		vkDestroyCommandPool(m_device, m_commandPool, nullptr);
	}
}
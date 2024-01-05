#include "sync_object.hpp"

#include <stdexcept>

namespace LIB_NAMESPACE
{
	Semaphore::Semaphore(VkDevice device, const CreateInfo& createInfo)
		: m_device(device)
	{
		if (vkCreateSemaphore(device, &createInfo, nullptr, &m_semaphore) != VK_SUCCESS) {
			throw std::runtime_error("failed to create semaphore.");
		}
	}

	Semaphore::~Semaphore()
	{
		vkDestroySemaphore(m_device, m_semaphore, nullptr);
	}



	Fence::Fence(VkDevice device, const CreateInfo& createInfo)
		: m_device(device)
	{
		if (vkCreateFence(device, &createInfo, nullptr, &m_fence) != VK_SUCCESS) {
			throw std::runtime_error("failed to create fence.");
		}
	}

	Fence::~Fence()
	{
		vkDestroyFence(m_device, m_fence, nullptr);
	}
}
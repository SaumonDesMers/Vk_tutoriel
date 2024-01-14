#include "sync_object.hpp"

#include <stdexcept>

namespace LIB_NAMESPACE
{
	namespace core
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

		void Fence::wait(uint64_t timeout)
		{
			vkWaitForFences(m_device, 1, &m_fence, VK_TRUE, timeout);
		}

		void Fence::reset()
		{
			vkResetFences(m_device, 1, &m_fence);
		}
	}
}
#include "queue.hpp"

namespace LIB_NAMESPACE
{
	namespace core
	{
		Queue::Queue(VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex)
		{
			vkGetDeviceQueue(device, queueFamilyIndex, queueIndex, &m_queue);
		}

		Queue::~Queue()
		{
		}

		VkResult Queue::submit(uint32_t submitCount, const VkSubmitInfo* submits, VkFence fence)
		{
			return vkQueueSubmit(m_queue, submitCount, submits, fence);
		}

		VkResult Queue::waitIdle()
		{
			return vkQueueWaitIdle(m_queue);
		}
	}
}
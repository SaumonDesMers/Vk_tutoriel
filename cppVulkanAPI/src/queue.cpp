#include "queue.hpp"

namespace LIB_NAMESPACE
{
	Queue::Queue(VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex)
	{
		vkGetDeviceQueue(device, queueFamilyIndex, queueIndex, &m_queue);
	}

	Queue::~Queue()
	{
	}
}
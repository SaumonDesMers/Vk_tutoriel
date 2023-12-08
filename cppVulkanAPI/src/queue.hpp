#pragma once

#include "defines.hpp"

#include <vulkan/vulkan.h>

namespace LIB_NAMESPACE
{
	class Queue
	{

	public:

		struct CreateInfo: public VkDeviceQueueCreateInfo
		{
			CreateInfo(): VkDeviceQueueCreateInfo()
			{
				sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			}
		};

		Queue(VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex = 0);

		~Queue();

		VkQueue getVk() const { return m_queue; }

	private:

		VkQueue m_queue;

	};

}
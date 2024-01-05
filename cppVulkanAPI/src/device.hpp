#pragma once

#include "defines.hpp"

#include <vulkan/vulkan.h>

namespace LIB_NAMESPACE
{
	class Device
	{

	public:

		struct CreateInfo: public VkDeviceCreateInfo
		{
			CreateInfo(): VkDeviceCreateInfo()
			{
				sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			}
		};

		Device(VkPhysicalDevice physicalDevice, const CreateInfo & createInfo);
		~Device();

		VkDevice getVk() const { return m_device; }

		VkResult waitIdle();

	private:

		VkDevice m_device;
	};
}
#include "device.hpp"

#include <stdexcept>
#include <iostream>

namespace LIB_NAMESPACE
{
	Device::Device(VkPhysicalDevice physicalDevice, const CreateInfo & createInfo)
	{
		if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create logical device!");
		}
		std::cout << "Created logical device" << std::endl;
	}

	Device::~Device()
	{
		vkDestroyDevice(m_device, nullptr);
	}
}
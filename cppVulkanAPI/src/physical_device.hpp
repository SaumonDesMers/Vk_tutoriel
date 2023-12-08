#pragma once

#include "defines.hpp"
#include "instance.hpp"
#include "queue.hpp"

#include <vulkan/vulkan.h>

#include <vector>

namespace LIB_NAMESPACE
{
	class PhysicalDevice
	{

	public:

		struct Properties: public VkPhysicalDeviceProperties
		{
			Properties(const VkPhysicalDevice& physicalDevice)
			{
				vkGetPhysicalDeviceProperties(physicalDevice, this);
			}
		};

		struct Features: public VkPhysicalDeviceFeatures
		{
			Features() : VkPhysicalDeviceFeatures() {}

			Features(const VkPhysicalDevice& physicalDevice)
				: VkPhysicalDeviceFeatures()
			{
				vkGetPhysicalDeviceFeatures(physicalDevice, this);
			}
		};

		PhysicalDevice(VkPhysicalDevice physicalDevice);

		~PhysicalDevice();

		VkPhysicalDevice getVk() const { return m_physicalDevice; }

		static std::vector<VkPhysicalDevice> enumeratePhysicalDevices(VkInstance instance);

		template <typename UnaryPredicate>
		static VkPhysicalDevice pickSuitablePhysicalDevice(
			VkInstance instance,
			UnaryPredicate isDeviceSuitable
		)
		{
			std::vector<VkPhysicalDevice> devices = PhysicalDevice::enumeratePhysicalDevices(instance);

			for (const auto& device : devices)
			{
				if (isDeviceSuitable(device))
				{
					return device;
				}
			}

			return VK_NULL_HANDLE;
		}

		static std::vector<VkQueueFamilyProperties> getQueueFamilyProperties(VkPhysicalDevice physicalDevice);

	private:

		VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;

	};

}
#include "physical_device.hpp"

namespace LIB_NAMESPACE
{
	PhysicalDevice::PhysicalDevice(VkPhysicalDevice physicalDevice)
		: m_physicalDevice(physicalDevice)
	{

	}

	PhysicalDevice::~PhysicalDevice()
	{

	}

	std::vector<VkQueueFamilyProperties> PhysicalDevice::getQueueFamilyProperties(VkPhysicalDevice physicalDevice)
	{
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

		return queueFamilyProperties;
	}

	bool PhysicalDevice::getSurfaceSupport(
		VkPhysicalDevice physicalDevice,
		uint32_t queueFamilyIndex,
		VkSurfaceKHR surface
	)
	{
		VkBool32 surfaceSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFamilyIndex, surface, &surfaceSupport);

		return surfaceSupport;
	}
}
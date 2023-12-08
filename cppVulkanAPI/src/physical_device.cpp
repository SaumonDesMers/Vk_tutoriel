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

	std::vector<VkPhysicalDevice> PhysicalDevice::enumeratePhysicalDevices(VkInstance instance)
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		return devices;
	}

	std::vector<VkQueueFamilyProperties> PhysicalDevice::getQueueFamilyProperties(VkPhysicalDevice physicalDevice)
	{
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

		return queueFamilyProperties;
	}
}
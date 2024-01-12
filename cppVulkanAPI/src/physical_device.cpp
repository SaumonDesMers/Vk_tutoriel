#include "physical_device.hpp"

#include <string>
#include <set>

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

	VkSurfaceCapabilitiesKHR PhysicalDevice::getSurfaceCapabilities(
		VkPhysicalDevice physicalDevice,
		VkSurfaceKHR surface
	)
	{
		VkSurfaceCapabilitiesKHR surfaceCapabilities;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);

		return surfaceCapabilities;
	}

	std::vector<VkSurfaceFormatKHR> PhysicalDevice::getSurfaceFormats(
		VkPhysicalDevice physicalDevice,
		VkSurfaceKHR surface
	)
	{
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

		std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, surfaceFormats.data());

		return surfaceFormats;
	}

	std::vector<VkPresentModeKHR> PhysicalDevice::getSurfacePresentModes(
		VkPhysicalDevice physicalDevice,
		VkSurfaceKHR surface
	)
	{
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

		std::vector<VkPresentModeKHR> presentModes(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data());

		return presentModes;
	}

	bool PhysicalDevice::checkExtensionSupport(
		const VkPhysicalDevice& physicalDevice,
		const std::vector<const char*>& extensions
	)
	{
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(extensions.begin(), extensions.end());

		for (const auto& extension : availableExtensions)
		{
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	void PhysicalDevice::getProperties(VkPhysicalDeviceProperties* properties) const
	{
		vkGetPhysicalDeviceProperties(m_physicalDevice, properties);
	}
}
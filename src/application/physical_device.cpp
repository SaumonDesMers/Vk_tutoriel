#include "application.hpp"

void Application::pickPhysicalDevice() {
	/* Get number of available devices */
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(this->instance, &deviceCount, nullptr);

	/* Check if there are any devices */
	if (deviceCount == 0) {
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}

	/* Get all available devices */
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(this->instance, &deviceCount, devices.data());

	/* Pick the first suitable device */
	for (const auto& device : devices) {
		if (this->isDeviceSuitable(device)) {
			this->physicalDevice = device;
			break;
		}
	}

	/* Check if there is a suitable device */
	if (this->physicalDevice == VK_NULL_HANDLE) {
		throw std::runtime_error("failed to find a suitable GPU!");
	}
}

bool Application::isDeviceSuitable(VkPhysicalDevice device) {
	/* Check for required queue families */
	QueueFamilyIndices indices = this->findQueueFamilies(device);

	/* Check for required extensions */
	bool extensionsSupported = this->checkDeviceExtensionSupport(device);

	/* Check for required swap chain. For now, we only check if there is at least one supported format and one supported presentation mode */
	bool swapChainAdequate = false;
	if (extensionsSupported) { /* Swap chain being an extension, check it only if the extension is supported */
		SwapChainSupportDetails swapChainSupport = this->querySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

bool Application::checkDeviceExtensionSupport(VkPhysicalDevice device) {
	/* Get number of available extensions */
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	/* Get all available extensions */
	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	/* Check if all required extensions are available by removing them from the list of available extensions and checking if the list is empty */
	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

QueueFamilyIndices Application::findQueueFamilies(VkPhysicalDevice device) {
	QueueFamilyIndices indices;

	/* Get number of available queue families */
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	/* Get all available queue families */
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	/* Check if there is families that support required operations, if so, save their index */
	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		/* Check for graphics support */
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}

		/* Check for presentation support */
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, this->surface, &presentSupport);
		if (presentSupport) {
			indices.presentFamily = i;
		}

		if (indices.isComplete())
			break;
		i++;
	}

	return indices;
}
#include "application.hpp"

void Application::createLogicalDevice() {
	/* Specify which queue families to create */
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

	/* Create all queue families */
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	/* Use a set to avoid duplicate queue families when one queue family supports multiple operations */
	std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	/* Specify which device features to enable (none for now) */
	VkPhysicalDeviceFeatures deviceFeatures{};

	/* Set up information about the logical device */
	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = static_cast<uint32_t> (queueCreateInfos.size());
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());

	/* Validation layers in logical devices are deprecated, but we set them up for backwards compatibility */
	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	} else {
		createInfo.enabledLayerCount = 0;
	}

	/* Create the logical device */
	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
		throw std::runtime_error("failed to create logical device!");
	}

	/* Get the queues handles */
	vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}
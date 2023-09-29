#include "application.hpp"

void Application::recreateSwapChain() {
	/* TODO: Instead of waiting for the device to become idle, which force us to stop rendering,
			we should recreate the swap chain while we are still rendering to the images of the old swap chain.
			For that we need to pass the previous swap chain to the oldSwapChain field in the VkSwapchainCreateInfoKHR
			struct and destroy the old swap chain as soon as we are done with it */
	
	/* If the window is minimized, wait until it is restored */
	int width = 0, height = 0;
    glfwGetFramebufferSize(window, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(window, &width, &height);
        glfwWaitEvents();
    }

	vkDeviceWaitIdle(this->device);

	this->cleanupSwapChain();

    this->createSwapChain();
    this->createImageViews();
    this->createFramebuffers();
}

void Application::createSwapChain() {
	SwapChainSupportDetails swapChainSupport = this->querySwapChainSupport(this->physicalDevice);

	VkSurfaceFormatKHR surfaceFormat = this->chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = this->chooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = this->chooseSwapExtent(swapChainSupport.capabilities);

	/* Store the chosen values for later use */
	this->swapChainImageFormat = surfaceFormat.format;
	this->swapChainExtent = extent;

	/* Choose the number of images in the swap chain. We try to get one more than the minimum to avoid waiting for the driver to complete internal operations before we can acquire another image to render to */
	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	/* Check if the maximum number of images is exceeded, where 0 means that there is no maximum */
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = this->surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	/* The imageArrayLayers specifies the amount of layers each image consists of. This is always 1 unless you are developing a stereoscopic 3D application */
	createInfo.imageArrayLayers = 1;
	/* The imageUsage specifies what kind of operations we will use the images in the swap chain for. In this case, we will render directly to them, which means that they are used as color attachment */
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = this->findQueueFamilies(this->physicalDevice);
	uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

	/* Images sharing mode is used to determine if the images can be used across multiple queue families */
	if (indices.graphicsFamily != indices.presentFamily) {
		/* Image can be used across multiple queue families without explicit ownership transfers */
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	} else {
		/* Image is owned by one queue family at a time and ownership must be explicitly transfered before using it in another queue family. This option offers the best performance */
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
	}

	/* We can specify that a certain transform should be applied to images in the swap chain if it is supported (supportedTransforms in capabilities). For example, we could rotate or flip images.
		To specify that you do not want any transformation, simply specify the current transformation */
	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	/* Specify if the alpha channel should be used for blending with other windows in the window system */
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	/* Specify if we care about the color of pixels that are obscured, e.g. because another window is in front of them. If true, the implementation may discard rendering operations that are obscured */
	createInfo.clipped = VK_TRUE;
	/* Specify a handle to an old swap chain if we are recreating it (when the window is resized for example) */
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	/* Create the swap chain */
	if (vkCreateSwapchainKHR(this->device, &createInfo, nullptr, &this->swapChain) != VK_SUCCESS) {
		throw std::runtime_error("failed to create swap chain!");
	}

	/* Get the handles of the swap chain images */
	vkGetSwapchainImagesKHR(this->device, this->swapChain, &imageCount, nullptr);
	this->swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(this->device, this->swapChain, &imageCount, this->swapChainImages.data());
}

SwapChainSupportDetails Application::querySwapChainSupport(VkPhysicalDevice device) {
	SwapChainSupportDetails details;

	/* Get the basic surface capabilities */
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, this->surface, &details.capabilities);

	/* Get the supported surface formats */
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, this->surface, &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, this->surface, &formatCount, details.formats.data());
	}

	/* Get the supported presentation modes */
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, this->surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, this->surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

/*
 * The surface format is the color depth of the swap chain images.
 * It is represented by a combination of format and color space.
 * The format specifies the color channels and types (e.g. RGBA 32 bit).
 * The color space specifies if the SRGB color space is supported or not.
 */
VkSurfaceFormatKHR Application::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
	/* Check for the preferred format: 32 bit BGRA with SRGB color space */
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}

	/* If the preferred format is not available, return the first available format */
	return availableFormats[0];
}

/*
 * The presentation mode is the condition for "swapping" images to the screen.
 * Possibles modes are:
 * VK_PRESENT_MODE_IMMEDIATE_KHR
 * VK_PRESENT_MODE_FIFO_KHR
 * VK_PRESENT_MODE_FIFO_RELAXED_KHR
 * VK_PRESENT_MODE_MAILBOX_KHR
 */
VkPresentModeKHR Application::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
	/* Check for the preferred presentation mode: Mailbox (triple buffering) */
	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}

	/* If the preferred presentation mode is not available, return the default one */
	return VK_PRESENT_MODE_FIFO_KHR;
}

/*
 * The swap extent is the resolution of the swap chain images.
 */
VkExtent2D Application::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
	/* If the current extent is not the maximum value, use it */
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	} else {
		/* Get the actual extent of the window in pixels. This is needed because the size in pixels may be different from the size in screen coordinates (e.g. on HiDPI display) */
		int width, height;
		glfwGetFramebufferSize(this->window, &width, &height);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		/* Clamp the width and height between the allowed minimum and maximum extents */
		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}
}
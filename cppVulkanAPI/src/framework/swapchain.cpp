#include "swapchain.hpp"

#include <algorithm>

namespace LIB_NAMESPACE
{
	Swapchain::Swapchain(VkDevice device, const CreateInfo& createInfo)
	{
		createSwapchain(device, createInfo);
		createImageViews(device);
	}

	Swapchain::~Swapchain()
	{
	}

	void Swapchain::createSwapchain(VkDevice device, const CreateInfo& createInfo)
	{
		VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(createInfo.supportDetails.formats);
		VkPresentModeKHR presentMode = chooseSwapPresentMode(createInfo.supportDetails.presentModes);
		VkExtent2D extent = chooseSwapExtent(
			createInfo.supportDetails.capabilities,
			createInfo.frameBufferExtent.width, 
			createInfo.frameBufferExtent.height
		);

		uint32_t imageCount = createInfo.supportDetails.capabilities.minImageCount + 1;

		if (
			createInfo.supportDetails.capabilities.maxImageCount > 0
			&& imageCount > createInfo.supportDetails.capabilities.maxImageCount
		)
		{
			imageCount = createInfo.supportDetails.capabilities.maxImageCount;
		}

		ft::core::Swapchain::CreateInfo swapchainInfo = {};
		swapchainInfo.surface = createInfo.surface;
		swapchainInfo.minImageCount = imageCount;
		swapchainInfo.imageFormat = surfaceFormat.format;
		swapchainInfo.imageColorSpace = surfaceFormat.colorSpace;
		swapchainInfo.imageExtent = extent;
		swapchainInfo.imageArrayLayers = 1;
		swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		uint32_t queueFamilyIndices[] = {
			createInfo.queueFamilyIndices.graphicsFamily.value(),
			createInfo.queueFamilyIndices.presentFamily.value()
		};

		if (createInfo.queueFamilyIndices.graphicsFamily != createInfo.queueFamilyIndices.presentFamily)
		{
			swapchainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			swapchainInfo.queueFamilyIndexCount = 2;
			swapchainInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}

		swapchainInfo.preTransform = createInfo.supportDetails.capabilities.currentTransform;
		swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapchainInfo.presentMode = presentMode;
		swapchainInfo.clipped = VK_TRUE;

		swapchainInfo.oldSwapchain = VK_NULL_HANDLE;

		swapchain = std::make_unique<ft::core::Swapchain>(device, swapchainInfo);
	}

	void Swapchain::createImageViews(VkDevice device)
	{
		imageViews.resize(swapchain->getImageCount());

		for (size_t i = 0; i < swapchain->getImageCount(); i++)
		{
			ft::core::ImageView::CreateInfo imageViewInfo = {};
			imageViewInfo.image = swapchain->getImage(i);
			imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imageViewInfo.format = swapchain->getImageFormat();
			imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageViewInfo.subresourceRange.baseMipLevel = 0;
			imageViewInfo.subresourceRange.levelCount = 1;
			imageViewInfo.subresourceRange.baseArrayLayer = 0;
			imageViewInfo.subresourceRange.layerCount = 1;

			imageViews[i] = std::make_unique<ft::core::ImageView>(device, imageViewInfo);
		}
	}


	VkSurfaceFormatKHR Swapchain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		for (const auto& availableFormat : availableFormats)
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB
				&& availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return availableFormat;
			}
		}

		return availableFormats[0];
	}

	VkPresentModeKHR Swapchain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
	{
		for (const auto& availablePresentMode : availablePresentModes)
		{
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				return availablePresentMode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D Swapchain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, int width, int height)
	{
		if (capabilities.currentExtent.width != UINT32_MAX)
		{
			return capabilities.currentExtent;
		}
		else
		{
			VkExtent2D actualExtent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};

			actualExtent.width = std::clamp(
				actualExtent.width,
				capabilities.minImageExtent.width,
				capabilities.maxImageExtent.width
			);

			actualExtent.height = std::clamp(
				actualExtent.height,
				capabilities.minImageExtent.height,
				capabilities.maxImageExtent.height
			);

			return actualExtent;
		}
	}

}
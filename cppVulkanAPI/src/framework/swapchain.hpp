#pragma once

#include "defines.hpp"
#include "queue.hpp"
#include "core/swapchain.hpp"
#include "core/image/image_view.hpp"

#include <memory>

namespace LIB_NAMESPACE
{
	class Swapchain
	{

	public:

		struct SupportDetails
		{
			VkSurfaceCapabilitiesKHR capabilities;
			std::vector<VkSurfaceFormatKHR> formats;
			std::vector<VkPresentModeKHR> presentModes;
		};

		struct CreateInfo
		{
			VkSurfaceKHR surface;
			SupportDetails supportDetails;
			VkExtent2D frameBufferExtent;
			Queue::FamilyIndices queueFamilyIndices;
			VkSwapchainKHR oldSwapchain;
		};

		std::unique_ptr<ft::core::Swapchain> swapchain;
		std::vector<std::unique_ptr<ft::core::ImageView>> imageViews;

		Swapchain(VkDevice device, const CreateInfo& createInfo);
		~Swapchain();

	private:

		void createSwapchain(VkDevice device, const CreateInfo& createInfo);
		void createImageViews(VkDevice device);

		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, int width, int height);

	};
}
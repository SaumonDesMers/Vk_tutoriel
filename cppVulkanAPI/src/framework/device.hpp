#pragma once

#include "defines.hpp"
#include "window/window.hpp"
#include "core/instance.hpp"
#include "core/debug.hpp"
#include "core/physical_device.hpp"

#include <memory>
#include <optional>

namespace LIB_NAMESPACE
{
	
	class Device
	{

	public:

		std::unique_ptr<Window::Manager> windowManager;
		std::unique_ptr<Window> window;

		std::unique_ptr<ft::core::Instance> instance;
		std::unique_ptr<ft::core::DebugMessenger> debugMessenger;

		std::unique_ptr<ft::Window::Surface> surface;

		std::unique_ptr<ft::core::PhysicalDevice> physicalDevice;
		VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;


		Device();
		~Device();

	private:


		struct QueueFamilyIndices {
			std::optional<uint32_t> graphicsFamily;
			std::optional<uint32_t> presentFamily;

			bool isComplete() {
				return graphicsFamily.has_value() && presentFamily.has_value();
			}
		};

		struct SwapChainSupportDetails
		{
			VkSurfaceCapabilitiesKHR capabilities;
			std::vector<VkSurfaceFormatKHR> formats;
			std::vector<VkPresentModeKHR> presentModes;
		};

		const std::vector<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};

		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		void createWindow();
		void createInstance();
		void setupDebugMessenger();
		void createSurface();
		void pickPhysicalDevice();

		std::vector<const char*> getRequiredExtensions();
		void populateDebugMessengerCreateInfo(ft::core::DebugMessenger::CreateInfo& createInfo);
		bool isDeviceSuitable(const VkPhysicalDevice& physicalDevice);
		QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice& physicalDevice);
		SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice& device);
		VkSampleCountFlagBits getMaxUsableSampleCount(const VkPhysicalDevice& physicalDevice);

	};
}
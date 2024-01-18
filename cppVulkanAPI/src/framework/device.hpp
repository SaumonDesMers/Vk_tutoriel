#pragma once

#include "defines.hpp"
#include "window/window.hpp"
#include "core/instance.hpp"
#include "core/debug.hpp"
#include "core/physical_device.hpp"
#include "core/device.hpp"
#include "swapchain.hpp"
#include "queue.hpp"

#include <memory>
#include <optional>

namespace LIB_NAMESPACE
{

	class Device
	{

	public:

		std::unique_ptr<Window::Manager> windowManager;
		std::unique_ptr<Window> window;

		std::unique_ptr<core::Instance> instance;
		std::unique_ptr<core::DebugMessenger> debugMessenger;

		std::unique_ptr<Window::Surface> surface;

		std::unique_ptr<core::PhysicalDevice> physicalDevice;
		VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;

		std::unique_ptr<core::Device> device;
		std::unique_ptr<core::Queue> graphicsQueue;
		std::unique_ptr<core::Queue> presentQueue;


		Device();
		~Device();

		Swapchain::SupportDetails querySwapChainSupport(const VkPhysicalDevice& device);
		Queue::FamilyIndices findQueueFamilies(const VkPhysicalDevice& physicalDevice);

	private:

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
		void createLogicalDevice();

		std::vector<const char*> getRequiredExtensions();
		void populateDebugMessengerCreateInfo(ft::core::DebugMessenger::CreateInfo& createInfo);
		bool isDeviceSuitable(const VkPhysicalDevice& physicalDevice);
		VkSampleCountFlagBits getMaxUsableSampleCount(const VkPhysicalDevice& physicalDevice);

	};
}
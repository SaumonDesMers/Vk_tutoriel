#pragma once

#include "defines.hpp"
#include "window/window.hpp"
#include "core/instance.hpp"
#include "core/debug.hpp"

#include <memory>

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


		Device();
		~Device();

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

		std::vector<const char*> getRequiredExtensions();
		void populateDebugMessengerCreateInfo(ft::core::DebugMessenger::CreateInfo& createInfo);

	};
}
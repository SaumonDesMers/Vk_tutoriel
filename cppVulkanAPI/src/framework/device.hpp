#pragma once

#include "defines.hpp"
#include "window/window.hpp"
#include "core/instance.hpp"
#include "core/debug.hpp"

#include <memory>

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

namespace LIB_NAMESPACE
{
	class Device
	{

	public:

		std::unique_ptr<Window::Manager> windowManager;
		std::unique_ptr<Window> window;

		std::unique_ptr<ft::core::Instance> instance;


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

		std::vector<const char*> getRequiredExtensions();
		void populateDebugMessengerCreateInfo(ft::core::DebugMessenger::CreateInfo& createInfo);

	};
}
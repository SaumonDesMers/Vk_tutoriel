#include "device.hpp"

#include <stdexcept>

namespace LIB_NAMESPACE
{
	Device::Device()
	{
		createWindow();
		createInstance();
	}

	Device::~Device()
	{

	}

	
	void Device::createWindow()
	{
		windowManager = std::make_unique<Window::Manager>();
		window = windowManager->createWindow("Master window", 800, 600);
	}

	void Device::createInstance()
	{
		if (enableValidationLayers && ft::core::Instance::checkValidationLayerSupport(validationLayers) == false)
		{
			throw std::runtime_error("Validation layers requested, but not available");
		}

		ft::core::ApplicationInfo appInfo = {};
		appInfo.pApplicationName = "Playground";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "Playground Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_2;

		ft::core::Instance::CreateInfo createInfo = {};
		createInfo.pApplicationInfo = &appInfo;

		std::vector<const char*> extensions = getRequiredExtensions();

		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		ft::core::DebugMessenger::CreateInfo debugCreateInfo = {};
		if (enableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();

			populateDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = &debugCreateInfo;
		}
		else
		{
			createInfo.enabledLayerCount = 0;
		}

		instance = std::make_unique<ft::core::Instance>(createInfo);
	}


	std::vector<const char*> Device::getRequiredExtensions() {
		std::vector<const char*> extensions = windowManager->getRequiredInstanceExtensions();

		if (enableValidationLayers) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}

	void Device::populateDebugMessengerCreateInfo(ft::core::DebugMessenger::CreateInfo& createInfo)
	{
		createInfo.messageSeverity =
			// VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			// VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

		createInfo.messageType =
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

		createInfo.pUserData = nullptr; // Optional

		/*
			This is used internally by the debug messenger to call the user callback.
			createInfo.pfnUserCallback = customUserCallback;

			Instead, use the following:
			createInfo.userCallback = customUserCallback;

			Which has the default value: ft::core::DebugMessenger::debugCallback
		*/
	}
}
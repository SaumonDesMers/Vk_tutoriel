#include "application.hpp"
#include "logger.hpp"

#include <vulkan/vulkan.h>

#include <set>

Application::Application()
{
	
}

Application::~Application()
{
	
}

void Application::run()
{
	init();

	while (m_window->shouldClose() == false)
	{
		m_windowManager->pollEvents();
	}
}

void Application::init()
{
	createWindowManager();
	createWindow();
	createInstance();
	setupDebugMessenger();
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();

	FT_INFO("Application initialized");
}

void Application::createWindowManager()
{
	m_windowManager = std::make_unique<ft::WindowManager>();
}

void Application::createWindow()
{
	ft::WindowCreateInfo createInfo = {};
	createInfo.title = "Playground";
	createInfo.width = 800;
	createInfo.height = 600;

	m_window = m_windowManager->createWindow(createInfo);
}

void Application::createInstance()
{
	if (enableValidationLayers && ft::Instance::checkValidationLayerSupport(validationLayers) == false)
	{
		throw std::runtime_error("Validation layers requested, but not available");
	}

	ft::ApplicationInfo appInfo = {};
	appInfo.pApplicationName = "Playground";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "Playground Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_2;

	ft::Instance::CreateInfo createInfo = {};
	createInfo.pApplicationInfo = &appInfo;

	std::vector<const char*> extensions = getRequiredExtensions();

	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	ft::DebugMessenger::CreateInfo debugCreateInfo = {};
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

	m_instance = std::make_unique<ft::Instance>(createInfo);
}

void Application::setupDebugMessenger()
{
	if (enableValidationLayers == false)
	{
		return;
	}

	ft::DebugMessenger::CreateInfo createInfo = {};
	populateDebugMessengerCreateInfo(createInfo);

	m_debugMessenger = std::make_unique<ft::DebugMessenger>(m_instance.get(), &createInfo);
}

void Application::createSurface()
{
	m_surface = std::make_unique<ft::Window::Surface>(m_instance->getVk(), m_window->getGLFWwindow());
}

void Application::pickPhysicalDevice()
{
	std::vector<VkPhysicalDevice> physicalDevices = m_instance->getPhysicalDevices();

	if (physicalDevices.empty())
	{
		throw std::runtime_error("Failed to find GPUs with Vulkan support");
	}

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	for (const auto& phyDev : physicalDevices)
	{
		if (isDeviceSuitable(phyDev))
		{
			physicalDevice = phyDev;
			break;
		}
	}

	if (physicalDevice == VK_NULL_HANDLE)
	{
		throw std::runtime_error("Failed to find a suitable GPU");
	}

	m_physicalDevice = std::make_unique<ft::PhysicalDevice>(physicalDevice);
}

void Application::createLogicalDevice()
{
	QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice->getVk());

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		ft::Queue::CreateInfo queueCreateInfo = {};
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	ft::PhysicalDevice::Features deviceFeatures = {};

	ft::Device::CreateInfo createInfo = {};
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();

	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	m_device = std::make_unique<ft::Device>(m_physicalDevice->getVk(), createInfo);

	m_graphicsQueue = std::make_unique<ft::Queue>(m_device->getVk(), indices.graphicsFamily.value());
	m_presentQueue = std::make_unique<ft::Queue>(m_device->getVk(), indices.presentFamily.value());
}

std::vector<const char*> Application::getRequiredExtensions() {
    std::vector<const char*> extensions = m_windowManager->getRequiredInstanceExtensions();

    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

void Application::populateDebugMessengerCreateInfo(ft::DebugMessenger::CreateInfo& createInfo)
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

		Which has the default value: ft::DebugMessenger::debugCallback
	*/
}

bool Application::isDeviceSuitable(const VkPhysicalDevice& physicalDevice)
{
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

	bool extensionsSupported = ft::PhysicalDevice::checkExtensionSupport(physicalDevice, deviceExtensions);

	return indices.isComplete() && extensionsSupported;
}

QueueFamilyIndices Application::findQueueFamilies(const VkPhysicalDevice& physicalDevice)
{
	QueueFamilyIndices indices;

	std::vector<VkQueueFamilyProperties> queueFamilyProperties = ft::PhysicalDevice::getQueueFamilyProperties(physicalDevice);

	int i = 0;
	for (const auto& queueFamily : queueFamilyProperties)
	{
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = ft::PhysicalDevice::getSurfaceSupport(physicalDevice, i, m_surface->getVk());

		if (presentSupport)
		{
			indices.presentFamily = i;
		}

		if (indices.isComplete())
		{
			break;
		}

		i++;
	}

	return indices;
}

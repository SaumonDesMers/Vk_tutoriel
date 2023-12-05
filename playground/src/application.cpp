#include "application.hpp"

#include <vulkan/vulkan.h>

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

	ft::InstanceCreateInfo createInfo = {};
	createInfo.pApplicationInfo = &appInfo;

	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	m_instance = std::make_unique<ft::Instance>(createInfo);
}
#include "window_manager.hpp"

namespace LIB_NAMESPACE
{

	WindowManager::WindowManager()
	{
		glfwInit();
	}

	WindowManager::~WindowManager()
	{
		glfwTerminate();
	}

	std::unique_ptr<Window> WindowManager::createWindow(const WindowCreateInfo& createInfo)
	{
		return std::make_unique<Window>(createInfo);
	}

	void WindowManager::pollEvents()
	{
		glfwPollEvents();
	}

	void WindowManager::waitEvents()
	{
		glfwWaitEvents();
	}

	std::vector<const char *> WindowManager::getRequiredInstanceExtensions()
	{
		uint32_t glfwExtensionCount = 0;
		const char ** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		return extensions;
	}
}
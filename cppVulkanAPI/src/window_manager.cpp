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

}
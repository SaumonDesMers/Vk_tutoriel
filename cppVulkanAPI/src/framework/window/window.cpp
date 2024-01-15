#include "window.hpp"

#include <stdexcept>

namespace LIB_NAMESPACE
{
	Window::Window(const std::string& title, int width, int height):
		m_title(title), m_width(width), m_height(height)
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);

		if (!m_window)
		{
			throw std::runtime_error("failed to create window.");
		}

		glfwSetWindowUserPointer(m_window, this);
		glfwSetFramebufferSizeCallback(m_window, framebufferResizeCallback);
	}

	Window::~Window()
	{
		glfwDestroyWindow(m_window);
	}

	void Window::framebufferResizeCallback(GLFWwindow* window, int, int)
	{
		auto userPtr = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		userPtr->m_framebufferResized = true;
	}


	Window::Surface::Surface(VkInstance instance, GLFWwindow* window)
		: m_instance(instance)
	{
		if (glfwCreateWindowSurface(m_instance, window, nullptr, &m_surface) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create window surface.");
		}
	}

	Window::Surface::~Surface()
	{
		vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
	}



	Window::Manager::Manager()
	{
		glfwInit();
	}

	Window::Manager::~Manager()
	{
		glfwTerminate();
	}

	std::unique_ptr<Window> Window::Manager::createWindow(const std::string& title, int width, int height)
	{
		return std::make_unique<Window>(title, width, height);
	}

	void Window::Manager::pollEvents()
	{
		glfwPollEvents();
	}

	void Window::Manager::waitEvents()
	{
		glfwWaitEvents();
	}

	std::vector<const char *> Window::Manager::getRequiredInstanceExtensions()
	{
		uint32_t glfwExtensionCount = 0;
		const char ** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		return extensions;
	}
}
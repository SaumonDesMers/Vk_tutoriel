#include "window.hpp"

#include <stdexcept>

namespace LIB_NAMESPACE
{
	Window::Window(const WindowCreateInfo& createInfo):
		m_title(createInfo.title),
		m_width(createInfo.width),
		m_height(createInfo.height)
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);

		if (!m_window)
		{
			throw std::runtime_error("failed to create window.");
		}
	}

	Window::~Window()
	{
		glfwDestroyWindow(m_window);
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
}
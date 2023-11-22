#include "app/window.hpp"

namespace ft
{

	Window::Window(std::string title, int width, int height)
		: m_title(title), m_width(width), m_height(height)
	{
		init();
	}

	Window::~Window()
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}

	void Window::init()
	{
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);

		glfwSetWindowUserPointer(m_window, this);
		glfwSetFramebufferSizeCallback(m_window, framebufferResizeCallback);
	}

	void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface)
	{
		if (glfwCreateWindowSurface(instance, m_window, nullptr, surface) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create window surface.");
		}
	}

	void Window::framebufferResizeCallback(GLFWwindow *GLFWwindow, int width, int height)
	{
		auto window = reinterpret_cast<Window *>(glfwGetWindowUserPointer(GLFWwindow));
		window->m_framebufferResized = true;
		window->m_width = width;
		window->m_height = height;
	}
}

#include "window.hpp"

namespace LIB_NAMESPACE
{
	Window::Window(const WindowCreateInfo& createInfo):
		m_title(createInfo.title),
		m_width(createInfo.width),
		m_height(createInfo.height)
	{

		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
	}

	Window::~Window()
	{
		glfwDestroyWindow(m_window);

		glfwTerminate();
	}
}
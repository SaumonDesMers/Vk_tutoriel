#pragma once

#include "defines.hpp"

#include <GLFW/glfw3.h>

#include <string>

namespace LIB_NAMESPACE
{

	struct WindowCreateInfo
	{
		std::string title = "Default title";
		int width = 800;
		int height = 600;
	};

	class Window
	{

	public:

		Window(const ft::WindowCreateInfo& createInfo);

		~Window();

		bool shouldClose() const
		{
			return glfwWindowShouldClose(m_window);
		}

	private:

		GLFWwindow* m_window;

		std::string m_title;
		int m_width;
		int m_height;
	};

}
#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "logger.hpp"

namespace ft
{

	class Window
	{

	public:

		Window(std::string title, int width, int height);

		Window(const Window &) = delete;
		Window &operator=(const Window &) = delete;

		~Window();

		bool shouldClose() const { return glfwWindowShouldClose(m_window); }

	private:

		GLFWwindow *m_window;

		std::string m_title;
		int m_width;
		int m_height;

		void init();
	};

}
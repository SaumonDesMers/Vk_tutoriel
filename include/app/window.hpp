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
		VkExtent2D getExtent() const { return {static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height)}; }
		bool wasWindowResized() { return m_framebufferResized; }
		void resetWindowResizedFlag() { m_framebufferResized = false; }
		GLFWwindow *getGLFWwindow() const { return m_window; }

		void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

	private:

		GLFWwindow *m_window;

		std::string m_title;
		int m_width;
		int m_height;

		bool m_framebufferResized = false;

		void init();

		static void framebufferResizeCallback(GLFWwindow *window, int width, int height);
	};

}
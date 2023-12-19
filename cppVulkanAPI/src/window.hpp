#pragma once

#include "defines.hpp"

#define GLFW_INCLUDE_VULKAN
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

		class Surface
		{

		public:

			Surface(VkInstance instance, GLFWwindow* window);
			~Surface();

			VkSurfaceKHR getVk() const { return m_surface; }

		private:

			VkSurfaceKHR m_surface;
			VkInstance m_instance;
		};

		Window(const ft::WindowCreateInfo& createInfo);
		~Window();

		GLFWwindow* getGLFWwindow() const { return m_window; }

		bool shouldClose() const
		{
			return glfwWindowShouldClose(m_window);
		}

		void getFramebufferSize(int* width, int* height) const
		{
			glfwGetFramebufferSize(m_window, width, height);
		}

	private:

		GLFWwindow* m_window;

		std::string m_title;
		int m_width;
		int m_height;
	};

}
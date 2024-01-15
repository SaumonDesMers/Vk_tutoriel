#pragma once

#include "defines.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace LIB_NAMESPACE
{
	class Window
	{

	public:

		class Manager
		{

		public:

			Manager();
			~Manager();

			std::unique_ptr<Window> createWindow(const std::string& title, int width, int height);

			void pollEvents();
			void waitEvents();

			std::vector<const char *> getRequiredInstanceExtensions();
		
		private:
			
		};

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

		Window(const std::string& title, int width, int height);
		~Window();

		GLFWwindow* getGLFWwindow() const { return m_window; }

		bool isResized() const { return m_framebufferResized; }
		void resetResize() { m_framebufferResized = false; }

		bool shouldClose() const
		{
			return glfwWindowShouldClose(m_window);
		}

		void getFramebufferSize(int* width, int* height) const
		{
			glfwGetFramebufferSize(m_window, width, height);
		}

		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

	private:

		GLFWwindow* m_window;

		std::string m_title;
		int m_width;
		int m_height;

		bool m_framebufferResized;
	};
}
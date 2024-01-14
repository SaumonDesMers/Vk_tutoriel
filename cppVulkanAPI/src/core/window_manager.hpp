#pragma once

#include "defines.hpp"
#include "window.hpp"

#include <GLFW/glfw3.h>

#include <memory>
#include <vector>

namespace LIB_NAMESPACE
{
	namespace core
	{

		class WindowManager
		{
		public:

			WindowManager();

			~WindowManager();

			std::unique_ptr<Window> createWindow(const WindowCreateInfo& createInfo);

			void pollEvents();
			void waitEvents();

			std::vector<const char *> getRequiredInstanceExtensions();
			
		};
	}
}
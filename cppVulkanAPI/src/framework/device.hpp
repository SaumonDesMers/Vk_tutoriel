#pragma once

#include "defines.hpp"
#include "window/window.hpp"
#include "core/instance.hpp"

#include <memory>

namespace LIB_NAMESPACE
{
	class Device
	{

	public:

		std::unique_ptr<Window::Manager> windowManager;
		std::unique_ptr<Window> window;

		Device();
		~Device();

	private:

		void createWindow();

	};
}
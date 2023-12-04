#pragma once

#include "defines.hpp"
#include "window.hpp"

#include <GLFW/glfw3.h>

#include <memory>

namespace LIB_NAMESPACE
{

class WindowManager
{
public:

	WindowManager();

	~WindowManager();

	std::unique_ptr<Window> createWindow(const WindowCreateInfo& createInfo);

	void pollEvents();

	
};

}
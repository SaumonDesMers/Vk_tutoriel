#include "device.hpp"

namespace LIB_NAMESPACE
{
	Device::Device()
	{
		createWindow();
	}

	Device::~Device()
	{

	}

	
	void Device::createWindow()
	{
		windowManager = std::make_unique<Window::Manager>();
		window = windowManager->createWindow("Master window", 800, 600);
	}
}
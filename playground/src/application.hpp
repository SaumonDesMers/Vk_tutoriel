#pragma once

#include <cppVulkanAPI.hpp>

#include <stdexcept>
#include <memory>

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

class Application
{

public:

	Application();

	~Application();

	void run();

private:

	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

	std::unique_ptr<ft::WindowManager> m_windowManager;
	std::unique_ptr<ft::Window> m_window;
	std::unique_ptr<ft::Instance> m_instance;

	void init();

	void createWindowManager();
	void createWindow();
	void createInstance();
};
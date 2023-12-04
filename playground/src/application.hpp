#pragma once

#include <cppVulkanAPI.hpp>

#include <stdexcept>
#include <memory>

class Application
{

public:

	Application();

	~Application();

	void run();

private:

	std::unique_ptr<ft::WindowManager> m_windowManager;
	std::unique_ptr<ft::Window> m_window;
	std::unique_ptr<ft::Instance> m_instance;

	void init();

	void createWindowManager();
	void createWindow();
	void createInstance();
};
#include "app/application.hpp"

namespace ft
{

	void Application::run()
	{
		while (!m_window.shouldClose())
		{
			glfwPollEvents();
		}
	}

}
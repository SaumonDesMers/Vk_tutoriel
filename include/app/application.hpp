#pragma once

#include "window.hpp"

namespace ft
{

	class Application
	{

	public:

		void run();
	
	private:

		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		Window m_window{"Vulkan", WIDTH, HEIGHT};

	};

}

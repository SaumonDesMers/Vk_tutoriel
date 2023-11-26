#pragma once

#include "window.hpp"
#include "device.hpp"
#include "renderer.hpp"
#include "game_object.hpp"
#include "descriptors.hpp"

#include "ft_glm/ft_glm.hpp"

#include <memory>
#include <vector>

namespace ft
{

	class Application
	{

	public:

		Application();
		~Application();

		Application(const Application &) = delete;
		Application &operator=(const Application &) = delete;

		void run();
	
	private:

		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		Window m_window{"Vulkan", WIDTH, HEIGHT};
		Device m_device{m_window};
		Renderer m_renderer{m_window, m_device};
		
		std::unique_ptr<DescriptorPool> m_globalPool;
		std::vector<GameObject> m_gameObjects;

		void loadGameObjects();

	};

}

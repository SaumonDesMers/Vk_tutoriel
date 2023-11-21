#pragma once

#include "window.hpp"
#include "pipeline.hpp"
#include "device.hpp"

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
		Device m_device{m_window};
		Pipeline m_pipeline{
			m_device,
			"shaders/vert.spv",
			"shaders/frag.spv",
			Pipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)
		};

	};

}

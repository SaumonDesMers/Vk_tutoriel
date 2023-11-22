#pragma once

#include "window.hpp"
#include "pipeline.hpp"
#include "device.hpp"
#include "swap_chain.hpp"
#include "model.hpp"

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

		void loadModels();
	
	private:

		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		Window m_window{"Vulkan", WIDTH, HEIGHT};
		Device m_device{m_window};
		std::unique_ptr<SwapChain> m_swapChain;
		std::unique_ptr<Pipeline> m_pipeline;
		VkPipelineLayout m_pipelineLayout;
		std::vector<VkCommandBuffer> m_commandBuffers;
		std::unique_ptr<Model> m_model;

		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void freeCommandBuffers();
		void drawFrame();
		void recreateSwapChain();
		void recordCommandBuffer(int imageIndex);

	};

}

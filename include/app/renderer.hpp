#pragma once

#include "window.hpp"
#include "device.hpp"
#include "swap_chain.hpp"

#include "ft_glm/ft_glm.hpp"

#include <memory>
#include <vector>
#include <cassert>

namespace ft
{

	class Renderer
	{

	public:

		Renderer(Window &window, Device &device);
		~Renderer();

		Renderer(const Renderer &) = delete;
		Renderer &operator=(const Renderer &) = delete;

		VkRenderPass getSwapChainRenderPass() const { return m_swapChain->getRenderPass(); }
		float getAspectRatio() const { return m_swapChain->extentAspectRatio(); }
		bool isFrameInProgress() const { return m_isFrameStarted; }

		VkCommandBuffer getCurrentCommandBuffer() const
		{
			assert(m_isFrameStarted && "Cannot get command buffer when frame not in progress");
			return m_commandBuffers[m_currentFrameIndex];
		}

		int getFrameIndex() const
		{
			assert(m_isFrameStarted && "Cannot get frame index when frame not in progress");
			return m_currentFrameIndex;
		}

		VkCommandBuffer beginFrame();
		void endFrame();

		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);
	
	private:

		Window &m_window;
		Device &m_device;
		std::unique_ptr<SwapChain> m_swapChain;
		std::vector<VkCommandBuffer> m_commandBuffers;

		uint32_t m_currentImageIndex;
		int m_currentFrameIndex;
		bool m_isFrameStarted;

		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();

	};

}

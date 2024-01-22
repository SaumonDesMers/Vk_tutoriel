#pragma once

#include "defines.hpp"
#include "device.hpp"
#include "swapchain.hpp"
#include "descriptor/descriptor.hpp"
#include "descriptor/uniform_buffer.hpp"
#include "descriptor/texture.hpp"
#include "command.hpp"
#include "pipeline.hpp"
#include "memory/image.hpp"
#include "memory/buffer.hpp"
#include "core/image/sampler.hpp"
#include "core/sync_object.hpp"
#include "object/mesh.hpp"

#include <glm/glm.hpp>

#include <stdexcept>
#include <memory>
#include <optional>
#include <vector>
#include <map>
#include <chrono>

struct ViewProj_UBO {
	glm::mat4 view;
	glm::mat4 proj;
};

namespace LIB_NAMESPACE
{
	class RenderAPI
	{

	public:

		RenderAPI();
		~RenderAPI();

		Mesh::ID loadModel(const std::string& filename);
		Pipeline::ID createPipeline(Pipeline::CreateInfo& createInfo);
		Descriptor::ID createDescriptor(VkDescriptorSetLayoutBinding layoutBinding);
		Texture::ID loadTexture(Texture::CreateInfo& createInfo);
		UniformBuffer::ID createUniformBuffer(UniformBuffer::CreateInfo& createInfo);

		// function to start recording a command buffer
		void startDraw();
		// function to start a render pass
		void startRendering();
		// function to do the actual drawing
		void bindPipeline(Pipeline::ID pipelineID);
		void bindMesh(Mesh::ID meshID);

		// function to end a render pass
		void endRendering();
		// function to end recording a command buffer
		void endDraw();

	// private:

		const std::vector<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};

		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		std::unique_ptr<ft::Device> m_device;

		std::unique_ptr<ft::Swapchain> m_swapchain;

		std::map<Descriptor::ID, std::unique_ptr<ft::Descriptor>> m_descriptorMap;

		std::unique_ptr<ft::Command> m_command;
		std::vector<VkCommandBuffer> m_vkCommandBuffers;

		std::map<Pipeline::ID, std::unique_ptr<ft::Pipeline>> m_pipelineMap;

		std::unique_ptr<ft::Image> m_colorImage;
		std::unique_ptr<ft::Image> m_depthImage;

		std::vector<std::unique_ptr<ft::core::Semaphore>> m_imageAvailableSemaphores;
		std::vector<std::unique_ptr<ft::core::Semaphore>> m_renderFinishedSemaphores;
		std::vector<std::unique_ptr<ft::core::Semaphore>> m_swapchainUpdatedSemaphores;
		std::vector<std::unique_ptr<ft::core::Fence>> m_inFlightFences;

		Mesh::ID m_maxMeshID = 0;
		std::map<Mesh::ID, std::unique_ptr<ft::Mesh>> m_meshMap;

		uint32_t m_mipLevels;
		Texture::ID m_maxTextureID;
		std::map<Texture::ID, std::unique_ptr<Texture>> m_textureMap;

		std::map<UniformBuffer::ID, std::unique_ptr<ft::UniformBuffer>> m_uniformBufferMap;

		bool m_framebufferResized = false;
		
		uint32_t m_currentFrame = 0;

		void init();

		void createDevice();
		void createSwapchain();
		void recreateSwapChain();
		void createDescriptor();
		void createCommandPool();
		void createColorResources();
		void createDepthResources();
		void createSyncObjects();

		VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
		VkFormat findDepthFormat();
		bool hasStencilComponent(VkFormat format);

		void generateMipmaps(VkImage image, VkFormat format, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
		void copyRenderedImageToSwapchainImage(uint32_t swapchainImageIndex);
	};
}
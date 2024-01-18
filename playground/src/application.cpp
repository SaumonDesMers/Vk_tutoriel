#include "application.hpp"
#include "logger.hpp"
#include "vertex.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vulkan/vulkan.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <set>
#include <chrono>

Application::Application()
{
	
}

Application::~Application()
{
	
}

void Application::run()
{
	init();

	while (m_device->window->shouldClose() == false)
	{
		m_device->windowManager->pollEvents();
		drawFrame();
	}

	m_device->device->waitIdle();
}

void Application::init()
{
	createDevice();
	createSwapchain();
	createRenderPass();
	createDescriptor();
	createGraphicsPipeline();
	createCommandPool();
	createColorResources();
	createDepthResources();
	createFramebuffers();
	createTextureImage();
	createTextureImageView();
	createTextureSampler();
	loadModel();
	createVertexBuffer();
	createIndexBuffer();
	createUniformBuffers();
	updateDescriptorSets();
	createCommandBuffer();
	createSyncObjects();

	FT_INFO("Application initialized");
}


void Application::createDevice()
{
	m_device = std::make_unique<ft::Device>();
}

void Application::createSwapchain()
{
	ft::Swapchain::CreateInfo swapchainInfo = {};
	swapchainInfo.surface = m_device->surface->getVk();
	swapchainInfo.supportDetails = m_device->querySwapChainSupport(m_device->physicalDevice->getVk());
	
	int width, height;
	m_device->window->getFramebufferSize(&width, &height);
	swapchainInfo.frameBufferExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

	swapchainInfo.queueFamilyIndices = m_device->findQueueFamilies(m_device->physicalDevice->getVk());

	swapchainInfo.oldSwapchain = VK_NULL_HANDLE;

	m_swapchain = std::make_unique<ft::Swapchain>(m_device->device->getVk(), swapchainInfo);
}

void Application::recreateSwapChain()
{
	int width = 0, height = 0;
    m_device->window->getFramebufferSize(&width, &height);
    while (width == 0 || height == 0)
	{
        m_device->window->getFramebufferSize(&width, &height);
        m_device->windowManager->waitEvents();
    }

	m_device->device->waitIdle();


	m_swapchainFramebuffers.clear();

	m_colorImageView.reset();
	m_colorImageMemory.reset();
	m_colorImage.reset();

	m_depthImageView.reset();
	m_depthImageMemory.reset();
	m_depthImage.reset();

	m_swapchain.reset();


	createSwapchain();
	createColorResources();
	createDepthResources();
	createFramebuffers();
}

void Application::createRenderPass()
{
	VkAttachmentDescription colorAttachment{};
    colorAttachment.format = m_swapchain->swapchain->getImageFormat();
    colorAttachment.samples = m_device->msaaSamples;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


	VkAttachmentDescription depthAttachment{};
	depthAttachment.format = findDepthFormat();
	depthAttachment.samples = m_device->msaaSamples;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef{};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;


	VkAttachmentDescription colorAttachmentResolve{};
    colorAttachmentResolve.format = m_swapchain->swapchain->getImageFormat();
    colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentResolveRef{};
    colorAttachmentResolveRef.attachment = 2;
    colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;
	subpass.pResolveAttachments = &colorAttachmentResolveRef;

	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	std::array<VkAttachmentDescription, 3> attachments = { colorAttachment, depthAttachment, colorAttachmentResolve };
	ft::core::RenderPass::CreateInfo renderPassInfo = {};
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	m_renderPass = std::make_unique<ft::core::RenderPass>(m_device->device->getVk(), renderPassInfo);
}

void Application::createDescriptor()
{
	VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;


	ft::Descriptor::CreateInfo descriptorInfo{};
	descriptorInfo.bindings = { uboLayoutBinding, samplerLayoutBinding };
	descriptorInfo.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	m_descriptor = std::make_unique<ft::Descriptor>(m_device->device->getVk(), descriptorInfo);
}

void Application::createGraphicsPipeline()
{
	ft::Pipeline::CreateInfo pipelineInfo = {};

	pipelineInfo.vertexShaderPath = "playground/shaders/simple_shader.vert.spv";
	pipelineInfo.fragmentShaderPath = "playground/shaders/simple_shader.frag.spv";

	pipelineInfo.msaaSamples = m_device->msaaSamples;

	pipelineInfo.descriptorSetLayouts = { m_descriptor->layout };

	pipelineInfo.renderPass = m_renderPass->getVk();

	m_graphicPipeline = std::make_unique<ft::Pipeline>(m_device->device->getVk(), pipelineInfo);
}

void Application::createFramebuffers()
{
	m_swapchainFramebuffers.resize(m_swapchain->imageViews.size());

	for (size_t i = 0; i < m_swapchain->imageViews.size(); i++) {
		std::array<VkImageView, 3> attachments = {
			m_colorImageView->getVk(),
			m_depthImageView->getVk(),
			m_swapchain->imageViews[i]->getVk()
		};

		ft::core::Framebuffer::CreateInfo framebufferInfo{};
		framebufferInfo.renderPass = m_renderPass->getVk();
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = m_swapchain->swapchain->getExtent().width;
		framebufferInfo.height = m_swapchain->swapchain->getExtent().height;
		framebufferInfo.layers = 1;

		m_swapchainFramebuffers[i] = std::make_unique<ft::core::Framebuffer>(m_device->device->getVk(), framebufferInfo);
	}
}

void Application::createCommandPool()
{
	ft::Queue::FamilyIndices queueFamilyIndices = m_device->findQueueFamilies(m_device->physicalDevice->getVk());

	ft::Command::CreateInfo commandInfo{};
	commandInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
	commandInfo.queue = m_device->graphicsQueue->getVk();
	commandInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	m_command = std::make_unique<ft::Command>(m_device->device->getVk(), commandInfo);
}

void Application::createColorResources()
{
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.format = m_swapchain->swapchain->getImageFormat();
	imageInfo.extent.width = m_swapchain->swapchain->getExtent().width;
	imageInfo.extent.height = m_swapchain->swapchain->getExtent().height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage =
		VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT |
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = m_device->msaaSamples;

	m_colorImage = std::make_unique<ft::core::Image>(m_device->device->getVk(), imageInfo);

	VkMemoryRequirements memRequirements = m_colorImage->getMemoryRequirements();

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = ft::core::DeviceMemory::findMemoryType(
		m_device->physicalDevice->getVk(),
		memRequirements.memoryTypeBits,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);

	m_colorImageMemory = std::make_unique<ft::core::DeviceMemory>(m_device->device->getVk(), allocInfo);

	m_colorImage->bindMemory(m_colorImageMemory->getVk());

	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = m_colorImage->getVk();
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = m_swapchain->swapchain->getImageFormat();
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	m_colorImageView = std::make_unique<ft::core::ImageView>(m_device->device->getVk(), viewInfo);

}

void Application::createDepthResources()
{
	VkFormat depthFormat = findDepthFormat();

	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = m_swapchain->swapchain->getExtent().width;
	imageInfo.extent.height = m_swapchain->swapchain->getExtent().height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = depthFormat;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = m_device->msaaSamples;

	m_depthImage = std::make_unique<ft::core::Image>(m_device->device->getVk(), imageInfo);

	VkMemoryRequirements memRequirements = m_depthImage->getMemoryRequirements();

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = ft::core::DeviceMemory::findMemoryType(
		m_device->physicalDevice->getVk(),
		memRequirements.memoryTypeBits,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);

	m_depthImageMemory = std::make_unique<ft::core::DeviceMemory>(m_device->device->getVk(), allocInfo);

	m_depthImage->bindMemory(m_depthImageMemory->getVk());

	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = m_depthImage->getVk();
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = depthFormat;

	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	m_depthImageView = std::make_unique<ft::core::ImageView>(m_device->device->getVk(), viewInfo);

}

void Application::createTextureImage()
{
	std::string modelPath = "playground/textures/viking_room.ppm";

	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load(modelPath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	VkDeviceSize imageSize = texWidth * texHeight * 4;

	if (pixels == nullptr)
	{
		throw std::runtime_error("failed to load texture: " + modelPath);
	}

	VkBufferCreateInfo stagingBufferInfo{};
	stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	stagingBufferInfo.size = imageSize;
	stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	stagingBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	ft::core::Buffer stagingBuffer(m_device->device->getVk(), stagingBufferInfo);

	VkMemoryRequirements stagingMemRequirements = stagingBuffer.getMemoryRequirements();

	VkMemoryAllocateInfo stagingMemoryInfo{};
	stagingMemoryInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	stagingMemoryInfo.allocationSize = stagingMemRequirements.size;
	stagingMemoryInfo.memoryTypeIndex = ft::core::DeviceMemory::findMemoryType(
		m_device->physicalDevice->getVk(),
		stagingMemRequirements.memoryTypeBits,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);

	ft::core::DeviceMemory stagingBufferMemory(m_device->device->getVk(), stagingMemoryInfo);

	vkBindBufferMemory(m_device->device->getVk(), stagingBuffer.getVk(), stagingBufferMemory.getVk(), 0);

	stagingBufferMemory.map();
	stagingBufferMemory.write((void*)pixels, imageSize);
	stagingBufferMemory.unmap();

	stbi_image_free(pixels);


	m_mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = static_cast<uint32_t>(texWidth);
	imageInfo.extent.height = static_cast<uint32_t>(texHeight);
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = m_mipLevels;
	imageInfo.arrayLayers = 1;
	imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = 
		VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
		VK_IMAGE_USAGE_TRANSFER_DST_BIT |
		VK_IMAGE_USAGE_SAMPLED_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

	m_textureImage = std::make_unique<ft::core::Image>(m_device->device->getVk(), imageInfo);

	VkMemoryRequirements imageMemRequirements = m_textureImage->getMemoryRequirements();

	VkMemoryAllocateInfo imageMemoryInfo{};
	imageMemoryInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	imageMemoryInfo.allocationSize = imageMemRequirements.size;
	imageMemoryInfo.memoryTypeIndex = ft::core::DeviceMemory::findMemoryType(
		m_device->physicalDevice->getVk(),
		imageMemRequirements.memoryTypeBits,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);

	m_textureImageMemory = std::make_unique<ft::core::DeviceMemory>(m_device->device->getVk(), imageMemoryInfo);

	m_textureImage->bindMemory(m_textureImageMemory->getVk());

	transitionImageLayout(
		m_textureImage->getVk(),
		VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		m_mipLevels
	);

	copyBufferToImage(
		stagingBuffer.getVk(),
		m_textureImage->getVk(),
		static_cast<uint32_t>(texWidth),
		static_cast<uint32_t>(texHeight)
	);

	//transitioned to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL while generating mipmaps
	generateMipmaps(
		m_textureImage->getVk(),
		VK_FORMAT_R8G8B8A8_SRGB,
		texWidth,
		texHeight,
		m_mipLevels
	);

}

void Application::createTextureImageView()
{
	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = m_textureImage->getVk();
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;

	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = m_mipLevels;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	m_textureImageView = std::make_unique<ft::core::ImageView>(m_device->device->getVk(), viewInfo);
}

void Application::createTextureSampler()
{
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;

	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	VkPhysicalDeviceProperties properties{};
	m_device->physicalDevice->getProperties(&properties);

	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

	samplerInfo.unnormalizedCoordinates = VK_FALSE;

	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = static_cast<float>(m_mipLevels);

	m_textureSampler = std::make_unique<ft::core::Sampler>(m_device->device->getVk(), samplerInfo);
}

void Application::loadModel()
{
	std::string modelPath = "playground/models/viking_room.obj";

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warning, error;

	if (tinyobj::LoadObj(&attrib, &shapes, &materials, &warning, &error, modelPath.c_str()) == false)
	{
		throw std::runtime_error(warning + error);
	}

	std::unordered_map<Vertex, uint32_t> uniqueVertices = {};

	for (const auto& shape : shapes)
	{
		for (const auto& index : shape.mesh.indices)
		{
			Vertex vertex = {};

			vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			vertex.texCoord = {
				attrib.texcoords[2 * index.texcoord_index + 0],
				1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
			};

			vertex.color = { 1.0f, 1.0f, 1.0f };

			if (uniqueVertices.count(vertex) == 0)
			{
				uniqueVertices[vertex] = static_cast<uint32_t>(m_vertices.size());
				m_vertices.push_back(vertex);
			}

			m_indices.push_back(uniqueVertices[vertex]);
		}
	}

}

void Application::createVertexBuffer()
{
	VkDeviceSize bufferSize = sizeof(m_vertices[0]) * m_vertices.size();


	VkBufferCreateInfo stagingBufferInfo{};
	stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	stagingBufferInfo.size = bufferSize;
	stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	stagingBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	ft::core::Buffer stagingBuffer(m_device->device->getVk(), stagingBufferInfo);

	VkMemoryRequirements memRequirements = stagingBuffer.getMemoryRequirements();
	
	VkMemoryAllocateInfo stagingMemoryInfo{};
	stagingMemoryInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	stagingMemoryInfo.allocationSize = memRequirements.size;
	stagingMemoryInfo.memoryTypeIndex = ft::core::DeviceMemory::findMemoryType(
		m_device->physicalDevice->getVk(),
		memRequirements.memoryTypeBits,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);

	ft::core::DeviceMemory stagingBufferMemory(m_device->device->getVk(), stagingMemoryInfo);

	vkBindBufferMemory(m_device->device->getVk(), stagingBuffer.getVk(), stagingBufferMemory.getVk(), 0);


	stagingBufferMemory.map();
	stagingBufferMemory.write((void*)m_vertices.data(), bufferSize);
	stagingBufferMemory.unmap();


	VkBufferCreateInfo vertexBufferInfo{};
	vertexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	vertexBufferInfo.size = bufferSize;
	vertexBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	vertexBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	m_vertexBuffer = std::make_unique<ft::core::Buffer>(m_device->device->getVk(), vertexBufferInfo);

	VkMemoryRequirements vertexMemRequirements = m_vertexBuffer->getMemoryRequirements();

	VkMemoryAllocateInfo vertexMemoryInfo{};
	vertexMemoryInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	vertexMemoryInfo.allocationSize = vertexMemRequirements.size;
	vertexMemoryInfo.memoryTypeIndex = ft::core::DeviceMemory::findMemoryType(
		m_device->physicalDevice->getVk(),
		vertexMemRequirements.memoryTypeBits,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);

	m_vertexBufferMemory = std::make_unique<ft::core::DeviceMemory>(m_device->device->getVk(), vertexMemoryInfo);

	vkBindBufferMemory(m_device->device->getVk(), m_vertexBuffer->getVk(), m_vertexBufferMemory->getVk(), 0);


	copyBufferToBuffer(stagingBuffer.getVk(), m_vertexBuffer->getVk(), bufferSize);
}

void Application::createIndexBuffer()
{
	VkDeviceSize bufferSize = sizeof(m_indices[0]) * m_indices.size();

	VkBufferCreateInfo stagingBufferInfo{};
	stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	stagingBufferInfo.size = bufferSize;
	stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	stagingBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	ft::core::Buffer stagingBuffer(m_device->device->getVk(), stagingBufferInfo);

	VkMemoryRequirements stagingMemRequirements = stagingBuffer.getMemoryRequirements();

	VkMemoryAllocateInfo stagingMemoryInfo{};
	stagingMemoryInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	stagingMemoryInfo.allocationSize = stagingMemRequirements.size;
	stagingMemoryInfo.memoryTypeIndex = ft::core::DeviceMemory::findMemoryType(
		m_device->physicalDevice->getVk(),
		stagingMemRequirements.memoryTypeBits,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);

	ft::core::DeviceMemory stagingBufferMemory(m_device->device->getVk(), stagingMemoryInfo);

	vkBindBufferMemory(m_device->device->getVk(), stagingBuffer.getVk(), stagingBufferMemory.getVk(), 0);


	stagingBufferMemory.map();
	stagingBufferMemory.write((void*)m_indices.data(), bufferSize);
	stagingBufferMemory.unmap();


	VkBufferCreateInfo indexBufferInfo{};
	indexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	indexBufferInfo.size = bufferSize;
	indexBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	indexBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	m_indexBuffer = std::make_unique<ft::core::Buffer>(m_device->device->getVk(), indexBufferInfo);

	VkMemoryRequirements indexMemRequirements = m_indexBuffer->getMemoryRequirements();

	VkMemoryAllocateInfo indexMemoryInfo{};
	indexMemoryInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	indexMemoryInfo.allocationSize = indexMemRequirements.size;
	indexMemoryInfo.memoryTypeIndex = ft::core::DeviceMemory::findMemoryType(
		m_device->physicalDevice->getVk(),
		indexMemRequirements.memoryTypeBits,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);

	m_indexBufferMemory = std::make_unique<ft::core::DeviceMemory>(m_device->device->getVk(), indexMemoryInfo);

	vkBindBufferMemory(m_device->device->getVk(), m_indexBuffer->getVk(), m_indexBufferMemory->getVk(), 0);


	copyBufferToBuffer(stagingBuffer.getVk(), m_indexBuffer->getVk(), bufferSize);
}

void Application::createUniformBuffers()
{
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    m_uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	m_uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = bufferSize;
		bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		m_uniformBuffers[i] = std::make_unique<ft::core::Buffer>(m_device->device->getVk(), bufferInfo);

		VkMemoryRequirements memRequirements = m_uniformBuffers[i]->getMemoryRequirements();

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = ft::core::DeviceMemory::findMemoryType(
			m_device->physicalDevice->getVk(),
			memRequirements.memoryTypeBits,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		);

		m_uniformBuffersMemory[i] = std::make_unique<ft::core::DeviceMemory>(m_device->device->getVk(), allocInfo);

		vkBindBufferMemory(m_device->device->getVk(), m_uniformBuffers[i]->getVk(), m_uniformBuffersMemory[i]->getVk(), 0);


		m_uniformBuffersMemory[i]->map();
    }
}

void Application::updateDescriptorSets()
{
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = m_uniformBuffers[i]->getVk();
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = m_textureImageView->getVk();
		imageInfo.sampler = m_textureSampler->getVk();

		std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = m_descriptor->sets[i];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = m_descriptor->sets[i];
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(
			m_device->device->getVk(),
			static_cast<uint32_t>(descriptorWrites.size()),
			descriptorWrites.data(),
			0, nullptr
		);
	}

}

void Application::createCommandBuffer()
{
	m_vkCommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		m_vkCommandBuffers[i] = m_command->allocateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	}
}

void Application::createSyncObjects()
{
	m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	ft::core::Semaphore::CreateInfo semaphoreInfo{};

	ft::core::Fence::CreateInfo fenceInfo{};
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		m_imageAvailableSemaphores[i] = std::make_unique<ft::core::Semaphore>(m_device->device->getVk(), semaphoreInfo);
		m_renderFinishedSemaphores[i] = std::make_unique<ft::core::Semaphore>(m_device->device->getVk(), semaphoreInfo);
		m_inFlightFences[i] = std::make_unique<ft::core::Fence>(m_device->device->getVk(), fenceInfo);
	}

}


VkFormat Application::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
	for (const auto& format : candidates)
	{
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(m_device->physicalDevice->getVk(), format, &props);

		if (
			tiling == VK_IMAGE_TILING_LINEAR &&
			(props.linearTilingFeatures & features) == features
		)
		{
			return format;
		}
		else if (
			tiling == VK_IMAGE_TILING_OPTIMAL &&
			(props.optimalTilingFeatures & features) == features
		)
		{
			return format;
		}
	}

	throw std::runtime_error("failed to find supported format.");
}

VkFormat Application::findDepthFormat() {
    return findSupportedFormat(
        {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}

bool Application::hasStencilComponent(VkFormat format) {
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}


void Application::copyBufferToBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	VkCommandBuffer commandBuffer = m_command->beginSingleTimeCommands();

	VkBufferCopy copyRegion{};
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	m_command->endSingleTimeCommands(commandBuffer);
}

void Application::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
	VkCommandBuffer commandBuffer = m_command->beginSingleTimeCommands();

	VkBufferImageCopy region{};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	region.imageOffset = {0, 0, 0};
	region.imageExtent = {
		width,
		height,
		1
	};

	vkCmdCopyBufferToImage(
		commandBuffer,
		buffer,
		image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&region
	);

	m_command->endSingleTimeCommands(commandBuffer);
}

void Application::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels)
{
	VkCommandBuffer commandBuffer = m_command->beginSingleTimeCommands();

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;

	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = mipLevels;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (
		oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
		newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
	)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (
		oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
		newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
	)
	{
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else
	{
		throw std::invalid_argument("unsupported image layout transition.");
	}

	vkCmdPipelineBarrier(
		commandBuffer,
		sourceStage, destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);

	m_command->endSingleTimeCommands(commandBuffer);
}

void Application::generateMipmaps(VkImage image, VkFormat format, int32_t texWidth, int32_t texHeight, uint32_t mipLevels)
{
	// Check if image format supports linear blitting
	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties(m_device->physicalDevice->getVk(), format, &formatProperties);

	VkCommandBuffer commandBuffer = m_command->beginSingleTimeCommands();

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.image = image;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.subresourceRange.levelCount = 1;

	int32_t mipWidth = texWidth;
	int32_t mipHeight = texHeight;

	for (uint32_t i = 1; i < mipLevels; i++)
	{
		barrier.subresourceRange.baseMipLevel = i - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

		vkCmdPipelineBarrier(
			commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		VkImageBlit blit{};
		blit.srcOffsets[0] = {0, 0, 0};
		blit.srcOffsets[1] = {mipWidth, mipHeight, 1};
		blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.srcSubresource.mipLevel = i - 1;
		blit.srcSubresource.baseArrayLayer = 0;
		blit.srcSubresource.layerCount = 1;

		blit.dstOffsets[0] = {0, 0, 0};
		blit.dstOffsets[1] = {
			mipWidth > 1 ? mipWidth / 2 : 1,
			mipHeight > 1 ? mipHeight / 2 : 1,
			1
		};
		blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.dstSubresource.mipLevel = i;
		blit.dstSubresource.baseArrayLayer = 0;
		blit.dstSubresource.layerCount = 1;

		vkCmdBlitImage(
			commandBuffer,
			image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &blit,
			VK_FILTER_LINEAR
		);

		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(
			commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		if (mipWidth > 1) mipWidth /= 2;
		if (mipHeight > 1) mipHeight /= 2;
	}

	barrier.subresourceRange.baseMipLevel = mipLevels - 1;
	barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	vkCmdPipelineBarrier(
		commandBuffer,
		VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);

	m_command->endSingleTimeCommands(commandBuffer);
}


void Application::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
	ft::core::CommandBuffer::BeginInfo beginInfo = {};

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = m_renderPass->getVk();
	renderPassInfo.framebuffer = m_swapchainFramebuffers[imageIndex]->getVk();
	renderPassInfo.renderArea.offset = {0, 0};
	renderPassInfo.renderArea.extent = m_swapchain->swapchain->getExtent();

	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
	clearValues[1].depthStencil = {1.0f, 0};

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicPipeline->pipeline->getVk());

	vkCmdBindDescriptorSets(
		commandBuffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		m_graphicPipeline->layout->getVk(),
		0, 1,
		&m_descriptor->sets[m_currentFrame],
		0, nullptr
	);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(m_swapchain->swapchain->getExtent().width);
	viewport.height = static_cast<float>(m_swapchain->swapchain->getExtent().height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = {0, 0};
	scissor.extent = m_swapchain->swapchain->getExtent();
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	VkBuffer vertexBuffers[] = {m_vertexBuffer->getVk()};
	VkDeviceSize offsets[] = {0};
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

	vkCmdBindIndexBuffer(commandBuffer, m_indexBuffer->getVk(), 0, VK_INDEX_TYPE_UINT32);

	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_indices.size()), 1, 0, 0, 0);

	vkCmdEndRenderPass(commandBuffer);

	vkEndCommandBuffer(commandBuffer);
}

void Application::drawFrame()
{
	m_inFlightFences[m_currentFrame]->wait();

	uint32_t imageIndex;
	VkResult result = m_swapchain->swapchain->acquireNextImage(UINT64_MAX, m_imageAvailableSemaphores[m_currentFrame]->getVk(), VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		recreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	m_inFlightFences[m_currentFrame]->reset();

	vkResetCommandBuffer(m_vkCommandBuffers[m_currentFrame], 0);
	recordCommandBuffer(m_vkCommandBuffers[m_currentFrame], imageIndex);

	updateUniformBuffer(m_currentFrame);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = {m_imageAvailableSemaphores[m_currentFrame]->getVk()};
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	VkCommandBuffer commandBuffers[] = {m_vkCommandBuffers[m_currentFrame]};
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = commandBuffers;

	VkSemaphore signalSemaphores[] = {m_renderFinishedSemaphores[m_currentFrame]->getVk()};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	m_device->graphicsQueue->submit(1, &submitInfo, m_inFlightFences[m_currentFrame]->getVk());

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = {m_swapchain->swapchain->getVk()};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;

	result = vkQueuePresentKHR(m_device->presentQueue->getVk(), &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_framebufferResized)
	{
		m_framebufferResized = false;
		recreateSwapChain();
	}
	else if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to present swap chain image.");
	}

	m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Application::updateUniformBuffer(uint32_t currentImage)
{
	static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	UniformBufferObject ubo{};
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), m_swapchain->swapchain->getExtent().width / (float) m_swapchain->swapchain->getExtent().height, 0.1f, 10.0f);

	ubo.proj[1][1] *= -1;

	m_uniformBuffersMemory[currentImage]->write(&ubo, sizeof(ubo));
}

#include "application.hpp"
#include "logger.hpp"
#include "vertex.hpp"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vulkan/vulkan.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

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

	while (m_window->shouldClose() == false)
	{
		m_windowManager->pollEvents();
		drawFrame();
	}

	m_device->waitIdle();
}

void Application::init()
{
	createWindowManager();
	createWindow();
	createInstance();
	setupDebugMessenger();
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
	createSwapChain();
	createSwapchainImageViews();
	createRenderPass();
	createDescriptorSetLayout();
	createGraphicsPipeline();
	createFramebuffers();
	createCommandPool();
	createTextureImage();
	createTextureImageView();
	createTextureSampler();
	createVertexBuffer();
	createIndexBuffer();
	createUniformBuffers();
	createDescriptorPool();
	createDescriptorSets();
	createCommandBuffer();
	createSyncObjects();

	FT_INFO("Application initialized");
}

void Application::createWindowManager()
{
	m_windowManager = std::make_unique<ft::WindowManager>();
}

void Application::createWindow()
{
	ft::WindowCreateInfo createInfo = {};
	createInfo.title = "Playground";
	createInfo.width = 800;
	createInfo.height = 600;

	m_window = m_windowManager->createWindow(createInfo);
}

void Application::createInstance()
{
	if (enableValidationLayers && ft::Instance::checkValidationLayerSupport(validationLayers) == false)
	{
		throw std::runtime_error("Validation layers requested, but not available");
	}

	ft::ApplicationInfo appInfo = {};
	appInfo.pApplicationName = "Playground";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "Playground Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_2;

	ft::Instance::CreateInfo createInfo = {};
	createInfo.pApplicationInfo = &appInfo;

	std::vector<const char*> extensions = getRequiredExtensions();

	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	ft::DebugMessenger::CreateInfo debugCreateInfo = {};
	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		populateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = &debugCreateInfo;
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	m_instance = std::make_unique<ft::Instance>(createInfo);
}

void Application::setupDebugMessenger()
{
	if (enableValidationLayers == false)
	{
		return;
	}

	ft::DebugMessenger::CreateInfo createInfo = {};
	populateDebugMessengerCreateInfo(createInfo);

	m_debugMessenger = std::make_unique<ft::DebugMessenger>(m_instance.get(), &createInfo);
}

void Application::createSurface()
{
	m_surface = std::make_unique<ft::Window::Surface>(m_instance->getVk(), m_window->getGLFWwindow());
}

void Application::pickPhysicalDevice()
{
	std::vector<VkPhysicalDevice> physicalDevices = m_instance->getPhysicalDevices();

	if (physicalDevices.empty())
	{
		throw std::runtime_error("Failed to find GPUs with Vulkan support");
	}

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	for (const auto& phyDev : physicalDevices)
	{
		if (isDeviceSuitable(phyDev))
		{
			physicalDevice = phyDev;
			break;
		}
	}

	if (physicalDevice == VK_NULL_HANDLE)
	{
		throw std::runtime_error("Failed to find a suitable GPU");
	}

	m_physicalDevice = std::make_unique<ft::PhysicalDevice>(physicalDevice);
}

void Application::createLogicalDevice()
{
	QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice->getVk());

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		ft::Queue::CreateInfo queueCreateInfo = {};
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	ft::PhysicalDevice::Features deviceFeatures = {};
	deviceFeatures.samplerAnisotropy = VK_TRUE;

	ft::Device::CreateInfo createInfo = {};
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();

	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	m_device = std::make_unique<ft::Device>(m_physicalDevice->getVk(), createInfo);

	m_graphicsQueue = std::make_unique<ft::Queue>(m_device->getVk(), indices.graphicsFamily.value());
	m_presentQueue = std::make_unique<ft::Queue>(m_device->getVk(), indices.presentFamily.value());
}

void Application::createSwapChain()
{
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(m_physicalDevice->getVk());

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

	if (
		swapChainSupport.capabilities.maxImageCount > 0
		&& imageCount > swapChainSupport.capabilities.maxImageCount
	)
	{
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	ft::Swapchain::CreateInfo createInfo = {};
	createInfo.surface = m_surface->getVk();
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice->getVk());
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily != indices.presentFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	createInfo.oldSwapchain = VK_NULL_HANDLE;

	m_swapchain = std::make_unique<ft::Swapchain>(m_device->getVk(), createInfo);
}

void Application::recreateSwapChain()
{
	int width = 0, height = 0;
    m_window->getFramebufferSize(&width, &height);
    while (width == 0 || height == 0)
	{
        m_window->getFramebufferSize(&width, &height);
        m_windowManager->waitEvents();
    }

	m_device->waitIdle();

	m_swapchainFramebuffers.clear();
	m_swapchainImageViews.clear();
	m_swapchain.reset();

	createSwapChain();
	createSwapchainImageViews();
	createFramebuffers();
}

void Application::createSwapchainImageViews()
{
	m_swapchainImageViews.resize(m_swapchain->getImageCount());

	for (size_t i = 0; i < m_swapchain->getImageCount(); i++)
	{
		ft::ImageView::CreateInfo createInfo = {};
		createInfo.image = m_swapchain->getImage(i);
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = m_swapchain->getImageFormat();
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		m_swapchainImageViews[i] = std::make_unique<ft::ImageView>(m_device->getVk(), createInfo);
	}
}

void Application::createRenderPass()
{
	VkAttachmentDescription colorAttachment{};
    colorAttachment.format = m_swapchain->getImageFormat();
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	ft::RenderPass::CreateInfo renderPassInfo = {};
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	m_renderPass = std::make_unique<ft::RenderPass>(m_device->getVk(), renderPassInfo);
}

void Application::createDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &uboLayoutBinding;

	m_descriptorSetLayout = std::make_unique<ft::DescriptorSetLayout>(m_device->getVk(), layoutInfo);
}

void Application::createGraphicsPipeline()
{
	ft::ShaderModule vertShaderModule(m_device->getVk(), "playground/shaders/simple_shader.vert.spv");
	ft::ShaderModule fragShaderModule(m_device->getVk(), "playground/shaders/simple_shader.frag.spv");

	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule.getVk();
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule.getVk();
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };


	std::vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};
	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();


	auto bindingDescription = Vertex::getBindingDescription();
	auto attributeDescriptions = Vertex::getAttributeDescriptions();

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();


	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;


	// dynamicly set in draw call
	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;


	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;


	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;


	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &m_descriptorSetLayout->getVkRef();

	m_pipelineLayout = std::make_unique<ft::PipelineLayout>(m_device->getVk(), pipelineLayoutInfo);


	ft::Pipeline::CreateInfo pipelineInfo = {};
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = nullptr; // Optional
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.layout = m_pipelineLayout->getVk();
	pipelineInfo.renderPass = m_renderPass->getVk();
	pipelineInfo.subpass = 0;

	m_graphicPipeline = std::make_unique<ft::Pipeline>(m_device->getVk(), pipelineInfo);
}	

void Application::createFramebuffers()
{
	m_swapchainFramebuffers.resize(m_swapchainImageViews.size());

	for (size_t i = 0; i < m_swapchainImageViews.size(); i++) {
		VkImageView attachments[] = {
			m_swapchainImageViews[i]->getVk()
		};

		ft::Framebuffer::CreateInfo framebufferInfo{};
		framebufferInfo.renderPass = m_renderPass->getVk();
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = m_swapchain->getExtent().width;
		framebufferInfo.height = m_swapchain->getExtent().height;
		framebufferInfo.layers = 1;

		m_swapchainFramebuffers[i] = std::make_unique<ft::Framebuffer>(m_device->getVk(), framebufferInfo);
	}
}

void Application::createCommandPool()
{
	QueueFamilyIndices queueFamilyIndices = findQueueFamilies(m_physicalDevice->getVk());

	ft::CommandPool::CreateInfo poolInfo{};
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

	m_commandPool = std::make_unique<ft::CommandPool>(m_device->getVk(), poolInfo);
}

void Application::createTextureImage()
{
	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load("playground/textures/texture.ppm", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	VkDeviceSize imageSize = texWidth * texHeight * 4;

	if (pixels == nullptr)
	{
		throw std::runtime_error("failed to load texture: playground/textures/texture.ppm");
	}

	VkBufferCreateInfo stagingBufferInfo{};
	stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	stagingBufferInfo.size = imageSize;
	stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	stagingBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	ft::Buffer stagingBuffer(m_device->getVk(), stagingBufferInfo);

	VkMemoryRequirements stagingMemRequirements = stagingBuffer.getMemoryRequirements();

	VkMemoryAllocateInfo stagingMemoryInfo{};
	stagingMemoryInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	stagingMemoryInfo.allocationSize = stagingMemRequirements.size;
	stagingMemoryInfo.memoryTypeIndex = ft::DeviceMemory::findMemoryType(
		m_physicalDevice->getVk(),
		stagingMemRequirements.memoryTypeBits,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);

	ft::DeviceMemory stagingBufferMemory(m_device->getVk(), stagingMemoryInfo);

	vkBindBufferMemory(m_device->getVk(), stagingBuffer.getVk(), stagingBufferMemory.getVk(), 0);

	stagingBufferMemory.map();
	stagingBufferMemory.write((void*)pixels, imageSize);
	stagingBufferMemory.unmap();

	stbi_image_free(pixels);


	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = static_cast<uint32_t>(texWidth);
	imageInfo.extent.height = static_cast<uint32_t>(texHeight);
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = 
		VK_IMAGE_USAGE_TRANSFER_DST_BIT |
		VK_IMAGE_USAGE_SAMPLED_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

	m_textureImage = std::make_unique<ft::Image>(m_device->getVk(), imageInfo);

	VkMemoryRequirements imageMemRequirements = m_textureImage->getMemoryRequirements();

	VkMemoryAllocateInfo imageMemoryInfo{};
	imageMemoryInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	imageMemoryInfo.allocationSize = imageMemRequirements.size;
	imageMemoryInfo.memoryTypeIndex = ft::DeviceMemory::findMemoryType(
		m_physicalDevice->getVk(),
		imageMemRequirements.memoryTypeBits,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);

	m_textureImageMemory = std::make_unique<ft::DeviceMemory>(m_device->getVk(), imageMemoryInfo);

	m_textureImage->bindMemory(m_textureImageMemory->getVk());


	transitionImageLayout(
		m_textureImage->getVk(),
		VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
	);

	copyBufferToImage(
		stagingBuffer.getVk(),
		m_textureImage->getVk(),
		static_cast<uint32_t>(texWidth),
		static_cast<uint32_t>(texHeight)
	);

	transitionImageLayout(
		m_textureImage->getVk(),
		VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
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
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	m_textureImageView = std::make_unique<ft::ImageView>(m_device->getVk(), viewInfo);
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
	m_physicalDevice->getProperties(&properties);

	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

	samplerInfo.unnormalizedCoordinates = VK_FALSE;

	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

	m_textureSampler = std::make_unique<ft::Sampler>(m_device->getVk(), samplerInfo);
}

void Application::createVertexBuffer()
{
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();


	VkBufferCreateInfo stagingBufferInfo{};
	stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	stagingBufferInfo.size = bufferSize;
	stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	stagingBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	ft::Buffer stagingBuffer(m_device->getVk(), stagingBufferInfo);

	VkMemoryRequirements memRequirements = stagingBuffer.getMemoryRequirements();
	
	VkMemoryAllocateInfo stagingMemoryInfo{};
	stagingMemoryInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	stagingMemoryInfo.allocationSize = memRequirements.size;
	stagingMemoryInfo.memoryTypeIndex = ft::DeviceMemory::findMemoryType(
		m_physicalDevice->getVk(),
		memRequirements.memoryTypeBits,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);

	ft::DeviceMemory stagingBufferMemory(m_device->getVk(), stagingMemoryInfo);

	vkBindBufferMemory(m_device->getVk(), stagingBuffer.getVk(), stagingBufferMemory.getVk(), 0);


	stagingBufferMemory.map();
	stagingBufferMemory.write((void*)vertices.data(), bufferSize);
	stagingBufferMemory.unmap();


	VkBufferCreateInfo vertexBufferInfo{};
	vertexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	vertexBufferInfo.size = bufferSize;
	vertexBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	vertexBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	m_vertexBuffer = std::make_unique<ft::Buffer>(m_device->getVk(), vertexBufferInfo);

	VkMemoryRequirements vertexMemRequirements = m_vertexBuffer->getMemoryRequirements();

	VkMemoryAllocateInfo vertexMemoryInfo{};
	vertexMemoryInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	vertexMemoryInfo.allocationSize = vertexMemRequirements.size;
	vertexMemoryInfo.memoryTypeIndex = ft::DeviceMemory::findMemoryType(
		m_physicalDevice->getVk(),
		vertexMemRequirements.memoryTypeBits,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);

	m_vertexBufferMemory = std::make_unique<ft::DeviceMemory>(m_device->getVk(), vertexMemoryInfo);

	vkBindBufferMemory(m_device->getVk(), m_vertexBuffer->getVk(), m_vertexBufferMemory->getVk(), 0);


	copyBufferToBuffer(stagingBuffer.getVk(), m_vertexBuffer->getVk(), bufferSize);
}

void Application::createIndexBuffer()
{
	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	VkBufferCreateInfo stagingBufferInfo{};
	stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	stagingBufferInfo.size = bufferSize;
	stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	stagingBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	ft::Buffer stagingBuffer(m_device->getVk(), stagingBufferInfo);

	VkMemoryRequirements stagingMemRequirements = stagingBuffer.getMemoryRequirements();

	VkMemoryAllocateInfo stagingMemoryInfo{};
	stagingMemoryInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	stagingMemoryInfo.allocationSize = stagingMemRequirements.size;
	stagingMemoryInfo.memoryTypeIndex = ft::DeviceMemory::findMemoryType(
		m_physicalDevice->getVk(),
		stagingMemRequirements.memoryTypeBits,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);

	ft::DeviceMemory stagingBufferMemory(m_device->getVk(), stagingMemoryInfo);

	vkBindBufferMemory(m_device->getVk(), stagingBuffer.getVk(), stagingBufferMemory.getVk(), 0);


	stagingBufferMemory.map();
	stagingBufferMemory.write((void*)indices.data(), bufferSize);
	stagingBufferMemory.unmap();


	VkBufferCreateInfo indexBufferInfo{};
	indexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	indexBufferInfo.size = bufferSize;
	indexBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	indexBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	m_indexBuffer = std::make_unique<ft::Buffer>(m_device->getVk(), indexBufferInfo);

	VkMemoryRequirements indexMemRequirements = m_indexBuffer->getMemoryRequirements();

	VkMemoryAllocateInfo indexMemoryInfo{};
	indexMemoryInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	indexMemoryInfo.allocationSize = indexMemRequirements.size;
	indexMemoryInfo.memoryTypeIndex = ft::DeviceMemory::findMemoryType(
		m_physicalDevice->getVk(),
		indexMemRequirements.memoryTypeBits,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);

	m_indexBufferMemory = std::make_unique<ft::DeviceMemory>(m_device->getVk(), indexMemoryInfo);

	vkBindBufferMemory(m_device->getVk(), m_indexBuffer->getVk(), m_indexBufferMemory->getVk(), 0);


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

		m_uniformBuffers[i] = std::make_unique<ft::Buffer>(m_device->getVk(), bufferInfo);

		VkMemoryRequirements memRequirements = m_uniformBuffers[i]->getMemoryRequirements();

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = ft::DeviceMemory::findMemoryType(
			m_physicalDevice->getVk(),
			memRequirements.memoryTypeBits,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		);

		m_uniformBuffersMemory[i] = std::make_unique<ft::DeviceMemory>(m_device->getVk(), allocInfo);

		vkBindBufferMemory(m_device->getVk(), m_uniformBuffers[i]->getVk(), m_uniformBuffersMemory[i]->getVk(), 0);


		m_uniformBuffersMemory[i]->map();
    }
}

void Application::createDescriptorPool()
{
	VkDescriptorPoolSize poolSize{};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	m_descriptorPool = std::make_unique<ft::DescriptorPool>(m_device->getVk(), poolInfo);
}

void Application::createDescriptorSets()
{
	// TODO: rework all descriptor set stuff so that I can allocate all descriptor sets at once
	// also don't allow to create descriptor sets without a descriptor pool

	std::vector<VkDescriptorSetLayout> layouts(1, m_descriptorSetLayout->getVk());

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = m_descriptorPool->getVk();
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = layouts.data();

	m_descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		m_descriptorSets[i] = std::make_unique<ft::DescriptorSet>(m_device->getVk(), allocInfo);

		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = m_uniformBuffers[i]->getVk();
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = m_descriptorSets[i]->getVk();
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;

		m_descriptorSets[i]->update(1, &descriptorWrite);
	}

}

void Application::createCommandBuffer()
{
	m_commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

	ft::CommandBuffer::AllocateInfo allocInfo{};
	allocInfo.commandPool = m_commandPool->getVk();
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	for (size_t i = 0; i < m_commandBuffers.size(); i++)
	{
		m_commandBuffers[i] = std::make_unique<ft::CommandBuffer>(m_device->getVk(), allocInfo);
	}
}

void Application::createSyncObjects()
{
	m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	ft::Semaphore::CreateInfo semaphoreInfo{};

	ft::Fence::CreateInfo fenceInfo{};
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		m_imageAvailableSemaphores[i] = std::make_unique<ft::Semaphore>(m_device->getVk(), semaphoreInfo);
		m_renderFinishedSemaphores[i] = std::make_unique<ft::Semaphore>(m_device->getVk(), semaphoreInfo);
		m_inFlightFences[i] = std::make_unique<ft::Fence>(m_device->getVk(), fenceInfo);
	}

}


std::vector<const char*> Application::getRequiredExtensions() {
    std::vector<const char*> extensions = m_windowManager->getRequiredInstanceExtensions();

    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

void Application::populateDebugMessengerCreateInfo(ft::DebugMessenger::CreateInfo& createInfo)
{
	createInfo.messageSeverity =
		// VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		// VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

	createInfo.messageType =
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

	createInfo.pUserData = nullptr; // Optional

	/*
		This is used internally by the debug messenger to call the user callback.
		createInfo.pfnUserCallback = customUserCallback;

		Instead, use the following:
		createInfo.userCallback = customUserCallback;

		Which has the default value: ft::DebugMessenger::debugCallback
	*/
}

bool Application::isDeviceSuitable(const VkPhysicalDevice& physicalDevice)
{
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

	bool extensionsSupported = ft::PhysicalDevice::checkExtensionSupport(physicalDevice, deviceExtensions);

	bool swapChainAdequate = false;
	if (extensionsSupported)
	{
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);
		swapChainAdequate = swapChainSupport.formats.empty() == false
						&&	swapChainSupport.presentModes.empty() == false;
	}

	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(physicalDevice, &supportedFeatures);

	return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}

QueueFamilyIndices Application::findQueueFamilies(const VkPhysicalDevice& physicalDevice)
{
	QueueFamilyIndices indices;

	std::vector<VkQueueFamilyProperties> queueFamilyProperties = ft::PhysicalDevice::getQueueFamilyProperties(physicalDevice);

	int i = 0;
	for (const auto& queueFamily : queueFamilyProperties)
	{
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = ft::PhysicalDevice::getSurfaceSupport(physicalDevice, i, m_surface->getVk());

		if (presentSupport)
		{
			indices.presentFamily = i;
		}

		if (indices.isComplete())
		{
			break;
		}

		i++;
	}

	return indices;
}

SwapChainSupportDetails Application::querySwapChainSupport(const VkPhysicalDevice& device)
{
	SwapChainSupportDetails details;

	details.capabilities = ft::PhysicalDevice::getSurfaceCapabilities(device, m_surface->getVk());
	details.formats = ft::PhysicalDevice::getSurfaceFormats(device, m_surface->getVk());
	details.presentModes = ft::PhysicalDevice::getSurfacePresentModes(device, m_surface->getVk());

    return details;
}

VkSurfaceFormatKHR Application::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	for (const auto& availableFormat : availableFormats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB
			&& availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return availableFormat;
		}
	}

	return availableFormats[0];
}

VkPresentModeKHR Application::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	for (const auto& availablePresentMode : availablePresentModes)
	{
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Application::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != UINT32_MAX)
	{
		return capabilities.currentExtent;
	}
	else
	{
		int width, height;
		m_window->getFramebufferSize(&width, &height);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::clamp(
			actualExtent.width,
			capabilities.minImageExtent.width,
			capabilities.maxImageExtent.width
		);

		actualExtent.height = std::clamp(
			actualExtent.height,
			capabilities.minImageExtent.height,
			capabilities.maxImageExtent.height
		);

		return actualExtent;
	}
}


void Application::copyBufferToBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	ft::CommandBuffer* commandBuffer = beginSingleTimeCommands();

	VkBufferCopy copyRegion{};
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer->getVk(), srcBuffer, dstBuffer, 1, &copyRegion);

	endSingleTimeCommands(commandBuffer);
}

void Application::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
	ft::CommandBuffer* commandBuffer = beginSingleTimeCommands();

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
		commandBuffer->getVk(),
		buffer,
		image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&region
	);

	endSingleTimeCommands(commandBuffer);
}

ft::CommandBuffer* Application::beginSingleTimeCommands()
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = m_commandPool->getVk();
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	ft::CommandBuffer* commandBuffer = new ft::CommandBuffer(m_device->getVk(), allocInfo);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	commandBuffer->begin(beginInfo);

	return commandBuffer;
}

void Application::endSingleTimeCommands(ft::CommandBuffer* commandBuffer)
{
	commandBuffer->end();

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	VkCommandBuffer cmdBuf = commandBuffer->getVk();
	submitInfo.pCommandBuffers = &cmdBuf;

	m_graphicsQueue->submit(1, &submitInfo);
	m_graphicsQueue->waitIdle();

	delete commandBuffer;
}

void Application::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	ft::CommandBuffer* commandBuffer = beginSingleTimeCommands();

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;

	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
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
		commandBuffer->getVk(),
		sourceStage, destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);

	endSingleTimeCommands(commandBuffer);
}


void Application::recordCommandBuffer(const std::unique_ptr<ft::CommandBuffer>& commandBuffer, uint32_t imageIndex)
{
	ft::CommandBuffer::BeginInfo beginInfo = {};

	commandBuffer->begin(beginInfo);

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = m_renderPass->getVk();
	renderPassInfo.framebuffer = m_swapchainFramebuffers[imageIndex]->getVk();
	renderPassInfo.renderArea.offset = {0, 0};
	renderPassInfo.renderArea.extent = m_swapchain->getExtent();

	VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(commandBuffer->getVk(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(commandBuffer->getVk(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicPipeline->getVk());

	vkCmdBindDescriptorSets(
		commandBuffer->getVk(),
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		m_pipelineLayout->getVk(),
		0, 1,
		m_descriptorSets[m_currentFrame]->getVkPtr(),
		0, nullptr
	);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(m_swapchain->getExtent().width);
	viewport.height = static_cast<float>(m_swapchain->getExtent().height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer->getVk(), 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = {0, 0};
	scissor.extent = m_swapchain->getExtent();
	vkCmdSetScissor(commandBuffer->getVk(), 0, 1, &scissor);

	VkBuffer vertexBuffers[] = {m_vertexBuffer->getVk()};
	VkDeviceSize offsets[] = {0};
	vkCmdBindVertexBuffers(commandBuffer->getVk(), 0, 1, vertexBuffers, offsets);

	vkCmdBindIndexBuffer(commandBuffer->getVk(), m_indexBuffer->getVk(), 0, VK_INDEX_TYPE_UINT16);

	vkCmdDrawIndexed(commandBuffer->getVk(), static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

	vkCmdEndRenderPass(commandBuffer->getVk());

	commandBuffer->end();
}

void Application::drawFrame()
{
	m_inFlightFences[m_currentFrame]->wait();

	uint32_t imageIndex;
	VkResult result = m_swapchain->acquireNextImage(UINT64_MAX, m_imageAvailableSemaphores[m_currentFrame]->getVk(), VK_NULL_HANDLE, &imageIndex);

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

	m_commandBuffers[m_currentFrame]->reset();
	recordCommandBuffer(m_commandBuffers[m_currentFrame], imageIndex);

	updateUniformBuffer(m_currentFrame);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = {m_imageAvailableSemaphores[m_currentFrame]->getVk()};
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	VkCommandBuffer commandBuffers[] = {m_commandBuffers[m_currentFrame]->getVk()};
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = commandBuffers;

	VkSemaphore signalSemaphores[] = {m_renderFinishedSemaphores[m_currentFrame]->getVk()};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	m_graphicsQueue->submit(1, &submitInfo, m_inFlightFences[m_currentFrame]->getVk());

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = {m_swapchain->getVk()};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;

	result = vkQueuePresentKHR(m_presentQueue->getVk(), &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_framebufferResized)
	{
		m_framebufferResized = false;
		recreateSwapChain();
	}
	else if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to present swap chain image!");
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
	ubo.proj = glm::perspective(glm::radians(45.0f), m_swapchain->getExtent().width / (float) m_swapchain->getExtent().height, 0.1f, 10.0f);

	ubo.proj[1][1] *= -1;

	m_uniformBuffersMemory[currentImage]->write(&ubo, sizeof(ubo));
}

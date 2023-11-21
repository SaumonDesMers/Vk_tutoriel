#include "app/pipeline.hpp"
#include "logger.hpp"

#include <fstream>
#include <stdexcept>

namespace ft
{

	Pipeline::Pipeline(
		Device &device,
		const std::string &vertFilePath,
		const std::string &fragFilePath,
		const PipelineConfigInfo &configInfo
	):
		m_device(device)
	{
		createGraphicsPipeline(vertFilePath, fragFilePath, configInfo);
	}

	Pipeline::~Pipeline()
	{
		vkDestroyShaderModule(m_device.device(), m_vertShaderModule, nullptr);
		vkDestroyShaderModule(m_device.device(), m_fragShaderModule, nullptr);
		vkDestroyPipeline(m_device.device(), m_graphicsPipeline, nullptr);
	}

	PipelineConfigInfo Pipeline::defaultPipelineConfigInfo(uint32_t width, uint32_t height)
	{
		PipelineConfigInfo configInfo
		{
			.viewport
			{
				.x = 0.0f,
				.y = 0.0f,
				.width = static_cast<float>(width),
				.height = static_cast<float>(height),
				.minDepth = 0.0f,
				.maxDepth = 1.0f
			},
			.scissor
			{
				.offset = {0, 0},
				.extent = {width, height}
			},
			.viewportInfo
			{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
				.viewportCount = 1,
				.pViewports = &configInfo.viewport,
				.scissorCount = 1,
				.pScissors = &configInfo.scissor
			},
			.inputAssemblyInfo
			{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
				.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
				.primitiveRestartEnable = VK_FALSE
			},
			.rasterizationInfo
			{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
				.depthClampEnable = VK_FALSE,
				.rasterizerDiscardEnable = VK_FALSE,
				.polygonMode = VK_POLYGON_MODE_FILL,
				.cullMode = VK_CULL_MODE_NONE,
				.frontFace = VK_FRONT_FACE_CLOCKWISE,
				.depthBiasEnable = VK_FALSE,
				.depthBiasConstantFactor = 0.0f, // Optional
				.depthBiasClamp = 0.0f, // Optional
				.depthBiasSlopeFactor = 0.0f, // Optional
				.lineWidth = 1.0f
			},
			.multisampleInfo
			{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
				.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
				.sampleShadingEnable = VK_FALSE,
				.minSampleShading = 1.0f, // Optional
				.pSampleMask = nullptr, // Optional
				.alphaToCoverageEnable = VK_FALSE, // Optional
				.alphaToOneEnable = VK_FALSE, // Optional
			},
			.colorBlendAttachment
			{
				.blendEnable = VK_FALSE,
				.srcColorBlendFactor = VK_BLEND_FACTOR_ONE, // Optional
				.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO, // Optional
				.colorBlendOp = VK_BLEND_OP_ADD, // Optional
				.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE, // Optional
				.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO, // Optional
				.alphaBlendOp = VK_BLEND_OP_ADD, // Optional
				.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
			},
			.colorBlendInfo
			{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
				.logicOpEnable = VK_FALSE,
				.logicOp = VK_LOGIC_OP_COPY, // Optional
				.attachmentCount = 1,
				.pAttachments = &configInfo.colorBlendAttachment,
				.blendConstants
				{
					0.0f,
					0.0f,
					0.0f,
					0.0f
				}
			},
			.depthStencilInfo
			{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
				.depthTestEnable = VK_TRUE,
				.depthWriteEnable = VK_TRUE,
				.depthCompareOp = VK_COMPARE_OP_LESS,
				.depthBoundsTestEnable = VK_FALSE,
				.stencilTestEnable = VK_FALSE,
				.front = {}, // Optional
				.back = {}, // Optional
				.minDepthBounds = 0.0f, // Optional
				.maxDepthBounds = 1.0f, // Optional
			}
		};

		return configInfo;
	}

	std::vector<char> Pipeline::readFile(const std::string &filename)
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			throw std::runtime_error("failed to open file: " + filename);
		}

		size_t fileSize = static_cast<size_t>(file.tellg());
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}

	void Pipeline::createGraphicsPipeline(
		const std::string &vertFilePath,
		const std::string &fragFilePath,
		const PipelineConfigInfo &configInfo
	) {
		auto vertShaderCode = readFile(vertFilePath);
		auto fragShaderCode = readFile(fragFilePath);

		createShaderModule(vertShaderCode, &m_vertShaderModule);
		createShaderModule(fragShaderCode, &m_fragShaderModule);

		VkPipelineShaderStageCreateInfo shaderStages[]
		{
			{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.stage = VK_SHADER_STAGE_VERTEX_BIT,
				.module = m_vertShaderModule,
				.pName = "main",
				.pSpecializationInfo = nullptr
			},
			{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
				.module = m_fragShaderModule,
				.pName = "main",
				.pSpecializationInfo = nullptr
			}
		};

		VkPipelineVertexInputStateCreateInfo vertexInputInfo
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			.vertexBindingDescriptionCount = 0,
			.pVertexBindingDescriptions = nullptr,
			.vertexAttributeDescriptionCount = 0,
			.pVertexAttributeDescriptions = nullptr
		};

		VkGraphicsPipelineCreateInfo pipelineInfo
		{
			.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			.stageCount = 2,
			.pStages = shaderStages,
			.pVertexInputState = &vertexInputInfo,
			.pInputAssemblyState = &configInfo.inputAssemblyInfo,
			.pViewportState = &configInfo.viewportInfo,
			.pRasterizationState = &configInfo.rasterizationInfo,
			.pMultisampleState = &configInfo.multisampleInfo,
			.pDepthStencilState = &configInfo.depthStencilInfo,
			.pColorBlendState = &configInfo.colorBlendInfo,
			.pDynamicState = nullptr,

			.layout = configInfo.pipelineLayout,
			.renderPass = configInfo.renderPass,
			.subpass = configInfo.subpass,

			.basePipelineHandle = VK_NULL_HANDLE,
			.basePipelineIndex = -1
		};

		if (vkCreateGraphicsPipelines(m_device.device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create graphics pipeline.");
		}
	}

	void Pipeline::createShaderModule(const std::vector<char> &code, VkShaderModule *shaderModule)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();

		// The data is uint32_t, so we need to cast it to char
		createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

		if (vkCreateShaderModule(m_device.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create shader module.");
		}
	}

}

#pragma once

#include "device.hpp"

#include <string>
#include <vector>

namespace ft
{

	struct PipelineConfigInfo
	{
		PipelineConfigInfo(const PipelineConfigInfo &) = delete;
		void operator=(const PipelineConfigInfo &) = delete;

		std::vector<VkVertexInputBindingDescription> bindingDescriptions;
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		std::vector<VkDynamicState> dynamicStateEnables;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo;
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};

	class Pipeline
	{

	public:

		Pipeline(
			Device &device,
			const std::string &vertFilePath,
			const std::string &fragFilePath,
			const PipelineConfigInfo &configInfo
		);
		~Pipeline();

		Pipeline(const Pipeline &) = delete;
		void operator=(const Pipeline &) = delete; 

		void bind(VkCommandBuffer commandBuffer);

		static void defaultPipelineConfigInfo(PipelineConfigInfo &configInfo);

	private:

		Device &m_device;
		VkPipeline m_graphicsPipeline;
		VkShaderModule m_vertShaderModule;
		VkShaderModule m_fragShaderModule;

		static std::vector<char> readFile(const std::string &filename);

		void createGraphicsPipeline(
			const std::string &vertFilePath,
			const std::string &fragFilePath,
			const PipelineConfigInfo &configInfo
		);

		void createShaderModule(const std::vector<char> &code, VkShaderModule *shaderModule);

	};
	
}

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
	}

	PipelineConfigInfo Pipeline::defaultPipelineConfigInfo(uint32_t width, uint32_t height)
	{
		PipelineConfigInfo configInfo{};

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

		ft::log << ft::Logger::Level::INFO << "vertShaderCode.size(): " << vertShaderCode.size() << std::endl;
		ft::log << ft::Logger::Level::INFO << "fragShaderCode.size(): " << fragShaderCode.size() << std::endl;
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

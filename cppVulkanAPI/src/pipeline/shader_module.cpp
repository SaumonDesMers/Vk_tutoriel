#include "shader_module.hpp"

#include <stdexcept>
#include <fstream>

namespace LIB_NAMESPACE
{
	namespace core
	{
		ShaderModule::ShaderModule(VkDevice device, const CreateInfo& createInfo)
			: m_device(device)
		{
			if (vkCreateShaderModule(m_device, &createInfo, nullptr, &m_shaderModule) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create shader module.");
			}
		}

		ShaderModule::ShaderModule(VkDevice device, std::filesystem::path path)
			: m_device(device)
		{
			std::vector<char> code = readFile(path);

			CreateInfo createInfo;
			createInfo.codeSize = code.size();
			createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

			if (vkCreateShaderModule(m_device, &createInfo, nullptr, &m_shaderModule) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create shader module.");
			}
		}

		ShaderModule::~ShaderModule()
		{
			vkDestroyShaderModule(m_device, m_shaderModule, nullptr);
		}

		std::vector<char> ShaderModule::readFile(const std::filesystem::path& path)
		{
			std::ifstream file(path, std::ios::ate | std::ios::binary);

			if (!file.is_open()) {
				throw std::runtime_error("failed to open file: " + path.string());
			}

			size_t fileSize = (size_t)file.tellg();
			std::vector<char> buffer(fileSize);

			file.seekg(0);
			file.read(buffer.data(), fileSize);

			file.close();

			return buffer;
		}
	}
}
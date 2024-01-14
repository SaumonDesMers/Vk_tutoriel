#pragma once

#include "../defines.hpp"

#include <filesystem>
#include <vector>

#include <vulkan/vulkan.h>

namespace LIB_NAMESPACE
{
	namespace core
	{
		class ShaderModule
		{
		
		public:

			struct CreateInfo: public VkShaderModuleCreateInfo
			{
				CreateInfo(): VkShaderModuleCreateInfo()
				{
					this->sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
				}
			};

			ShaderModule(VkDevice device, const CreateInfo& createInfo);
			ShaderModule(VkDevice device, std::filesystem::path path);
			~ShaderModule();

			VkShaderModule getVk() const { return m_shaderModule; }

			static std::vector<char> readFile(const std::filesystem::path& path);

		private:

			VkShaderModule m_shaderModule;

			VkDevice m_device;

		};
	}
}
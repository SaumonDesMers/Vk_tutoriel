#pragma once

#include "defines.hpp"
#include "framework/memory/image.hpp"
#include "framework/descriptor/descriptor.hpp"
#include "framework/command.hpp"
#include "core/image/sampler.hpp"

#include <memory>

namespace LIB_NAMESPACE
{
	class Texture
	{

	public:

		typedef uint32_t ID;

		struct CreateInfo
		{
			std::string filepath;
			uint32_t mipLevel;
			VkShaderStageFlags stageFlags;
		};

		Texture(
			VkDevice device,
			VkPhysicalDevice physicalDevice,
			Command& command,
			CreateInfo& createInfo
		);
		Texture(const Texture&) = delete;
		~Texture();

		Image& image() const { return *m_image.get(); }
		VkSampler sampler() const { return m_sampler->getVk(); }
		Descriptor* descriptor() const { return m_descriptor.get(); }

		int width() const { return m_width; }
		int height() const { return m_height; }

	private:

		std::unique_ptr<Image> m_image;
		std::unique_ptr<core::Sampler> m_sampler;
		std::unique_ptr<Descriptor> m_descriptor;

		int m_width;
		int m_height;

		void createImage(
			VkDevice device,
			VkPhysicalDevice physicalDevice,
			CreateInfo& createInfo
		);

		void createSampler(
			VkDevice device,
			VkPhysicalDevice physicalDevice,
			CreateInfo& createInfo
		);

		void createDescriptor(
			VkDevice device,
			CreateInfo& createInfo
		);

	};
}
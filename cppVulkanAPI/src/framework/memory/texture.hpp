#pragma once

#include "defines.hpp"
#include "image.hpp"
#include "framework/command.hpp"
#include "core/image/sampler.hpp"

#include <memory>

namespace LIB_NAMESPACE
{
	class Texture
	{

	public:

		Texture(
			VkDevice device,
			VkPhysicalDevice physicalDevice,
			Command& command,
			std::string path
		);
		Texture(const Texture&) = delete;
		Texture(Texture&&);
		~Texture();

		Image& image() const { return *m_image.get(); }
		VkSampler sampler() const { return m_sampler->getVk(); }

		int width() const { return m_width; }
		int height() const { return m_height; }

	private:

		std::unique_ptr<Image> m_image;
		std::unique_ptr<core::Sampler> m_sampler;

		int m_width;
		int m_height;

		void createImage(VkDevice device, VkPhysicalDevice physicalDevice);

	};
}
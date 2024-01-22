#pragma once

#include "defines.hpp"
#include "framework/memory/buffer.hpp"
#include "framework/descriptor/descriptor.hpp"
#include "framework/command.hpp"

#include <memory>

namespace LIB_NAMESPACE
{
	class UniformBuffer
	{

	public:

		typedef uint32_t ID;

		struct CreateInfo
		{
			VkDeviceSize size;
			VkShaderStageFlags stageFlags;
		};

		static inline ID maxID = 0;

		UniformBuffer(
			VkDevice device,
			VkPhysicalDevice physicalDevice,
			CreateInfo& createInfo
		);
		UniformBuffer(const UniformBuffer&) = delete;
		~UniformBuffer();

		Buffer* buffer(uint32_t index) const { return m_buffers[index].get(); }
		Descriptor* descriptor() const { return m_descriptor.get(); }

		int size() { return m_size; }

	private:

		std::vector<std::unique_ptr<Buffer>> m_buffers;
		std::unique_ptr<Descriptor> m_descriptor;

		VkDeviceSize m_size;

		void createBuffer(
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
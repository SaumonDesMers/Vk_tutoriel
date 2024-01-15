#pragma once

#include "defines.hpp"
#include "buffer.hpp"

namespace LIB_NAMESPACE
{
	class StagingBuffer: public Buffer
	{
	
	public:

		StagingBuffer(
			VkDevice device,
			VkPhysicalDevice physicalDevice,
			void* data,
			uint32_t bufferSize
		);
		~StagingBuffer();

	private:

		static VkBufferCreateInfo getBufferCreateInfo(VkDeviceSize bufferSize);

	};
}
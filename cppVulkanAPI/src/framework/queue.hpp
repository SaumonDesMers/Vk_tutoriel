#pragma once

#include "defines.hpp"

#include <optional>
#include <cstdint>

namespace LIB_NAMESPACE
{
	class Queue
	{

	public:

		struct FamilyIndices
		{
			std::optional<uint32_t> graphicsFamily;
			std::optional<uint32_t> presentFamily;

			bool isComplete()
			{
				return graphicsFamily.has_value() && presentFamily.has_value();
			}
		};

	private:

	};
}
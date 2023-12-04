#pragma once

#include "defines.hpp"

#include <vulkan/vulkan.hpp>

namespace LIB_NAMESPACE
{

struct ApplicationInfo: public vk::ApplicationInfo
{
};

struct InstanceCreateInfo: public vk::InstanceCreateInfo
{
};

class Instance: public vk::Instance
{

public:

	Instance(const ft::InstanceCreateInfo& createInfo);

	~Instance();

private:


};

}
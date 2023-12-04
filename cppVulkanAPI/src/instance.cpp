#include "instance.hpp"
#include "defines.hpp"

namespace LIB_NAMESPACE
{

Instance::Instance(const ft::InstanceCreateInfo& createInfo)
{
	// Create instance
	vk::Result result = vk::createInstance(&createInfo, nullptr, this);
	if (result != vk::Result::eSuccess)
	{
		throw std::runtime_error("Failed to create instance");
	}
}

Instance::~Instance()
{
	this->destroy();
}

}
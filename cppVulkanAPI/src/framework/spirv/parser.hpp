#pragma once

#include "defines.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <iomanip>
#include <map>

#include "spriv.hpp"

std::string storageClassName(uint32_t n);

struct ShaderTypeInfo
{
	// the ID
	uint32_t id;
	// the OpType* instruction
	spv::Op opType;
	// the IDs types this type is based on
	std::vector<uint32_t> baseTypesID;
	// the size in bytes
	uint32_t size;
	// storage class
	spv::StorageClass storageClass = spv::StorageClassMax;
	// decorations
	std::vector<spv::Decoration> decorations;
	// decorations arguments
	uint32_t location;
	uint32_t binding;
	uint32_t set;

	// add a base type to this type
	void addBaseType(const ShaderTypeInfo& type);

	void print();
};

struct VulkanResourceInfo
{
	enum Type
	{
		IMAGE_SAMPLER,
		UNIFORM_BUFFER,
		PUSH_CONSTANT
	};
	Type type;

	VkShaderStageFlagBits stageFlags;

	// IMAGE_SAMPLER
	uint32_t imageSamplerDescriptorSet;
	uint32_t imageSamplerBinding;

	// UNIFORM_BUFFER
	uint32_t uniformBufferSize;
	uint32_t uniformBufferDescriptorSet;
	uint32_t uniformBufferBinding;

	// PUSH_CONSTANT
	uint32_t pushConstantSize;
};

std::vector<VulkanResourceInfo> parseSpriv(const std::string& vertexShaderFile, const std::string& fragmentShaderFile);
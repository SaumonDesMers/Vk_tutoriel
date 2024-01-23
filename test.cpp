#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <iomanip>
#include <map>

#include "spriv.hpp"

struct TypeInfo
{
	// the ID of the type
	uint32_t id;
	// the OpType* instruction
	spv::Op opType;
	// the IDs of the types this type is based on
	std::vector<uint32_t> baseTypesID;
	// the size of the type in bytes
	uint32_t size;
	// storage class of the type
	spv::StorageClass storageClass;

	// add a base type to this type
	void addBaseType(const TypeInfo& type)
	{
		baseTypesID.push_back(type.id);
		size += type.size;
	}
};

std::map<uint32_t, TypeInfo> typeInfoMap;



void handleOpCode(uint32_t opcode, uint32_t* args, uint32_t count);

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cout << "Usage: " << argv[0] << " <filename>" << std::endl;
		return 1;
	}

	// read binary file
	std::ifstream file(argv[1], std::ios::binary | std::ios::ate);
	if (!file.is_open())
	{
		std::cout << "Failed to open file" << std::endl;
		return 1;
	}

	size_t fileSize = (size_t)file.tellg();
	file.seekg(0);

	// read file
	std::vector<char> buffer(fileSize);
	file.read(buffer.data(), fileSize);

	file.close();

	std::vector<uint32_t> spirvCode(fileSize / sizeof(uint32_t));
	memcpy(spirvCode.data(), buffer.data(), fileSize);

	size_t offset = 0;
	uint32_t magic = spirvCode[offset++];
	uint32_t version = spirvCode[offset++];
	uint32_t generator = spirvCode[offset++];
	uint32_t bound = spirvCode[offset++];
	uint32_t schema = spirvCode[offset++];

	while (offset < spirvCode.size())
	{
		uint32_t wordCount = spirvCode[offset] >> 16;
		uint32_t opcode = spirvCode[offset] & 0xFFFF;
		
		handleOpCode(opcode, &spirvCode[offset + 1], wordCount - 1);

		offset += wordCount;
	}

	std::cout << std::endl << "Type sizes:" << std::endl;
	for (auto& type : typeInfoMap)
	{
		std::cout << "%" << type.first << ": " << type.second.size << std::endl;
	}

	return 0;
}


std::string storageClassName(uint32_t n)
{
	switch (n)
	{
		case spv::StorageClassUniformConstant: return "uniformConstant";
		case spv::StorageClassInput: return "input";
		case spv::StorageClassUniform: return "uniform";
		case spv::StorageClassOutput: return "output";
		case spv::StorageClassWorkgroup: return "workgroup";
		case spv::StorageClassCrossWorkgroup: return "crossWorkgroup";
		case spv::StorageClassPrivate: return "private";
		case spv::StorageClassFunction: return "function";
		case spv::StorageClassGeneric: return "generic";
		case spv::StorageClassPushConstant: return "pushConstant";
		case spv::StorageClassAtomicCounter : return "atomicCounter";
		case spv::StorageClassImage : return "image";
		case spv::StorageClassStorageBuffer : return "storageBuffer";
		default: return "unknown";
	}
}

void handleOpCode(uint32_t opcode, uint32_t* args, uint32_t count)
{
	uint32_t size = 0;
	switch (opcode)
	{
		case spv::OpTypeVoid:
			std::cout << "%" << args[0] << " = OpTypeVoid" << std::endl;
			typeInfoMap[args[0]] = {args[0], spv::OpTypeVoid, {}, 0};
			break;
		case spv::OpTypeBool:
			std::cout << "%" << args[0] << " = OpTypeBool" << std::endl;
			typeInfoMap[args[0]] = {args[0], spv::OpTypeBool, {}, 1};
			break;
		case spv::OpTypeInt:
			std::cout << "%" << args[0] << " = OpTypeInt " << args[1] << " " << args[2] << std::endl;
			typeInfoMap[args[0]] = {args[0], spv::OpTypeInt, {}, args[2] / 8};
			break;
		case spv::OpTypeFloat:
			std::cout << "%" << args[0] << " = OpTypeFloat " << args[1] << std::endl;
			typeInfoMap[args[0]] = {args[0], spv::OpTypeFloat, {}, args[1] / 8};
			break;
		case spv::OpTypeVector:
			std::cout << "%" << args[0] << " = OpTypeVector %" << args[1] << " " << args[2] << std::endl;
			typeInfoMap[args[0]] = {args[0], spv::OpTypeVector, {args[1]}, typeInfoMap[args[1]].size * args[2]};
			break;
		case spv::OpTypeMatrix:
			std::cout << "%" << args[0] << " = OpTypeMatrix %" << args[1] << " " << args[2] << std::endl;
			typeInfoMap[args[0]] = {args[0], spv::OpTypeMatrix, {args[1]}, typeInfoMap[args[1]].size * args[2]};
			break;
		case spv::OpTypeImage:
			std::cout << "%" << args[0] << " = OpTypeImage %" << args[1] << " " << args[2] << " " << args[3] << " " << args[4] << " " << args[5] << " " << args[6] << " " << args[7] << std::endl;
			break;
		case spv::OpTypeSampler:
			std::cout << "%" << args[0] << " = OpTypeSampler" << std::endl;
			break;
		case spv::OpTypeSampledImage:
			std::cout << "%" << args[0] << " = OpTypeSampledImage %" << args[1] << std::endl;
			break;
		case spv::OpTypeArray:
			std::cout << "%" << args[0] << " = OpTypeArray %" << args[1] << " %" << args[2] << std::endl;
			break;
		case spv::OpTypeStruct:
			std::cout << "%" << args[0] << " = OpTypeStruct";
			for (uint32_t i = 1; i < count; i++)
				std::cout << " %" << args[i];
			std::cout << std::endl;
			typeInfoMap[args[0]] = {args[0], spv::OpTypeStruct, {}, size};
			for (uint32_t i = 1; i < count; i++)
				typeInfoMap[args[0]].addBaseType(typeInfoMap[args[i]]);
			break;
		case spv::OpTypePointer:
			std::cout << "%" << args[0] << " = OpTypePointer " << storageClassName(args[1]) << " %" << args[2] << std::endl;
			typeInfoMap[args[0]] = {args[0], spv::OpTypePointer, {args[2]}, 0};
			break;
		case spv::OpVariable:
			std::cout << "%" << args[1] << " = OpVariable %" << args[0] << " " << storageClassName(args[2]) << " " << args[3] << std::endl;
			typeInfoMap[args[1]] = {args[1], spv::OpVariable, {args[0]}, 0};
			break;
		case spv::OpDecorate:
			std::cout << "OpDecorate %" << args[0] << " " << args[1];
			for (uint32_t i = 2; i < count; i++)
				std::cout << " " << args[i];
			std::cout << std::endl;
			break;
		default:
			break;
	}
}

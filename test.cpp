#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <iomanip>
#include <map>

#include "spriv.hpp"

bool readSpirv(const std::string& filename, std::vector<uint32_t> &spirvCode);
std::string storageClassName(uint32_t n);

struct TypeInfo
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
	void addBaseType(const TypeInfo& type)
	{
		baseTypesID.push_back(type.id);
		size += type.size;
	}

	void print()
	{
		std::cout << "%" << id << ":\n";
		std::cout << "  opType: " << opType << "\n";
		if (baseTypesID.size() > 0)
		{
			std::cout << "  baseTypesID: ";
			for (auto& id : baseTypesID)
				std::cout << "%" << id << " ";
			std::cout << "\n";
		}
		if (size > 0)
			std::cout << "  size: " << size << "\n";
		if (storageClass != spv::StorageClassMax)
			std::cout << "  storageClass: " << storageClassName(storageClass) << "\n";
		for (auto& decoration : decorations)
		{
			std::cout << "  decoration: " << decoration;
			switch (decoration)
			{
				case spv::DecorationLocation:
					std::cout << " (Location) " << location;
					break;
				case spv::DecorationBinding:
					std::cout << " (Binding) " << binding;
					break;
				case spv::DecorationDescriptorSet:
					std::cout << " (DescriptorSet) " << set;
					break;
				default:
					break;
			}
			std::cout << "\n";
		}
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
	std::vector<uint32_t> spirvCode;
	if (!readSpirv(argv[1], spirvCode))
		return 1;

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

	std::cout << std::endl;
	for (auto& [id, typeInfo] : typeInfoMap)
	{
		if (typeInfo.opType == spv::OpVariable)
		{
			switch (typeInfo.storageClass)
			{
				case spv::StorageClassUniformConstant:
					std::cout << "uniformConstant ";
					break;
				case spv::StorageClassInput:
					std::cout << "layout(location = " << typeInfo.location << ") in " << typeInfo.size << " ";
					break;
				case spv::StorageClassUniform: {
					uint32_t pointerID = typeInfo.baseTypesID[0];
					uint32_t structID = typeInfoMap[pointerID].baseTypesID[0];
					std::cout << "layout(set = " << typeInfo.set
						<< ", binding = " << typeInfo.binding
						<< ") uniform " << typeInfoMap[structID].size << " ";
					break;
				}
				case spv::StorageClassOutput:
					std::cout << "layout(location = " << typeInfo.location << ") out " << typeInfo.size << " ";
					break;
				default:
					break;
			}
			std::cout << std::endl;
		}
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
			typeInfoMap[args[0]].id = args[0];
			typeInfoMap[args[0]].opType = spv::OpTypeVoid;
			typeInfoMap[args[0]].size = 0;
			break;
		case spv::OpTypeBool:
			std::cout << "%" << args[0] << " = OpTypeBool" << std::endl;
			typeInfoMap[args[0]].id = args[0];
			typeInfoMap[args[0]].opType = spv::OpTypeBool;
			typeInfoMap[args[0]].size = 1;
			break;
		case spv::OpTypeInt:
			std::cout << "%" << args[0] << " = OpTypeInt " << args[1] << " " << args[2] << std::endl;
			typeInfoMap[args[0]].id = args[0];
			typeInfoMap[args[0]].opType = spv::OpTypeInt;
			typeInfoMap[args[0]].size = args[1] / 8;
			break;
		case spv::OpTypeFloat:
			std::cout << "%" << args[0] << " = OpTypeFloat " << args[1] << std::endl;
			typeInfoMap[args[0]].id = args[0];
			typeInfoMap[args[0]].opType = spv::OpTypeFloat;
			typeInfoMap[args[0]].size = args[1] / 8;
			break;
		case spv::OpTypeVector:
			std::cout << "%" << args[0] << " = OpTypeVector %" << args[1] << " " << args[2] << std::endl;
			typeInfoMap[args[0]].id = args[0];
			typeInfoMap[args[0]].opType = spv::OpTypeVector;
			typeInfoMap[args[0]].baseTypesID.push_back(args[1]);
			typeInfoMap[args[0]].size = typeInfoMap[args[1]].size * args[2];
			break;
		case spv::OpTypeMatrix:
			std::cout << "%" << args[0] << " = OpTypeMatrix %" << args[1] << " " << args[2] << std::endl;
			typeInfoMap[args[0]].id = args[0];
			typeInfoMap[args[0]].opType = spv::OpTypeMatrix;
			typeInfoMap[args[0]].baseTypesID.push_back(args[1]);
			typeInfoMap[args[0]].size = typeInfoMap[args[1]].size * args[2];
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
			typeInfoMap[args[0]].id = args[0];
			typeInfoMap[args[0]].opType = spv::OpTypeStruct;
			for (uint32_t i = 1; i < count; i++)
			{
				typeInfoMap[args[0]].baseTypesID.push_back(args[i]);
				typeInfoMap[args[0]].size += typeInfoMap[args[i]].size;
			}
			break;
		case spv::OpTypePointer:
			std::cout << "%" << args[0] << " = OpTypePointer " << storageClassName(args[1]) << " %" << args[2] << std::endl;
			typeInfoMap[args[0]].id = args[0];
			typeInfoMap[args[0]].opType = spv::OpTypePointer;
			typeInfoMap[args[0]].baseTypesID.push_back(args[2]);
			typeInfoMap[args[0]].storageClass = (spv::StorageClass)args[1];
			break;
		case spv::OpVariable:
			std::cout << "%" << args[1] << " = OpVariable %" << args[0] << " " << storageClassName(args[2]) << " " << args[3] << std::endl;
			typeInfoMap[args[1]].id = args[1];
			typeInfoMap[args[1]].opType = spv::OpVariable;
			typeInfoMap[args[1]].baseTypesID.push_back(args[0]);
			typeInfoMap[args[1]].storageClass = (spv::StorageClass)args[2];
			break;
		case spv::OpDecorate:
			std::cout << "OpDecorate %" << args[0] << " " << args[1];
			typeInfoMap[args[0]].decorations.push_back((spv::Decoration)args[1]);
			switch (args[1])
			{
				case spv::DecorationLocation:
					std::cout << " (Location) " << args[2];
					typeInfoMap[args[0]].location = args[2];
					break;
				case spv::DecorationBinding:
					std::cout << " (Binding) " << args[2];
					typeInfoMap[args[0]].binding = args[2];
					break;
				case spv::DecorationDescriptorSet:
					std::cout << " (DescriptorSet) " << args[2];
					typeInfoMap[args[0]].set = args[2];
					break;
				default:
					break;
			}
			std::cout << std::endl;
			break;
		default:
			break;
	}
}

bool readSpirv(const std::string& filename, std::vector<uint32_t> &spirvCode)
{
	std::ifstream file(filename, std::ios::binary | std::ios::ate);
	if (!file.is_open())
	{
		std::cout << "Failed to open file" << std::endl;
		return false;
	}

	size_t fileSize = (size_t)file.tellg();
	file.seekg(0);

	std::vector<char> buffer(fileSize);
	file.read(buffer.data(), fileSize);

	file.close();

	spirvCode.clear();
	spirvCode.resize(fileSize / sizeof(uint32_t));
	memcpy(spirvCode.data(), buffer.data(), fileSize);
	return true;
}
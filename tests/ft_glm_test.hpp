#include "ft_glm/ft_glm.hpp"
#include <glm/glm.hpp>

#include <iostream>
#include <vector>
#include <bitset>

/*
 * This is a testing function to see if ft_glm is working the same as glm.
 */

std::string getMemoryBytes(void* ptr, size_t size) {

	std::string result = "";

	for (size_t i = 0; i < size; i++) {
		result += std::bitset<8>(*((uint8_t*)ptr + i)).to_string() + " ";
	}

	return result;
}

template<typename T>
std::string test_memory(T arg) {
	return getMemoryBytes(&arg, sizeof(T));
}


template<typename T>
std::string test_vec2(T vec) {

	std::string result = "";

	T vec2_1 = {1, 2};

	result += std::to_string(vec2_1[0]) + std::to_string(vec2_1[1]) + " ";

	return result;
}

void test_ft_glm() {

	std::cout << "glm    = " << test_vec2(glm::vec2()) << std::endl;
	std::cout << "ft_glm = " << test_vec2(ft_glm::vec2()) << std::endl;

	std::cout << "glm    = " << test_memory(glm::vec2(36.0f, 3.0f)) << std::endl;
	std::cout << "ft_glm = " << test_memory(ft_glm::vec2(36.0f, 3.0f)) << std::endl;
}
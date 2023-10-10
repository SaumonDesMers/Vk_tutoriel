#ifndef FT_GLM_TEST_HPP
#define FT_GLM_TEST_HPP

#include "ft_glm/ft_glm.hpp"
#include <glm/glm.hpp>

#include <iostream>
#include <vector>
#include <bitset>

/*
 * This is a testing file to see if ft is working the same as glm.
 */

#define TEST(test) std::string color = test ? "\033[32m" : "\033[31m"; \
	std::cout << color << #test << "\033[0m" << std::endl;

std::string getMemoryBytes(void* ptr, size_t size) {

	std::string result = "";

	for (size_t i = 0; i < size; i++) {
		result += std::bitset<8>(*((uint8_t*)ptr + i)).to_string() + " ";
	}

	return result;
}

bool checkMemoryBytes(void* ptr1, size_t size1, void* ptr2, size_t size2) {

	if (size1 != size2) {
		return false;
	}

	uint8_t* ptr1_ = (uint8_t*)ptr1;
	uint8_t* ptr2_ = (uint8_t*)ptr2;

	for (size_t i = 0; i < size1; i++) {
		if (ptr1_[i] != ptr2_[i]) {
			return false;
		}
	}

	return true;
}


template<typename T, typename U>
bool memoryTest(T arg1, U arg2, std::string testName) {
	bool result = checkMemoryBytes(&arg1, sizeof(arg1), &arg2, sizeof(arg2));

	std::string color = result ? "\033[32m" : "\033[31m";

	if (result == true)
		std::cout << color << testName << "\033[0m" << std::endl;
	else {
		std::cout << color << testName << "\033[0m" << std::endl;
		std::cout << "Expected: " << getMemoryBytes(&arg1, sizeof(arg1)) << std::endl;
		std::cout << "Got:      " << getMemoryBytes(&arg2, sizeof(arg2)) << std::endl;
	}

	return result;
}

#define test_vec(size, ...) {\
\
	typedef ft::vec##size ft_vec2;\
	typedef glm::vec##size glm_vec2;\
\
	std::cout << "Memory tests vec" << size << ":" << std::endl;\
\
	memoryTest(glm_vec2(), ft_vec2(), "Default constructor");\
\
	ft_vec2 ft_vec2_1(__VA_ARGS__);\
	glm_vec2 glm_vec2_1(__VA_ARGS__);\
	memoryTest(glm_vec2_1, ft_vec2_1, "Constructor with " + std::to_string(size) + " arguments");\
\
	ft_vec2 ft_vec2_2 = ft_vec2_1;\
	glm_vec2 glm_vec2_2 = glm_vec2_1;\
	memoryTest(glm_vec2_2, ft_vec2_2, "Copy constructor");\
\
	ft_vec2 ft_vec2_3;\
	glm_vec2 glm_vec2_3;\
	ft_vec2_3 = ft_vec2_1;\
	glm_vec2_3 = glm_vec2_1;\
	memoryTest(glm_vec2_3, ft_vec2_3, "Copy assignment operator");\
\
	ft_vec2 ft_vec2_4 = ft_vec2_1.operator*<float>(2.0f);\
	glm_vec2 glm_vec2_4 = glm_vec2_1 * 2.0f;\
	memoryTest(glm_vec2_4, ft_vec2_4, "Scalar multiplication");\
}

template<typename T>
void log_vec4(T vec) {
	for (size_t j = 0; j < 4; j++)
		std::cout << vec[j] << " ";
	std::cout << std::endl;
}

template<typename T>
void log_mat4(T mat) {
	for (size_t i = 0; i < 4; i++) {
		for (size_t j = 0; j < 4; j++)
			std::cout << mat[i][j] << " ";
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

void test_mat4() {

	std::cout << "Memory tests mat4:" << std::endl;

	memoryTest(ft::mat4(), glm::mat4(), "Default constructor");

	ft::mat4 ft_mat4_1(1.0f);
	glm::mat4 glm_mat4_1(1.0f);
	memoryTest(ft_mat4_1, glm_mat4_1, "Constructor with one argument");

	ft::mat4 ft_mat4_2 = ft_mat4_1;
	glm::mat4 glm_mat4_2 = glm_mat4_1;
	memoryTest(ft_mat4_2, glm_mat4_2, "Copy constructor");

	ft::mat4 ft_mat4_3;
	glm::mat4 glm_mat4_3;
	ft_mat4_3 = ft_mat4_1;
	glm_mat4_3 = glm_mat4_1;
	memoryTest(ft_mat4_3, glm_mat4_3, "Copy assignment operator");

	ft::mat4 ft_mat4_4 = ft_mat4_1.operator*<float>(2.0f);
	glm::mat4 glm_mat4_4 = glm_mat4_1 * 2.0f;
	memoryTest(ft_mat4_4, glm_mat4_4, "Scalar multiplication");

	ft::mat4 ft_mat4_5 = { 1.0f, 2.0f, 3.0f, 4.0f,
						   5.0f, 6.0f, 7.0f, 8.0f,
						   9.0f, 10.0f, 11.0f, 12.0f,
						   13.0f, 14.0f, 15.0f, 16.0f };
	glm::mat4 glm_mat4_5 = { 1.0f, 2.0f, 3.0f, 4.0f,
							 5.0f, 6.0f, 7.0f, 8.0f,
							 9.0f, 10.0f, 11.0f, 12.0f,
							 13.0f, 14.0f, 15.0f, 16.0f };
	memoryTest(ft_mat4_5, glm_mat4_5, "Initializer list constructor");

	ft::mat4 ft_mat4_6 = { 3.0f, 5.0f, 7.0f, 9.0f,
						   11.0f, 13.0f, 15.0f, 17.0f,
						   19.0f, 21.0f, 23.0f, 25.0f,
						   27.0f, 29.0f, 31.0f, 33.0f };
	glm::mat4 glm_mat4_6 = { 3.0f, 5.0f, 7.0f, 9.0f,
							 11.0f, 13.0f, 15.0f, 17.0f,
							 19.0f, 21.0f, 23.0f, 25.0f,
							 27.0f, 29.0f, 31.0f, 33.0f };
	ft::mat4 ft_mat4_7 = ft_mat4_5 * ft_mat4_6;
	glm::mat4 glm_mat4_7 = glm_mat4_5 * glm_mat4_6;
	memoryTest(ft_mat4_7, glm_mat4_7, "Matrix multiplication");

	// ft::vec4 ft_vec4_1 = ft::operator*(ft_mat4_5, ft::vec4(1.0f, 2.0f, 3.0f, 4.0f));
	// glm::vec4 glm_vec4_1 = glm_mat4_5 * glm::vec4(1.0f, 2.0f, 3.0f, 4.0f);
	// memoryTest(ft_vec4_1, glm_vec4_1, "Matrix-vector multiplication");
	// log_vec4(ft_vec4_1);
	// log_vec4(glm_vec4_1);
}

void test_ft_glm() {

	test_vec(2, 1.0f, 2.0f);
	test_vec(3, 1.0f, 2.0f, 3.0f);
	test_vec(4, 1.0f, 2.0f, 3.0f, 4.0f);
	test_mat4();
}

#endif // FT_GLM_TEST_HPP
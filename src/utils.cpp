#include <random>
#include <vector>

#include "ft_glm/ft_glm.hpp"

template<typename T>
inline T rand(T min, T max) {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_real_distribution<T> dis(min, max);
	return dis(gen);
}

ft::vec3 randomColor() {
	return ft::vec3(rand(0.0f, 1.0f), rand(0.0f, 1.0f), rand(0.0f, 1.0f));
}
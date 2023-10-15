#include "application.hpp"

#include <chrono>

float Application::getTime() {
	/* Calculate the time in seconds since the application started */
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	return std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
}

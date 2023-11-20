#pragma once

#include <vector>

#include "vertex.hpp"

/**
 * @brief A vector of vertex. It is a wrapper around std::vector<Vertex> principally to prevent data copying.
*/
class Vertices: public std::vector<Vertex> {

public:

	Vertices() = default;

	Vertices(const Vertices&) = delete;
	Vertices& operator=(const Vertices&) = delete;

	Vertices(Vertices&& other):
		std::vector<Vertex>(std::move(other)) {
	}

	Vertices& operator=(Vertices&& other) {
		if (this != &other) {
			std::vector<Vertex>::operator=(std::move(other));
		}
		return *this;
	}

	~Vertices() = default;

};
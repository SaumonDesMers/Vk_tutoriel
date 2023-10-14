#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "vertex.hpp"

#include <iostream>
#include <vector>

class Object {

private:

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

public:

	Object() = delete;
	Object(const Object&) = delete;
	Object& operator=(const Object&) = delete;

	Object(Object&& other) {
		this->vertices = std::move(other.vertices);
		this->indices = std::move(other.indices);

		// other.vertices.data() = nullptr;
		// other.indices.data() = nullptr;
	}

	Object& operator=(Object&& other) {
		if (this != &other) {
			this->vertices = std::move(other.vertices);
			this->indices = std::move(other.indices);

			// other.vertices.data() = nullptr;
			// other.indices.data() = nullptr;
		}
		return *this;
	}

	Object(std::vector<Vertex>&& vertices, std::vector<uint32_t>&& indices) {
		this->vertices = vertices;
		this->indices = indices;

		// vertices.data() = nullptr;
		// indices.data() = nullptr;
	}

	~Object() {}

	void setVertices(std::vector<Vertex>&& vertices) {
		this->vertices = vertices;
		// vertices.data() = nullptr;
	}

	void setIndices(std::vector<uint32_t>&& indices) {
		this->indices = indices;
		// indices.data() = nullptr;
	}

	const std::vector<Vertex>& getVertices() const {
		return this->vertices;
	}

	const std::vector<uint32_t>& getIndices() const {
		return this->indices;
	}

};

#endif
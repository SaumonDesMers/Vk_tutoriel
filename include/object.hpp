#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "vertex.hpp"

#include <iostream>
#include <vector>

class Object {

private:

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	ft::vec3 baricenter;

public:

	ft::vec3 position;
	ft::vec3 rotation;
	ft::vec3 scale;

	Object() = delete;
	Object(const Object&) = delete;
	Object& operator=(const Object&) = delete;

	Object(Object&& other):
		vertices(std::move(other.vertices)),
		indices(std::move(other.indices)),
		baricenter(other.baricenter),
		position(other.position),
		rotation(other.rotation),
		scale(other.scale) {
	}

	Object& operator=(Object&& other) {
		if (this != &other) {
			this->vertices = std::move(other.vertices);
			this->indices = std::move(other.indices);
			this->position = other.position;
			this->rotation = other.rotation;
			this->scale = other.scale;
			this->baricenter = other.baricenter;
		}
		return *this;
	}

	Object(std::vector<Vertex>&& vertices, std::vector<uint32_t>&& indices) {
		this->vertices = vertices;
		this->indices = indices;
		this->calculateBaricenter();
	}

	void setVertices(std::vector<Vertex>&& vertices) {
		this->vertices = vertices;
		this->calculateBaricenter();
	}

	void setIndices(std::vector<uint32_t>&& indices) {
		this->indices = indices;
	}

	const std::vector<Vertex>& getVertices() const {
		return this->vertices;
	}

	const std::vector<uint32_t>& getIndices() const {
		return this->indices;
	}

	const ft::vec3& getBaricenter() const {
		return this->baricenter;
	}

	ft::vec3& getBaricenter() {
		return this->baricenter;
	}

private:

	void calculateBaricenter() {
		ft::vec3 sum = ft::vec3(0.0f, 0.0f, 0.0f);
		for (const Vertex& vertex : this->vertices) {
			sum += vertex.pos;
		}
		this->baricenter = sum / static_cast<float>(this->vertices.size());
	}

};

#endif
#ifndef MODEL_LOADING_HPP
#define MODEL_LOADING_HPP

#include "vertex.hpp"
#include "utils.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>
#include <regex>
#include <random>

static std::vector<std::string> split(std::string str, std::string delim) {
	std::vector<std::string> tab;
	char *c_str = const_cast<char *>(str.c_str());
	char *token = std::strtok(c_str, delim.c_str());
	while(token != NULL) {
		tab.push_back(token);
		token = std::strtok(NULL, delim.c_str());
	}
	return tab;
}

struct Face {
	uint32_t vertexIndex[3] = {0};
	uint32_t texCoordIndex[3] = {0};
	uint32_t normalIndex[3] = {0};

	void log() const {
		std::cout << "f";
		for (size_t i = 0; i < 3; i++) {
			std::cout << " " << vertexIndex[i] << "/" << texCoordIndex[i] << "/" << normalIndex[i];
		}
		std::cout << std::endl;
	}
};

class ObjLoader {

public:

	void loadModel(const std::string& path) {
		this->path = path;

		this->readFile();
		this->removeComments();
		this->parse();
	}

	std::unique_ptr<Object> createObject() {
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		this->populateVerticesAndIndices(vertices, indices);

		return std::make_unique<Object>(std::move(vertices), std::move(indices));
	}

private:

	std::string path;
	std::vector<std::string> lines;

	std::vector<ft::vec3> vertices;
	std::vector<ft::vec2> texCoords;
	std::vector<ft::vec3> normals;

	std::vector<Face> faces;

	bool hasTexCoords = false;
	bool hasNormals = false;


	void readFile() {
		std::ifstream file(this->path);
		if (!file.is_open()) {
			throw std::runtime_error("Failed to open file");
		}

		std::string line;
		while (std::getline(file, line)) {
			this->lines.push_back(line);
		}
	}

	void removeComments() {
		for (size_t i = 0; i < this->lines.size(); i++) {
			std::string line = this->lines[i];
			size_t commentPos = line.find('#');
			if (commentPos != std::string::npos) {
				line = line.substr(0, commentPos);
			}
			this->lines[i] = line;
		}
	}

	void parse() {

		std::regex faceRegex_v(R"(^f(\s\d+){3,}$)");
		std::regex faceRegex_v_vt(R"(^f(\s\d+\/\d+){3,}$)");
		std::regex faceRegex_v_vn(R"(^f(\s\d+\/\/\d+){3,}$)");
		std::regex faceRegex_v_vt_vn(R"(^f(\s\d+\/\d+\/\d+){3,}$)");

		for (uint32_t i = 0; i < this->lines.size(); i++) {

			/* Skip empty lines or lines with only spaces */
			if (std::all_of(this->lines[i].begin(), this->lines[i].end(), isspace)) {
				continue;
			}
			
			std::string line = this->lines[i];
			std::stringstream ss(line);
			ss.exceptions(std::ios_base::failbit | std::ios_base::badbit);

			try {
			
			if (memcmp(line.c_str(), "v ", 2) == 0) {
				ft::vec3 vertex;
				ss.ignore(2);
				ss >> vertex[0] >> vertex[1] >> vertex[2];
				this->vertices.push_back(vertex);
			}
			else if (memcmp(line.c_str(), "vt ", 3) == 0) {
				ft::vec2 texCoord;
				ss.ignore(3);
				ss >> texCoord[0] >> texCoord[1];
				this->texCoords.push_back(texCoord);
				this->hasTexCoords = true;
			}
			else if (memcmp(line.c_str(), "vn ", 3) == 0) {
				ft::vec3 normal;
				ss.ignore(3);
				ss >> normal[0] >> normal[1] >> normal[2];
				this->normals.push_back(normal);
				this->hasNormals = true;
			}
			else if (memcmp(line.c_str(), "f ", 2) == 0) {
				std::vector<Face> tmp_faces;

				/* Check which face regex matches */
				bool v = false, v_vt = false, v_vn = false, v_vt_vn = false;
				if (std::regex_match(line, faceRegex_v))
					v = true;
				else if (std::regex_match(line, faceRegex_v_vt) && hasTexCoords)
					v_vt = true;
				else if (std::regex_match(line, faceRegex_v_vn) && hasNormals)
					v_vn = true;
				else if (std::regex_match(line, faceRegex_v_vt_vn) && hasTexCoords && hasNormals)
					v_vt_vn = true;
				else
					throw std::string("Parsing syntax error: Invalid face format");

				ss.ignore(2);

				/* Read each vertex of the face */
				size_t j = 0;
				while (ss.rdstate() == std::ios_base::goodbit) {
					Face face;
					char c;
					if (v)
						ss >> face.vertexIndex[0];
					else if (v_vt)
						ss >> face.vertexIndex[0] >> c >> face.texCoordIndex[0];
					else if (v_vn)
						ss >> face.vertexIndex[0] >> c >> c >> face.normalIndex[0];
					else if (v_vt_vn)
						ss >> face.vertexIndex[0] >> c >> face.texCoordIndex[0] >> c >> face.normalIndex[0];

					if (this->checkIndices(face) == false) {
						throw std::string("Parsing value error");
					}

					tmp_faces.push_back(face);
					j++;
				}

				/* Construct a face for each triangle (e.i. 3 vertices) */
				for (j = 1; j + 1 < tmp_faces.size(); j++) {
					Face face;
					face.vertexIndex[0] = tmp_faces[0].vertexIndex[0];
					face.vertexIndex[1] = tmp_faces[j].vertexIndex[0];
					face.vertexIndex[2] = tmp_faces[j + 1].vertexIndex[0];
					if (v_vt || v_vt_vn) {
						face.texCoordIndex[0] = tmp_faces[0].texCoordIndex[0];
						face.texCoordIndex[1] = tmp_faces[j].texCoordIndex[0];
						face.texCoordIndex[2] = tmp_faces[j + 1].texCoordIndex[0];
					}
					if (v_vn || v_vt_vn) {
						face.normalIndex[0] = tmp_faces[0].normalIndex[0];
						face.normalIndex[1] = tmp_faces[j].normalIndex[0];
						face.normalIndex[2] = tmp_faces[j + 1].normalIndex[0];
					}
					this->faces.push_back(face);
				}
			}
			else if (
				memcmp(line.c_str(), "mtllib ", 7) != 0
				&& memcmp(line.c_str(), "usemtl ", 7) != 0
				&& memcmp(line.c_str(), "s ", 2) != 0
				&& memcmp(line.c_str(), "g ", 2) != 0
				&& memcmp(line.c_str(), "o ", 2) != 0
			) {
				throw std::string("Parsing syntax error: Unknown line type");
			}

			} catch (std::string& e) {
				throw std::runtime_error(this->path + ": line " + std::to_string(i + 1) + ": " + e);
			}
		}
	}

	bool checkIndices(const Face& face) {
		if (face.vertexIndex[0] > this->vertices.size() || face.vertexIndex[0] <= 0) {
			return false;
		}
		if (this->hasTexCoords && (face.texCoordIndex[0] > this->texCoords.size() || face.texCoordIndex[0] <= 0)) {
			return false;
		}
		if (this->hasNormals && (face.normalIndex[0] > this->normals.size() || face.normalIndex[0] <= 0)) {
			return false;
		}
		return true;
	}

	void populateVerticesAndIndices(
		std::vector<Vertex>& vertices,
		std::vector<uint32_t>& indices
	) {
		std::unordered_map<Vertex, uint32_t> uniqueVertices{};

		for (const auto& face : this->faces) {

			ft::vec3 randomFaceColor = randomColor();
			ft::vec2 arbitraryTexCoords[3] = {
				ft::vec2(0.0f, 0.0f),
				ft::vec2(1.0f, 0.0f),
				ft::vec2(0.0f, 1.0f)
			};

			for (size_t i = 0; i < 3; i++) {
				Vertex vertex{};

				vertex.pos = this->vertices[face.vertexIndex[i] - 1];

				vertex.color = randomFaceColor;

				if (this->hasTexCoords) {
					vertex.texCoord = this->texCoords[face.texCoordIndex[i] - 1];
				} else {
					vertex.texCoord = arbitraryTexCoords[i];
				}

				if (this->hasNormals) {
					vertex.normal = this->normals[face.normalIndex[i] - 1];
				}

				if (uniqueVertices.count(vertex) == 0) {
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}

				indices.push_back(uniqueVertices[vertex]);
			}
		}
	}

};

#endif // MODEL_LOADING_HPP
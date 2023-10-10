#ifndef MODEL_LOADING_HPP
#define MODEL_LOADING_HPP

#include "vertex.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <regex>

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
	uint32_t vertexIndex[3];
	uint32_t texCoordIndex[3];
	uint32_t normalIndex[3];

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
		std::cout << "Loading model from " << path << std::endl;

		this->readFile(path);
		this->removeComments();
		this->parse();
	}

	void populateVerticesAndIndices(
		std::vector<Vertex>& vertices,
		std::vector<uint32_t>& indices
	) {
		std::unordered_map<Vertex, uint32_t> uniqueVertices{};

		for (const auto& face : this->faces) {
			for (size_t i = 0; i < 3; i++) {
				Vertex vertex{};
				vertex.pos = this->vertices[face.vertexIndex[i] - 1];
				if (this->hasTexCoords) {
					vertex.texCoord = this->texCoords[face.texCoordIndex[i] - 1];
				}
				// if (this->hasNormals) {
				// 	vertex.normal = this->normals[face.normalIndex[i] - 1];
				// }
				if (uniqueVertices.count(vertex) == 0) {
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}
				indices.push_back(uniqueVertices[vertex]);
			}
		}
	}

private:

	std::vector<std::string> lines;

	std::vector<ft::vec3> vertices;
	std::vector<ft::vec2> texCoords;
	std::vector<ft::vec3> normals;

	std::vector<Face> faces;

	bool hasTexCoords = false;
	bool hasNormals = false;


	void readFile(const std::string& path) {
		std::ifstream file(path);
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

		std::regex vertexRegex(R"(^v(\s-?(\d+\.)?\d+){3,4}$)");
		std::regex texCoordRegex(R"(^vt(\s-?(\d+\.)?\d+){2}$)");
		std::regex normalRegex(R"(^vn(\s-?(\d+\.)?\d+){3}$)");
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
			std::vector<std::string> tokens = split(line, " ");

			if (std::regex_match(line, vertexRegex)) {
				this->parseVertex(tokens);
			}
			else if (std::regex_match(line, texCoordRegex)) {
				this->parseTexCoord(tokens);
				this->hasTexCoords = true;
			}
			else if (std::regex_match(line, normalRegex)) {
				this->parseNormal(tokens);
				this->hasNormals = true;
			}
			else if (std::regex_match(line, faceRegex_v)) {
				this->parseFace_v(tokens);
			}
			else if (std::regex_match(line, faceRegex_v_vt) && hasTexCoords) {
				this->parseFace_v_vt(tokens);
			}
			else if (std::regex_match(line, faceRegex_v_vn) && hasNormals) {
				this->parseFace_v_vn(tokens);
			}
			else if (std::regex_match(line, faceRegex_v_vt_vn) && hasTexCoords && hasNormals) {
				this->parseFace_v_vt_vn(tokens);
			}
			else {
				throw std::runtime_error("Line " + std::to_string(i) + ": Parsing syntax error");
			}
		}
	}

	void parseVertex(const std::vector<std::string>& tokens) {
		this->vertices.push_back(ft::vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3])));
	}

	void parseTexCoord(const std::vector<std::string>& tokens) {
		this->texCoords.push_back(ft::vec2(std::stof(tokens[1]), std::stof(tokens[2])));
	}

	void parseNormal(const std::vector<std::string>& tokens) {
		this->normals.push_back(ft::vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3])));
	}

	void parseFace_v(const std::vector<std::string>& tokens) {
		/* Construct a face for each triangle (e.i. 3 vertices) */
		for (size_t i = 1; i < tokens.size() - 2; i++) {
			Face face;
			face.vertexIndex[0] = std::stoi(tokens[1]);
			face.vertexIndex[1] = std::stoi(tokens[i + 1]);
			face.vertexIndex[2] = std::stoi(tokens[i + 2]);
			this->faces.push_back(face);
		}
	}

	void parseFace_v_vt(const std::vector<std::string>& tokens) {
		std::vector<std::vector<std::string>> subTokens;
		for (size_t i = 1; i < tokens.size(); i++) {
			/* tokens[i] is of the form vertexIndex/texCoordIndex */
			std::vector<std::string> tmp = split(tokens[i], "/");
			subTokens.push_back(tmp);
		}

		/* Construct a face for each triangle (e.i. 3 vertices) */
		for (size_t i = 0; i < subTokens.size() - 2; i++) {
			Face face;
			/* subToken[i] is of the form {vertexIndex, texCoordIndex} */
			face.vertexIndex[0] = std::stoi(subTokens[i][0]);
			face.vertexIndex[1] = std::stoi(subTokens[i + 1][0]);
			face.vertexIndex[2] = std::stoi(subTokens[i + 2][0]);
			face.texCoordIndex[0] = std::stoi(subTokens[i][1]);
			face.texCoordIndex[1] = std::stoi(subTokens[i + 1][1]);
			face.texCoordIndex[2] = std::stoi(subTokens[i + 2][1]);
			this->faces.push_back(face);
		}
	}

	void parseFace_v_vn(const std::vector<std::string>& tokens) {
		std::vector<std::vector<std::string>> subTokens;
		for (size_t i = 1; i < tokens.size(); i++) {
			/* tokens[i] is of the form vertexIndex//normalIndex */
			std::vector<std::string> tmp = split(tokens[i], "//");
			subTokens.push_back(tmp);
		}

		/* Construct a face for each triangle (e.i. 3 vertices) */
		for (size_t i = 0; i < subTokens.size() - 2; i++) {
			Face face;
			/* subToken[i] is of the form {vertexIndex, normalIndex} */
			face.vertexIndex[0] = std::stoi(subTokens[i][0]);
			face.vertexIndex[1] = std::stoi(subTokens[i + 1][0]);
			face.vertexIndex[2] = std::stoi(subTokens[i + 2][0]);
			face.normalIndex[0] = std::stoi(subTokens[i][1]);
			face.normalIndex[1] = std::stoi(subTokens[i + 1][1]);
			face.normalIndex[2] = std::stoi(subTokens[i + 2][1]);
			this->faces.push_back(face);
		}
	}

	void parseFace_v_vt_vn(const std::vector<std::string>& tokens) {
		std::vector<std::vector<std::string>> subTokens;
		for (size_t i = 1; i < tokens.size(); i++) {
			/* tokens[i] is of the form vertexIndex/texCoordIndex/normalIndex */
			std::vector<std::string> tmp = split(tokens[i], "/");
			subTokens.push_back(tmp);
		}

		/* Construct a face for each triangle (e.i. 3 vertices) */
		for (size_t i = 0; i < subTokens.size() - 2; i++) {
			Face face;
			/* subToken[i] is of the form {vertexIndex, texCoordIndex, normalIndex} */
			face.vertexIndex[0] = std::stoi(subTokens[i][0]);
			face.vertexIndex[1] = std::stoi(subTokens[i + 1][0]);
			face.vertexIndex[2] = std::stoi(subTokens[i + 2][0]);
			face.texCoordIndex[0] = std::stoi(subTokens[i][1]);
			face.texCoordIndex[1] = std::stoi(subTokens[i + 1][1]);
			face.texCoordIndex[2] = std::stoi(subTokens[i + 2][1]);
			face.normalIndex[0] = std::stoi(subTokens[i][2]);
			face.normalIndex[1] = std::stoi(subTokens[i + 1][2]);
			face.normalIndex[2] = std::stoi(subTokens[i + 2][2]);
			this->faces.push_back(face);
		}
	}

};

#endif // MODEL_LOADING_HPP
#pragma once

#include"base/vertex.h"
#include<vector>
#include"base/glsl_program.h"

class Mesh {
private:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	unsigned int VAO, VBO, EBO;
	void SetUpMesh();
public:
	Mesh(std::vector<Vertex> v, std::vector<unsigned int> i);
	~Mesh() = default;
	std::vector<Vertex> GetVertices() {
		return this->vertices;
	}
	std::vector<unsigned int> GetIndices() {
		return this->indices;
	}
	void Draw();
	unsigned int GetVao() {
		return this->VAO;
	}
	unsigned int GetVbo() {
		return this->VBO;
	}
	unsigned int GetEbo() {
		return this->EBO;
	}
};
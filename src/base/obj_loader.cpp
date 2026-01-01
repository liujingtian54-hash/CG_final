#include"obj_loader.h"
#include"vertex.h"
#include<iostream>
#include<string>
#include<glm/glm.hpp>
#include<vector>
#include<unordered_map>

std::vector<std::string> string_split(std::string& inputstring, char split, bool check_empty_string = false) {
	int i = 0;
	int st = 0;
	std::vector < std::string> out_vec;
	for (int i = 0; i < inputstring.size(); ++i) {
		if (inputstring[i] == split) {
			std::string temp_str = inputstring.substr(st, i - st);
			if(!check_empty_string || !temp_str.empty())
				out_vec.push_back(temp_str);
			st = i + 1;
		}
	}
	std::string temp_str = inputstring.substr(st,inputstring.size()-st);
	if (!temp_str.empty())
		out_vec.push_back(temp_str);
	return out_vec;
}

bool ObjLoader::LoadObj(std::string& path,
	std::vector<Vertex>& out_vertices,
	std::vector<unsigned int>& out_indices) {
	std::vector<glm::vec3> temp_positions;
	std::vector<glm::vec2> temp_texCoords;
	std::vector<glm::vec3> temp_normals;
	std::unordered_map<Vertex, unsigned int> vertexCache;
	std::string s00;
	std::vector<std::string> s00_s;
	
	std::ifstream file(path);
	if (!file.is_open()) {
		std::cerr << "Failed to open file: " << path << std::endl;
		return false;
	}

	while (std::getline(file, s00)) {
		if (s00.empty())
			continue;
		s00_s = string_split(s00, ' ', true);
		if (s00_s.empty())
			continue;
		if (s00_s[0] == "v") {
			if (s00_s.size() < 4)
				continue;
			glm::vec3 pos;
			pos.x = std::stof(s00_s[1]);
			pos.y = std::stof(s00_s[2]);
			pos.z = std::stof(s00_s[3]);
			temp_positions.push_back(pos);
		}
		else if (s00_s[0] == "vt") {
			if (s00_s.size() < 4)
				continue;
			glm::vec2 uv;
			uv.x = std::stof(s00_s[1]);
			uv.y = std::stof(s00_s[2]);
			temp_texCoords.push_back(uv);
		}
		else if (s00_s[0] == "vn") {
			if (s00_s.size() < 4)
				continue;
			glm::vec3 norm;
			norm.x = std::stof(s00_s[1]);
			norm.y = std::stof(s00_s[2]);
			norm.z = std::stof(s00_s[3]);
			temp_normals.push_back(norm);
		}
		else if (s00_s[0] == "f") {
			std::vector<Vertex> faceVertices;
			for (int i = 1; i < s00_s.size(); ++i) {
				std::vector<std::string> idxs = string_split(s00_s[i], '/');
				Vertex temp_vertex;
				temp_vertex.position = glm::vec3(0.0f);
				temp_vertex.texCoord = glm::vec2(0.0f);
				temp_vertex.normal = glm::vec3(0.0f);
				int vIdx = std::stoi(idxs[0]) - 1;
				if (vIdx >= 0 && vIdx < temp_positions.size())
					temp_vertex.position = temp_positions[vIdx];
				if (idxs.size() > 1 && !idxs[1].empty()) {
					int vtIdx = std::stoi(idxs[1]) - 1; 
					if (vtIdx >= 0 && vtIdx < temp_texCoords.size())
						temp_vertex.texCoord = temp_texCoords[vtIdx];
				}
				if (idxs.size() > 2 && !idxs[2].empty()) {
					int vnIdx = std::stoi(idxs[2]) - 1;
					if (vnIdx >= 0 && vnIdx < temp_normals.size())
						temp_vertex.normal = temp_normals[vnIdx];
				}
				faceVertices.push_back(temp_vertex);
			}
			if (faceVertices.size() >= 3) {
				for (size_t i = 1; i < faceVertices.size() - 1; ++i) {
					Vertex tri[3] = { faceVertices[0], faceVertices[i], faceVertices[i + 1] };
					for (int j = 0; j < 3; ++j) {
						if (vertexCache.find(tri[j]) != vertexCache.end()) {
							out_indices.push_back(vertexCache[tri[j]]);
						}
						else {
							unsigned int newIndex = (unsigned int)out_vertices.size();
							out_vertices.push_back(tri[j]);
							out_indices.push_back(newIndex);
							vertexCache[tri[j]] = newIndex;
						}
					}
				}
			}
		}
		else
			continue;
	}

	return true;
}
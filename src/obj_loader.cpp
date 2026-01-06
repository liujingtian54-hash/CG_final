#include"obj_loader.h"
#include"mesh.h"
#include"resource_manager.h"
#include"base/vertex.h"
#include<iostream>
#include<fstream>
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

bool ObjLoader::LoadObj(const std::string& path,
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
		else if (s00_s[0] == "mtllib") {
			std::string mtlPath = path.substr(0, path.find_last_of("/") + 1) + s00_s[1];
			std::ifstream mtlFile(mtlPath);
			if (!mtlFile.is_open()) {
				std::cerr << "Failed to open MTL file: " << mtlPath << std::endl;
				continue;
			}
			std::string s01;
			MixMaterial* material = new MixMaterial(ResourceManager::GetShader("Blinn_Phongshader"), ResourceManager::GetTexture("white"), ResourceManager::GetTexture("white"));
			while(std::getline(mtlFile, s01)) {
				if (s01.empty())
					continue;
				std::vector<std::string> s01_s = string_split(s01, ' ', true);
				if (s01_s.empty())
					continue;
				if (s01_s[0] == "Ka") {
					if (s01_s.size() < 4)
						continue;
					material->ka[0] = std::stof(s01_s[1]);
					material->ka[1] = std::stof(s01_s[2]);
					material->ka[2] = std::stof(s01_s[3]);
				}
				else if (s01_s[0] == "Kd") {
					if (s01_s.size() < 4)
						continue;
					material->kds1[0] = std::stof(s01_s[1]);
					material->kds1[1] = std::stof(s01_s[2]);
					material->kds1[2] = std::stof(s01_s[3]);
				}
				else if (s01_s[0] == "Ks") {
					if (s01_s.size() < 4)
						continue;
					material->ks[0] = std::stof(s01_s[1]);
					material->ks[1] = std::stof(s01_s[2]);
					material->ks[2] = std::stof(s01_s[3]);
				}
				else
					continue;
			}
			ResourceManager::AddMaterial(s00_s[1].substr(0,s00_s[1].size()-4), material);
			mtlFile.close();
		}
		else
			continue;
	}
	return true;
}

bool ObjLoader::ExportObj(const std::string& mesh_name, const std::string& material_name, const std::string& path) {
	std::ofstream file(path);
	if (!file.is_open()) {
		std::cerr << "Failed to open file for writing: " << path << std::endl;
		return false;
	}
	file << "# Exported OBJ file: " << mesh_name << "\n";
	file << "mtllib " << material_name << ".mtl\n";
	Mesh* mesh = ResourceManager::GetMesh(mesh_name);
	for(auto& iter : mesh->GetVertices()) {
		file << "v " << iter.position.x << " " << iter.position.y << " " << iter.position.z << "\n";
	}
	for (auto& iter : mesh->GetVertices()) {
		file << "vn " << iter.normal.x << " " << iter.normal.y << " " << iter.normal.z << "\n";
	}
	for (auto& iter : mesh->GetVertices()) {
		file << "vt " << iter.texCoord.x << " " << iter.texCoord.y << "\n";
	}
	for (size_t i = 0; i < mesh->GetIndices().size(); i += 3) {
		unsigned int idx0 = mesh->GetIndices()[i] + 1;
		unsigned int idx1 = mesh->GetIndices()[i + 1] + 1;
		unsigned int idx2 = mesh->GetIndices()[i + 2] + 1;
		file << "f " << idx0 << "/" << idx0 << "/" << idx0 << " "
			<< idx1 << "/" << idx1 << "/" << idx1 << " "
			<< idx2 << "/" << idx2 << "/" << idx2 << "\n";
	}

	file.close();

	std::ofstream mtlFile(path.substr(0, path.find_last_of(".") + 1) + "mtl");
	if (!mtlFile.is_open()) {
		std::cerr << "Failed to open MTL file for writing: " << path << std::endl;
		return false;
	}
	MixMaterial* material = ResourceManager::GetMaterial(material_name);
	mtlFile << "# Exported MTL file: " << material_name << "\n";
	mtlFile << "newmtl " << material_name << "\n";
	mtlFile << "Ka " << material->ka[0] << " " << material->ka[1] << " " << material->ka[2] << "\n";
	mtlFile << "Kd " << material->kds1[0] << " " << material->kds1[1] << " " << material->kds1[2] << "\n";
	mtlFile << "Ks " << material->ks[0] << " " << material->ks[1] << " " << material->ks[2] << "\n";
	mtlFile.close();

	return true;
}
#include"obj_loader.h"
#include"mesh.h"
#include"material.h"
#include"model.h"
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

std::string GetDirectory(const std::string& path) {
	size_t pos = path.find_last_of("/\\");
	return (pos != std::string::npos) ? path.substr(0, pos + 1) : "";
}

std::string GetModelName(const std::string& path) {
	size_t lastSlash = path.find_last_of("/\\");
	size_t start = (lastSlash == std::string::npos) ? 0 : lastSlash + 1;

	size_t lastDot = path.find_last_of(".");
	size_t count = (lastDot == std::string::npos || lastDot < start) ? std::string::npos : lastDot - start;

	return path.substr(start, count);
}

void LoadMtlLib(const std::string& directory, const std::string& filename, const std::string& modelPrefix) {
	std::string path = directory + filename;
	std::ifstream file(path);
	if (!file.is_open()) {
		std::cerr << "Failed to open MTL file: " << path << std::endl;
		return;
	}
	std::string currentMtlName = "";
	glm::vec3 kd(1.0f); // 默认白
	glm::vec3 ks(0.0f); // 默认黑
	float ns = 32.0f;
	std::string mapKdPath = "";

	auto SaveCurrentMaterial = [&]() {
		if (currentMtlName.empty()) return;
		std::string uniqueMaterialName = modelPrefix + "_" + currentMtlName;
		if (ResourceManager::GetMaterial(uniqueMaterialName)) return;
		std::cout << uniqueMaterialName << std::endl;
		MixMaterial* mat = new MixMaterial(ResourceManager::GetShader("Blinn_Phongshader"), ResourceManager::GetTexture("white"), ResourceManager::GetTexture("white"));
		mat->kds1 = kd;
		mat->ks = ks;
		mat->N = ns;

		if (!mapKdPath.empty()) {
			mat->tex1 = ResourceManager::LoadTexture(currentMtlName + "_diffuse", directory + mapKdPath);
		}
		else {
			mat->tex1 = ResourceManager::GetTexture("white");
		}
		mat->tex2 = ResourceManager::GetTexture("white");
		ResourceManager::AddMaterial(uniqueMaterialName, mat);
		};
	std::string line;
	while (std::getline(file, line)) {
		if (line.empty()) continue;
		std::vector<std::string> tokens = string_split(line, ' ', true);
		if (tokens.empty()) continue;

		if (tokens[0] == "newmtl") {
			SaveCurrentMaterial();
			currentMtlName = tokens[1];
			kd = glm::vec3(1.0f);
			ks = glm::vec3(0.0f);
			ns = 32.0f;
			mapKdPath = "";
		}
		else if (tokens[0] == "Kd") {
			kd = glm::vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
		}
		else if (tokens[0] == "Ks") {
			ks = glm::vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
		}
		else if (tokens[0] == "Ns") {
			ns = std::stof(tokens[1]);
		}
		else if (tokens[0] == "map_Kd") {
			mapKdPath = tokens[1];
		}
	}
	SaveCurrentMaterial();

	file.close();
}

bool ObjLoader::ExportGeometry(const std::string& filename,
	const std::vector<Vertex>& vertices,
	const std::vector<unsigned int>& indices,
	const std::string& materialName) {
	std::ofstream file(filename);
	if (!file.is_open()) {
		std::cerr << "Failed to open file for writing: " << filename << std::endl;
		return false;
	}

	// 写入OBJ文件头
	file << "# Exported Geometry\n";
	file << "mtllib " << materialName << ".mtl\n";

	// 写入顶点位置
	for (const auto& vertex : vertices) {
		file << "v " << vertex.position.x << " "
			<< vertex.position.y << " "
			<< vertex.position.z << "\n";
	}

	// 写入纹理坐标
	for (const auto& vertex : vertices) {
		file << "vt " << vertex.texCoord.x << " "
			<< vertex.texCoord.y << "\n";
	}

	// 写入法线
	for (const auto& vertex : vertices) {
		file << "vn " << vertex.normal.x << " "
			<< vertex.normal.y << " "
			<< vertex.normal.z << "\n";
	}

	// 写入面
	for (size_t i = 0; i < indices.size(); i += 3) {
		if (i + 2 < indices.size()) {
			unsigned int idx0 = indices[i] + 1;
			unsigned int idx1 = indices[i + 1] + 1;
			unsigned int idx2 = indices[i + 2] + 1;

			file << "f " << idx0 << "/" << idx0 << "/" << idx0 << " "
				<< idx1 << "/" << idx1 << "/" << idx1 << " "
				<< idx2 << "/" << idx2 << "/" << idx2 << "\n";
		}
	}

	file.close();

	// 创建对应的MTL文件
	std::string mtlFilename = filename.substr(0, filename.find_last_of('.')) + ".mtl";
	std::ofstream mtlFile(mtlFilename);

	if (!mtlFile.is_open()) {
		std::cerr << "Failed to create MTL file: " << mtlFilename << std::endl;
		return false;
	}

	mtlFile << "# Exported Material\n";
	mtlFile << "newmtl " << materialName << "\n";
	mtlFile << "Ka 0.2 0.2 0.2\n";  // 环境光
	mtlFile << "Kd 0.8 0.8 0.8\n";  // 漫反射
	mtlFile << "Ks 0.5 0.5 0.5\n";  // 镜面反射
	mtlFile << "Ns 32.0\n";         // 高光指数

	mtlFile.close();

	std::cout << "Geometry exported successfully: " << filename << std::endl;
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
	for (auto& iter : mesh->GetVertices()) {
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
	mtlFile << "Ka " << 0.0000 << " " << 0.0000 << " " << 0.0000 << "\n";
	mtlFile << "Kd " << material->kds1[0] << " " << material->kds1[1] << " " << material->kds1[2] << "\n";
	mtlFile << "Ks " << material->ks[0] << " " << material->ks[1] << " " << material->ks[2] << "\n";
	mtlFile.close();

	return true;
}



Model* ObjLoader::LoadObj(const std::string& path) {
	std::vector<glm::vec3> temp_positions;
	std::vector<glm::vec2> temp_texCoords;
	std::vector<glm::vec3> temp_normals;

	std::vector<Vertex> currentVertices;
	std::vector<unsigned int> currentIndices;
	std::unordered_map<Vertex, unsigned int> vertexCache;
	std::string currentMaterialName = "default";
	std::string directory = GetDirectory(path);
	std::string modelPrefix = GetModelName(path);
	Model* model = new Model(CollisionDetector());

	std::ifstream file(path);
	if (!file.is_open()) {
		std::cerr << "[ObjLoader::LoadObj]Failed to open file: " << path << std::endl;
		delete model;
		return false;
	}

	auto FlushMesh = [&]() {
		if (currentVertices.empty()) return;
		Mesh* mesh = new Mesh(currentVertices, currentIndices);
		std::string uniqueMatName = modelPrefix + "_" + currentMaterialName;
		MixMaterial* mat = dynamic_cast<MixMaterial*>(ResourceManager::GetMaterial(uniqueMatName));
		if (!mat) {
			// 尝试加载一个默认材质
			mat = ResourceManager::GetMaterial("white_material"); 
			std::cerr << "[ObjLoader::LoadObj(FlushMesh)]Warning: Material not found: " << uniqueMatName << std::endl;
		}
		model->AddMesh(mesh, mat, glm::vec3(0.0f));
		currentVertices.clear();
		currentIndices.clear();
		vertexCache.clear();
		};

	std::string line;
	while (std::getline(file, line)) {
		if (line.empty()) continue;
		std::vector<std::string> tokens = string_split(line, ' ', true);
		if (tokens.empty()) continue;

		if (tokens[0] == "v") {
			temp_positions.emplace_back(
				std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
		}
		else if (tokens[0] == "vt") {
			temp_texCoords.emplace_back(
				std::stof(tokens[1]), std::stof(tokens[2]));
		}
		else if (tokens[0] == "vn") {
			temp_normals.emplace_back(
				std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
		}
		else if (tokens[0] == "mtllib") {
			LoadMtlLib(directory, tokens[1], modelPrefix);
		}
		else if (tokens[0] == "usemtl") {
			FlushMesh();
			currentMaterialName = tokens[1];
		}
		else if (tokens[0] == "f") {
			std::vector<Vertex> faceVertices;
			for (size_t i = 1; i < tokens.size(); ++i) {
				std::vector<std::string> idxs = string_split(tokens[i], '/');

				Vertex v;
				v.position = glm::vec3(0.0f);
				v.texCoord = glm::vec2(0.0f);
				v.normal = glm::vec3(0.0f);

				if (idxs.size() > 0 && !idxs[0].empty()) {
					int idx = std::stoi(idxs[0]) - 1;
					if (idx >= 0 && idx < temp_positions.size())
						v.position = temp_positions[idx];
				}
				if (idxs.size() > 1 && !idxs[1].empty()) {
					int idx = std::stoi(idxs[1]) - 1;
					if (idx >= 0 && idx < temp_texCoords.size())
						v.texCoord = temp_texCoords[idx];
				}
				if (idxs.size() > 2 && !idxs[2].empty()) {
					int idx = std::stoi(idxs[2]) - 1;
					if (idx >= 0 && idx < temp_normals.size())
						v.normal = temp_normals[idx];
				}
				faceVertices.push_back(v);
			}
			for (size_t i = 1; i < faceVertices.size() - 1; ++i) {
				Vertex tri[3] = { faceVertices[0], faceVertices[i], faceVertices[i + 1] };
				for (int j = 0; j < 3; ++j) {
					if (vertexCache.count(tri[j])) {
						currentIndices.push_back(vertexCache[tri[j]]);
					}
					else {
						unsigned int newIdx = (unsigned int)currentVertices.size();
						currentVertices.push_back(tri[j]);
						currentIndices.push_back(newIdx);
						vertexCache[tri[j]] = newIdx;
					}
				}
			}
		}
	}
	FlushMesh();
	file.close();
	return model;
}

#pragma once

#include<vector>
#include<string>
#include"base/vertex.h"
#include<fstream>

class ObjLoader {
public:
	static bool LoadObj(const std::string& path,
		std::vector<Vertex>& out_vertices,
		std::vector<unsigned int>& out_indices);
	static bool ExportObj(const std::string& mesh_name, const std::string& material_name, const std::string& path);
};
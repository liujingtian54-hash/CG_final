#pragma once

#include<vector>
#include<string>
#include"base/vertex.h"
#include"model.h"
#include<fstream>

class ObjLoader {
public:
	static Model* LoadObj(const std::string& path);
	static bool ExportObj(const std::string& mesh_name, const std::string& material_name, const std::string& path);
};
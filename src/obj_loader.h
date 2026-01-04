#pragma once

#include<vector>
#include<string>
#include"base/vertex.h"

class ObjLoader {
public:
	static bool LoadObj(const std::string& path,
		std::vector<Vertex>& out_vertices,
		std::vector<unsigned int>& out_indices);
};
#pragma once

#include<vector>
#include<string>
#include"vertex.h"

class ObjLoader {
	bool LoadObj(std::string& path,
		std::vector<Vertex>& out_vertices,
		std::vector<unsigned int>& out_indices);
};
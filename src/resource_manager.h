#pragma once
#include"base/glsl_program.h"
#include"base/texture2d.h"
#include"mesh.h"
#include"material.h"
#include<map>
#include<string>

//TODO：可能加个load的接口给这些资源会比较好
class ResourceManager {
public:
	static std::map<std::string, Texture2D*> textures;
	static std::map<std::string, Mesh*> meshes;
	static std::map<std::string, Material*> materials;
	static std::map<std::string, GLSLProgram*> shaders;
	static void Clear() {
		for (auto& iter : shaders) delete iter.second;
		for (auto& iter : textures) delete iter.second;
		for (auto& iter : meshes) delete iter.second;
		for (auto& iter : materials) delete iter.second;
		shaders.clear();
		textures.clear();
		meshes.clear();
		materials.clear();
	}
private:
	ResourceManager();
};
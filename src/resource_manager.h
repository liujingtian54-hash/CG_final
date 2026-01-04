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
	static GLSLProgram* LoadShader(const std::string& name, const std::string& vShaderFile, const std::string& fShaderFile);
	static GLSLProgram* GetShader(const std::string& name);

	static Texture2D* LoadTexture(const std::string& name, const std::string& file);
	static Texture2D* GetTexture(const std::string& name);

	static Mesh* LoadMesh(const std::string& name, const std::string& file);
	static Mesh* GetMesh(const std::string& name);

	static Material* AddMaterial(const std::string& name, Material* material);
	static Material* CreateStandardMaterial(const std::string& name, GLSLProgram* shader, Texture2D* diffuse, glm::vec3 color = glm::vec3(1.0f));
	static Material* GetMaterial(const std::string& name);
private:
	ResourceManager();
};
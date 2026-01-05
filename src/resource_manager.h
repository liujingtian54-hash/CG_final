#pragma once
#include"base/glsl_program.h"
#include"base/texture2d.h"
#include"mesh.h"
#include"material.h"
#include"geometry_generator.h"
#include<map>
#include<string>

//TODO：可能加个load的接口给这些资源会比较好
class ResourceManager {
public:
	static std::map<std::string, Texture2D*> textures;
	static std::map<std::string, Mesh*> meshes;
	static std::map<std::string, MixMaterial*> materials;
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

	//name,path
	static Mesh* LoadMesh(const std::string& name, const std::string& file);
	static Mesh* LoadMesh(const std::string& name, ShapeType type, const std::vector<float>& params = {});
	static Mesh* LoadCube(const std::string& name, float size) {
		return LoadMesh(name, ShapeType::Cube, { size });
	}
	static Mesh* LoadSphere(const std::string& name, float radius, int segments) {
		return LoadMesh(name, ShapeType::Sphere, { radius, (float)segments });
	}
	static Mesh* LoadCylinder(const std::string& name, float radius, float height, int segments) {
		return LoadMesh(name, ShapeType::Cylinder, { radius, height, (float)segments });
	}
	static Mesh* LoadCone(const std::string& name, float radius, float height, int segments) {
		return LoadMesh(name, ShapeType::Cone, { radius, height, (float)segments });
	}
	static Mesh* LoadPrism(const std::string& name, int sides, float radius, float height) {
		return LoadMesh(name, ShapeType::Prism, { (float)sides, radius, height });
	}
	static Mesh* LoadFrustum(const std::string& name, int sides, float bottomRadius, float topRadius, float height) {
		return LoadMesh(name, ShapeType::Frustum, { (float)sides, bottomRadius, topRadius, height });
	}

	static Mesh* GetMesh(const std::string& name);
	static bool ExportMesh(const std::string& name, const std::string& path);

	static MixMaterial* AddMaterial(const std::string& name, MixMaterial* material);
	static MixMaterial* GetMaterial(const std::string& name);
private:
	ResourceManager();
};
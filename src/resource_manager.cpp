#include"base/glsl_program.h"
#include"base/texture2d.h"
#include"mesh.h"
#include"material.h"
#include"resource_manager.h"
#include"obj_loader.h"
#include<map>
#include<string>

std::map<std::string, Texture2D*> ResourceManager::textures;
std::map<std::string, Mesh*> ResourceManager::meshes;
std::map<std::string, Material*> ResourceManager::materials;
std::map<std::string, GLSLProgram*> ResourceManager::shaders;

GLSLProgram* ResourceManager::LoadShader(const std::string& name, const std::string& vShaderFile, const std::string& fShaderFile) {
    if (shaders.find(name) != shaders.end()) {
        return shaders[name];
    }

    GLSLProgram* shader = new GLSLProgram();
    try {
        shader->attachVertexShaderFromFile(vShaderFile);
        shader->attachFragmentShaderFromFile(fShaderFile);
        shader->link();
    }
    catch (std::exception& e) {
        std::cerr << "ERROR::SHADER::LOAD_FAILED: " << name << "\n" << e.what() << std::endl;
        delete shader;
        return nullptr;
    }

    shaders[name] = shader;
    return shader;
}

GLSLProgram* ResourceManager::GetShader(const std::string& name) {
    if (shaders.find(name) != shaders.end())
        return shaders[name];
    std::cerr << "WARNING::SHADER_NOT_FOUND: " << name << std::endl;
    return nullptr;
}

Texture2D* ResourceManager::LoadTexture(const std::string& name, const std::string& file) {
    if (textures.find(name) != textures.end())
        return textures[name];
    Texture2D* texture = new ImageTexture2D(file);
    textures[name] = texture;
    return texture;
}

Texture2D* ResourceManager::GetTexture(const std::string& name) {
    if (textures.find(name) != textures.end())
        return textures[name];
    std::cerr << "WARNING::TEXTURE_NOT_FOUND: " << name << std::endl;
    return nullptr;
}

// --- Mesh 实现 ---
Mesh* ResourceManager::LoadMesh(const std::string& name, const std::string& file) {
    if (meshes.find(name) != meshes.end())
        return meshes[name];

    // 使用 ObjLoader 加载数据
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // 调用你的 ObjLoader (假设是静态方法)
    bool success = ObjLoader::LoadObj(file, vertices, indices);

    if (!success) {
        std::cerr << "ERROR::MESH::LOAD_FAILED: " << file << std::endl;
        return nullptr;
    }

    // 创建 Mesh 对象 (假设 Mesh 构造函数接受 vertices 和 indices)
    Mesh* mesh = new Mesh(vertices, indices);

    meshes[name] = mesh;
    return mesh;
}

Mesh* ResourceManager::GetMesh(const std::string& name) {
    if (meshes.find(name) != meshes.end())
        return meshes[name];
    std::cerr << "WARNING::MESH_NOT_FOUND: " << name << std::endl;
    return nullptr;
}

Material* ResourceManager::AddMaterial(const std::string& name, Material* material) {
    if (materials.find(name) != materials.end()) {
        std::cerr << "WARNING::MATERIAL_ALREADY_EXISTS: " << name << " (Overwriting)" << std::endl;
        delete materials[name]; // 如果覆盖，先删除旧的防止泄漏
    }
    materials[name] = material;
    return material;
}

// 这是一个快捷辅助函数，用于快速创建标准材质
Material* ResourceManager::CreateStandardMaterial(const std::string& name, GLSLProgram* shader, Texture2D* diffuse, glm::vec3 color) {
    StandardMaterial* mat = new StandardMaterial(shader);
    mat->diffuseMap = diffuse;
    mat->color = color;

    return AddMaterial(name, mat);
}

Material* ResourceManager::GetMaterial(const std::string& name) {
    if (materials.find(name) != materials.end())
        return materials[name];
    std::cerr << "WARNING::MATERIAL_NOT_FOUND: " << name << std::endl;
    return nullptr;
}
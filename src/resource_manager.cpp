#include"base/glsl_program.h"
#include"base/texture2d.h"
#include"mesh.h"
#include"material.h"
#include"resourcemanager.h"
#include<map>
#include<string>

std::map<std::string, Texture2D*> ResourceManager::textures;
std::map<std::string, Mesh*> ResourceManager::meshes;
std::map<std::string, Material*> ResourceManager::materials;
std::map<std::string, GLSLProgram*> ResourceManager::shaders;
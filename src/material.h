#pragma once
#include <string>
#include <glm/glm.hpp>
#include "base/glsl_program.h"
#include "base/texture2d.h"

//TODO:后续要检查一下
// 1. 定义材质基类 (接口)
class Material {
public:
    // 每个材质必须持有一个 Shader
    GLSLProgram* shader;

    Material(GLSLProgram* program) : shader(program) {}
    virtual ~Material() = default;

    // 核心接口：每个子类自己决定怎么传数据给 Shader
    virtual void Setup() = 0;
};

// 2. 材质
//Texture2D* tex1 tex2 分别是两张纹理,通过blend参数来混合
//glm::vec3 kds1 kds2 ks 分别是两张纹理的漫反射系数和镜面反射系数
//float blend 是混合系数，范围在0到1之间
//float N 是镜面反射指数
class MixMaterial : public Material {
public:
    Texture2D* tex1;
    Texture2D* tex2;
    glm::vec3 kds1 = glm::vec3(1.0f,1.0f,1.0f);
    glm::vec3 kds2 = glm::vec3(1.0f,1.0f,1.0f);
	glm::vec3 ks = glm::vec3(0.0f, 0.0f, 0.0f);
    float blend = 0.0f;
	float N = 64.0f;

    MixMaterial(GLSLProgram* shader, Texture2D* t1, Texture2D* t2);
    ~MixMaterial() override;
    void Setup() override; 

    bool operator==(const MixMaterial& other) {
        return this->tex1 == other.tex1 &&
               this->tex2 == other.tex2 &&
               this->kds1 == other.kds1 &&
               this->kds2 == other.kds2 &&
               this->ks == other.ks &&
               this->blend == other.blend &&
			this->N == other.N;
	}
};
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

// 2. 实现一个最常用的标准材质
//并非标准，啊，并非标准，这东西就是ai的胡言乱语，设计的一塌糊涂要啥啥没有，不要用它
//还没被铲掉的理由只有防止别的地方有废弃的api在用这个东西
class StandardMaterial : public Material {
public:
    // 材质属性
    Texture2D* diffuseMap = nullptr; // 漫反射贴图
    Texture2D* specularMap = nullptr;// 高光贴图 (EX加分项可选)
    glm::vec3 color = glm::vec3(1.0f); // 基础颜色 tint
    float shininess = 32.0f;           // 光泽度

    StandardMaterial(GLSLProgram* shader) : Material(shader) {}

    // 实现 Setup
    void Setup() override {
        shader->use();

        // 绑定漫反射图到 0 号槽
        if (diffuseMap) {
            diffuseMap->bind(0);
            shader->setUniformInt("uMaterial.diffuseMap", 0);
            shader->setUniformBool("uMaterial.hasDiffuse", true); // 告诉shader我有图
        }
        else {
            shader->setUniformBool("uMaterial.hasDiffuse", false);
        }

        // 绑定高光图到 1 号槽 (如果有的话)
        if (specularMap) {
            specularMap->bind(1);
            shader->setUniformInt("uMaterial.specularMap", 1);
        }

        // 传递数值参数
        shader->setUniformVec3("uMaterial.color", color);
        shader->setUniformFloat("uMaterial.shininess", shininess);
    }
};

// 3. 这个大概是正常的材质
//其实也没多正常，不过当作普通的材质玩还是可以的，blend设0就行
//可以做一点混合纹理，比较酷
class MixMaterial : public Material {
public:
    Texture2D* tex1;
    Texture2D* tex2;
    glm::vec3 kds1 = glm::vec3(1.0f,1.0f,1.0f);
    glm::vec3 kds2 = glm::vec3(1.0f,1.0f,1.0f);
    float blend = 0.0f;
    float ka[3] = {0.0f,0.0f,0.0f};
    float ks[3] = {0.0f,0.0f,0.0f};

    MixMaterial(GLSLProgram* shader, Texture2D* t1, Texture2D* t2);
    ~MixMaterial() override;
    void Setup() override; 
};
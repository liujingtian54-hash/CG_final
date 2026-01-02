#pragma once
#include<glm/glm.hpp>
#include<vector>
#include"base/transform.h"
#include"mesh.h"
#include"base/texture2d.h"
#include"material.h"

enum class ObjectGroup {
    Object,
    Building,   // 房子、墙壁 (静态，需要碰撞检测)
    StreetLamp, // 路灯 (需要批量控制开关)
    Vehicle,    // 车辆 (会动)
    Player,     // 玩家
    Skybox      // 天空盒
};

class GameObject {
private:
	Transform transform;
    Mesh* mesh;
    Material* material;
public:
    GameObject(unsigned int uid, ObjectGroup g) : id(uid), group(g) {};
    ~GameObject() = default;
    void ApplyMesh(Mesh* m) {
        mesh = m;
    }
    Mesh* GetMesh() {
        return mesh;
    }
    void SetTransform(Transform t) {
        transform = t;
    }
    Transform GetTransform() {
        return transform;
    }
    void ApplyMaterial(Material* mat) {
        material = mat;
    }
    Material* GetMaterial() {
        return material;
    }
    void Draw() {
        if (mesh && material) {
            material->Setup();
            material->shader->setUniformMat4("model", transform.getLocalMatrix());
            mesh->Draw();
        }
    }

    const unsigned int id;
    ObjectGroup group;
};
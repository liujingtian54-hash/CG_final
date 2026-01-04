#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "base/transform.h"
#include "mesh.h"
#include "base/texture2d.h"
#include "material.h"

enum class ObjectGroup {
    Object,
    Building,   // 房子、墙壁 (静态，需要碰撞检测)
    StreetLamp, // 路灯 (需要批量控制开关)
    Vehicle,    // 车辆 (会动)
    Skybox      // 天空盒
};

class GameObject {
private:
    Transform transform;
    Mesh* mesh = nullptr;
    Material* material = nullptr;

public:
    GameObject(unsigned int uid, ObjectGroup g) : id(uid), group(g) {};
    ~GameObject() = default;

    // === 修正后的Transform操作方法 ===

    // 位置相关方法
    void SetPosition(const glm::vec3& position) {
        transform.position = position;  // 直接赋值
    }

    glm::vec3 GetPosition() const {  // 添加const
        return transform.position;  // 直接返回成员
    }

    void Translate(const glm::vec3& translation) {
        transform.position += translation;  // 直接相加
    }

    // 旋转相关方法
    void SetRotation(const glm::quat& rotation) {  // 使用glm::quat
        transform.rotation = rotation;
    }

    glm::quat GetRotation() const {  // 返回quat类型
        return transform.rotation;
    }

    // 获取欧拉角（如果需要的话）
    glm::vec3 GetRotationEuler() const {
        return glm::eulerAngles(transform.rotation);
    }

    void Rotate(float angle, const glm::vec3& axis) {
        glm::quat delta = glm::angleAxis(glm::radians(angle), glm::normalize(axis));
        transform.rotation = delta * transform.rotation;
    }

    // 缩放相关方法
    void SetScale(const glm::vec3& scale) {
        transform.scale = scale;
    }

    glm::vec3 GetScale() const {
        return transform.scale;
    }

    // 方向向量
    glm::vec3 GetFront() const {
        return transform.getFront();
    }

    glm::vec3 GetUp() const {
        return transform.getUp();
    }

    glm::vec3 GetRight() const {
        return transform.getRight();
    }

    // 观察目标
    void LookAt(const glm::vec3& target, const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f)) {
        transform.lookAt(target, up);
    }

    // 门开关动画相关
    bool isDoor = false;
    bool doorOpen = false;
    float doorAngle = 0.0f;

    void UpdateDoor(float deltaTime) {
        if (isDoor) {
            float targetAngle = doorOpen ? 90.0f : 0.0f;
            doorAngle += (targetAngle - doorAngle) * 5.0f * deltaTime;

            // 重置旋转然后添加门旋转
            transform.rotation = glm::angleAxis(glm::radians(doorAngle), glm::vec3(0, 1, 0));
        }
    }

    // === 原来的方法（保持不变，但添加const）===
    void ApplyMesh(Mesh* m) { mesh = m; }
    Mesh* GetMesh() const { return mesh; }  // 添加const

    void SetTransform(const Transform& t) { transform = t; }  // 修改参数类型
    Transform GetTransform() const { return transform; }  // 添加const

    void ApplyMaterial(Material* mat) { material = mat; }
    Material* GetMaterial() const { return material; }  // 添加const

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
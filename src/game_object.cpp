#include <glm/glm.hpp>
#include <vector>
#include "base/transform.h"
#include "mesh.h"
#include "base/texture2d.h"
#include "material.h"
#include "game_object.h"

void GameObject::SetPosition(const glm::vec3& position) {
    transform.position = position;  // 直接赋值
}
glm::vec3 GameObject::GetPosition() const {  // 添加const
    return transform.position;  // 直接返回成员
}
void GameObject::Translate(const glm::vec3& translation) {
    transform.position += translation;  // 直接相加
}
void GameObject::SetRotation(const glm::quat& rotation) {  // 使用glm::quat
    transform.rotation = rotation;
}
glm::quat GameObject::GetRotation() const {  // 返回quat类型
    return transform.rotation;
}
// 获取欧拉角（如果需要的话）
glm::vec3 GameObject::GetRotationEuler() const {
    return glm::eulerAngles(transform.rotation);
}
void GameObject::Rotate(float angle, const glm::vec3& axis) {
    glm::quat delta = glm::angleAxis(glm::radians(angle), glm::normalize(axis));
    transform.rotation = delta * transform.rotation;
}
// 缩放相关方法
void GameObject::SetScale(const glm::vec3& scale) {
    transform.scale = scale;
}
glm::vec3 GameObject::GetScale() const {
    return transform.scale;
}

// 方向向量
glm::vec3 GameObject::GetFront() const {
    return transform.getFront();
}

glm::vec3 GameObject::GetUp() const {
    return transform.getUp();
}

glm::vec3 GameObject::GetRight() const {
    return transform.getRight();
}

// 观察目标
void GameObject::LookAt(const glm::vec3& target, const glm::vec3& up) {
    transform.lookAt(target, up);
}

void GameObject::UpdateDoor(float deltaTime) {
    if (isDoor) {
        float targetAngle = doorOpen ? 90.0f : 0.0f;
        doorAngle += (targetAngle - doorAngle) * 5.0f * deltaTime;

        // 重置旋转然后添加门旋转
        transform.rotation = glm::angleAxis(glm::radians(doorAngle), glm::vec3(0, 1, 0));
    }
}

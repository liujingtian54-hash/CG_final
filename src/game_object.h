#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "base/transform.h"
#include "mesh.h"
#include "base/texture2d.h"
#include "material.h"

enum class ObjectGroup {
    Object,
    Building,   // building
    StreetLamp, // street lamps
    Player,     // player
    Skybox
};

class GameObject {
private:
    Transform transform;
    Mesh* mesh = nullptr;
    MixMaterial* material = nullptr;

public:
    const unsigned int id;
    ObjectGroup group;
    bool isDoor = false;
    bool doorOpen = false;
    float doorAngle = 0.0f;

    GameObject(unsigned int uid, ObjectGroup g) : id(uid), group(g),transform(),mesh(nullptr),material(nullptr) {};
    ~GameObject() = default;
    void ApplyMesh(Mesh* m) { mesh = m; }
    Mesh* GetMesh() const { return mesh; }
    void SetTransform(const Transform& t) { transform = t; }
    Transform GetTransform() const { return transform; }
    void ApplyMaterial(MixMaterial* mat) { material = mat; }
    MixMaterial* GetMaterial() const { return material; }
    void SetPosition(const glm::vec3& position);
    glm::vec3 GetPosition() const;
    void SetRotation(const glm::quat& rotation);
    glm::quat GetRotation() const;
    glm::vec3 GetRotationEuler() const;
    void SetScale(const glm::vec3& scale);
    glm::vec3 GetScale() const;

    void Translate(const glm::vec3& translation);
    void Rotate(float angle, const glm::vec3& axis);
    glm::vec3 GetFront() const;
    glm::vec3 GetUp() const;
    glm::vec3 GetRight() const;
    void LookAt(const glm::vec3& target, const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f));
    void UpdateDoor(float deltaTime);
    void Draw() {
        if (mesh && material) {
            material->Setup();
            material->shader->setUniformMat4("model", transform.getLocalMatrix());
            mesh->Draw();
        }
    }
};
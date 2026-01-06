#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "base/transform.h"
#include "mesh.h"
#include "base/texture2d.h"
#include "material.h"
#include "model.h"

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
    Model* model;

public:
    const unsigned int id;
    ObjectGroup group;
    bool isDoor = false;
    bool doorOpen = false;
    float doorAngle = 0.0f;

    GameObject(unsigned int uid, ObjectGroup g) : id(uid), group(g),transform(),model(nullptr) {};
    ~GameObject() = default;
	void AddModel(Model* m) { model = m; }
	Model* GetModel() const { return model; }
    void SetTransform(const Transform& t) { transform = t; }
    Transform GetTransform() const { return transform; }
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
        if (model) {
            model->Draw(transform);
        }
    }
};
// collision_detector.h
#pragma once
#include <glm/glm.hpp>
#include <vector>


// Ç°ÏòÉùÃ÷
class GameObject;

class CollisionDetector {
public:
    struct BoundingBox {
        glm::vec3 min;
        glm::vec3 max;
        glm::vec3 center;
        glm::vec3 size;

        BoundingBox() : min(0.0f), max(0.0f), center(0.0f), size(0.0f) {}
        BoundingBox(glm::vec3 min, glm::vec3 max) : min(min), max(max) {
            center = (min + max) * 0.5f;
            size = max - min;
        }

        bool intersects(const BoundingBox& other) const {
            return (min.x <= other.max.x && max.x >= other.min.x) &&
                (min.y <= other.max.y && max.y >= other.min.y) &&
                (min.z <= other.max.z && max.z >= other.min.z);
        }
    };

    static bool CheckCollision(const BoundingBox& a, const BoundingBox& b);
    static BoundingBox CalculateBoundingBox(GameObject* obj, float scaleFactor = 1.0f);
};
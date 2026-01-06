// collision_detector.cpp
#include "collision_detector.h"
#include "game_object.h"  // 添加包含
#include "mesh.h"

bool CollisionDetector::CheckCollision(const BoundingBox& a, const BoundingBox& b) {
    return a.intersects(b);
}

CollisionDetector::BoundingBox CollisionDetector::CalculateBoundingBox(GameObject* obj, float scaleFactor) {
    if (!obj || !obj->GetModel() || !obj->GetModel()->meshes.empty()) {
        return BoundingBox();
    }

    auto vertices = obj->GetModel()->meshes[0].mesh->GetVertices();
    for(int i=1;i<obj->GetModel()->meshes.size();++i){
        auto moreVertices = obj->GetModel()->meshes[i].mesh->GetVertices();
        vertices.insert(vertices.end(), moreVertices.begin(), moreVertices.end());
	}
    if (vertices.empty()) {
        return BoundingBox();
    }

    // 获取完整的模型变换矩阵
    glm::mat4 modelMatrix = obj->GetTransform().getLocalMatrix();

    // 转换第一个顶点来初始化min/max
    glm::vec4 firstWorldPos = modelMatrix * glm::vec4(vertices[0].position, 1.0f);
    glm::vec3 min = glm::vec3(firstWorldPos);
    glm::vec3 max = glm::vec3(firstWorldPos);

    // 正确考虑旋转、缩放、位移的变换
    for (const auto& vertex : vertices) {
        glm::vec4 worldPos = modelMatrix * glm::vec4(vertex.position, 1.0f);
        glm::vec3 pos = glm::vec3(worldPos);

        min.x = std::min(min.x, pos.x);
        min.y = std::min(min.y, pos.y);
        min.z = std::min(min.z, pos.z);

        max.x = std::max(max.x, pos.x);
        max.y = std::max(max.y, pos.y);
        max.z = std::max(max.z, pos.z);
    }

    // 应用缩放因子
    glm::vec3 center = (min + max) * 0.5f;
    glm::vec3 size = (max - min) * scaleFactor;

    return BoundingBox(center - size * 0.5f, center + size * 0.5f);
}
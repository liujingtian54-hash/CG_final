#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "base/vertex.h"

enum class ShapeType {
    Cube,
    Sphere,
    Cylinder,
    Cone,
    Prism,
    Frustum
};

class GeometryGenerator {
public:
    // 生成立方体
    static void CreateCube(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, float size = 1.0f);

    // 生成球体
    static void CreateSphere(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, float radius = 1.0f, int segments = 32);

    // 生成圆柱体
    static void CreateCylinder(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, float radius = 0.5f, float height = 2.0f, int segments = 32);

    // 生成圆锥体
    static void CreateCone(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, float radius = 0.5f, float height = 2.0f, int segments = 32);

    // 生成多面棱柱
    static void CreatePrism(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, int sides = 6, float radius = 0.5f, float height = 2.0f);

    // 生成多面棱台
    static void CreateFrustum(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, int sides = 6, float bottomRadius = 0.8f, float topRadius = 0.3f, float height = 2.0f);

private:
    // 计算法向量
    static glm::vec3 CalculateNormal(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3);

    // 添加三角形
    static void AddTriangle(std::vector<unsigned int>& indices, unsigned int i0, unsigned int i1, unsigned int i2);

    // 添加四边形
    static void AddQuad(std::vector<unsigned int>& indices, unsigned int i0, unsigned int i1, unsigned int i2, unsigned int i3);
};
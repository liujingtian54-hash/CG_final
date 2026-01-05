#include "geometry_generator.h"
#include <cmath>
#include <glm/gtc/constants.hpp>

void GeometryGenerator::CreateCube(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, float size) {
    vertices.clear();
    indices.clear();

    float halfSize = size * 0.5f;

    // 立方体的8个顶点
    glm::vec3 positions[8] = {
        glm::vec3(-halfSize, -halfSize, -halfSize),
        glm::vec3(halfSize, -halfSize, -halfSize),
        glm::vec3(halfSize,  halfSize, -halfSize),
        glm::vec3(-halfSize,  halfSize, -halfSize),
        glm::vec3(-halfSize, -halfSize,  halfSize),
        glm::vec3(halfSize, -halfSize,  halfSize),
        glm::vec3(halfSize,  halfSize,  halfSize),
        glm::vec3(-halfSize,  halfSize,  halfSize)
    };

    // 法向量
    glm::vec3 normals[6] = {
        glm::vec3(0.0f, 0.0f, -1.0f),  // 前面
        glm::vec3(0.0f, 0.0f, 1.0f),   // 后面
        glm::vec3(-1.0f, 0.0f, 0.0f),  // 左面
        glm::vec3(1.0f, 0.0f, 0.0f),   // 右面
        glm::vec3(0.0f, -1.0f, 0.0f),  // 下面
        glm::vec3(0.0f, 1.0f, 0.0f)    // 上面
    };

    // 纹理坐标
    glm::vec2 uvs[4] = {
        glm::vec2(0.0f, 0.0f),
        glm::vec2(1.0f, 0.0f),
        glm::vec2(1.0f, 1.0f),
        glm::vec2(0.0f, 1.0f)
    };

    // 每个面的顶点索引
    int faces[6][4] = {
        {0, 1, 2, 3},  // 前面
        {5, 4, 7, 6},  // 后面
        {4, 0, 3, 7},  // 左面
        {1, 5, 6, 2},  // 右面
        {4, 5, 1, 0},  // 下面
        {3, 2, 6, 7}   // 上面
    };

    // 创建每个面的顶点
    for (int face = 0; face < 6; ++face) {
        int baseIndex = static_cast<int>(vertices.size());

        for (int i = 0; i < 4; ++i) {
            Vertex vertex = {};
            vertex.position = positions[faces[face][i]];
            vertex.normal = normals[face];
            vertex.texCoord = uvs[i];
            vertices.push_back(vertex);
        }

        // 添加两个三角形（四边形）
        AddQuad(indices,
            static_cast<unsigned int>(baseIndex),
            static_cast<unsigned int>(baseIndex + 1),
            static_cast<unsigned int>(baseIndex + 2),
            static_cast<unsigned int>(baseIndex + 3));
    }
}

void GeometryGenerator::CreateSphere(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, float radius, int segments) {
    vertices.clear();
    indices.clear();

    if (segments < 3) segments = 3;

    int rings = segments / 2;
    if (rings < 2) rings = 2;

    // 生成顶点
    for (int ring = 0; ring <= rings; ++ring) {
        float phi = glm::pi<float>() * static_cast<float>(ring) / static_cast<float>(rings);
        float y = radius * cos(phi);
        float ringRadius = radius * sin(phi);

        for (int segment = 0; segment <= segments; ++segment) {
            float theta = 2.0f * glm::pi<float>() * static_cast<float>(segment) / static_cast<float>(segments);
            float x = ringRadius * cos(theta);
            float z = ringRadius * sin(theta);

            Vertex vertex = {};
            vertex.position = glm::vec3(x, y, z);
            vertex.normal = glm::normalize(glm::vec3(x, y, z));
            vertex.texCoord = glm::vec2(
                static_cast<float>(segment) / static_cast<float>(segments),
                static_cast<float>(ring) / static_cast<float>(rings)
            );

            vertices.push_back(vertex);
        }
    }

    // 生成索引
    for (int ring = 0; ring < rings; ++ring) {
        for (int segment = 0; segment < segments; ++segment) {
            int current = ring * (segments + 1) + segment;
            int next = current + segments + 1;

            AddQuad(indices,
                static_cast<unsigned int>(current),
                static_cast<unsigned int>(next),
                static_cast<unsigned int>(next + 1),
                static_cast<unsigned int>(current + 1));
        }
    }
}

void GeometryGenerator::CreateCylinder(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, float radius, float height, int segments) {
    vertices.clear();
    indices.clear();

    if (segments < 3) segments = 3;

    float halfHeight = height * 0.5f;

    // 顶部圆盘中心
    Vertex topCenter = {};
    topCenter.position = glm::vec3(0.0f, halfHeight, 0.0f);
    topCenter.normal = glm::vec3(0.0f, 1.0f, 0.0f);
    topCenter.texCoord = glm::vec2(0.5f, 0.5f);
    vertices.push_back(topCenter);
    unsigned int topCenterIndex = 0;

    // 底部圆盘中心
    Vertex bottomCenter = {};
    bottomCenter.position = glm::vec3(0.0f, -halfHeight, 0.0f);
    bottomCenter.normal = glm::vec3(0.0f, -1.0f, 0.0f);
    bottomCenter.texCoord = glm::vec2(0.5f, 0.5f);
    vertices.push_back(bottomCenter);
    unsigned int bottomCenterIndex = 1;

    // 生成顶部和底部圆盘的顶点
    std::vector<unsigned int> topRing, bottomRing;

    for (int i = 0; i <= segments; ++i) {
        float theta = 2.0f * glm::pi<float>() * static_cast<float>(i) / static_cast<float>(segments);
        float x = radius * cos(theta);
        float z = radius * sin(theta);

        // 顶部圆盘顶点
        Vertex topVertex = {};
        topVertex.position = glm::vec3(x, halfHeight, z);
        topVertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        topVertex.texCoord = glm::vec2(
            0.5f + 0.5f * cos(theta),
            0.5f + 0.5f * sin(theta)
        );
        vertices.push_back(topVertex);
        topRing.push_back(static_cast<unsigned int>(vertices.size() - 1));

        // 底部圆盘顶点
        Vertex bottomVertex = {};
        bottomVertex.position = glm::vec3(x, -halfHeight, z);
        bottomVertex.normal = glm::vec3(0.0f, -1.0f, 0.0f);
        bottomVertex.texCoord = glm::vec2(
            0.5f + 0.5f * cos(theta),
            0.5f + 0.5f * sin(theta)
        );
        vertices.push_back(bottomVertex);
        bottomRing.push_back(static_cast<unsigned int>(vertices.size() - 1));

        // 侧面顶点（每个顶点需要两个：一个用于顶部，一个用于底部）
        Vertex sideTop = {}, sideBottom = {};
        sideTop.position = glm::vec3(x, halfHeight, z);
        sideBottom.position = glm::vec3(x, -halfHeight, z);

        glm::vec3 normal = glm::normalize(glm::vec3(x, 0.0f, z));
        sideTop.normal = normal;
        sideBottom.normal = normal;

        sideTop.texCoord = glm::vec2(static_cast<float>(i) / static_cast<float>(segments), 1.0f);
        sideBottom.texCoord = glm::vec2(static_cast<float>(i) / static_cast<float>(segments), 0.0f);

        vertices.push_back(sideTop);
        vertices.push_back(sideBottom);
    }

    // 生成顶部圆盘三角形
    for (int i = 0; i < segments; ++i) {
        AddTriangle(indices, topCenterIndex, topRing[i + 1], topRing[i]);
    }

    // 生成底部圆盘三角形
    for (int i = 0; i < segments; ++i) {
        AddTriangle(indices, bottomCenterIndex, bottomRing[i], bottomRing[i + 1]);
    }

    // 生成侧面四边形
    unsigned int sideStartIndex = 2 + 2 * (static_cast<unsigned int>(segments) + 1);
    for (int i = 0; i < segments; ++i) {
        unsigned int topLeft = sideStartIndex + 2 * static_cast<unsigned int>(i);
        unsigned int topRight = sideStartIndex + 2 * static_cast<unsigned int>(i + 1);
        unsigned int bottomLeft = topLeft + 1;
        unsigned int bottomRight = topRight + 1;

        AddQuad(indices, topLeft, topRight, bottomRight, bottomLeft);
    }
}

void GeometryGenerator::CreateCone(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, float radius, float height, int segments) {
    vertices.clear();
    indices.clear();

    if (segments < 3) segments = 3;

    float halfHeight = height * 0.5f;

    // 底部圆盘中心
    Vertex bottomCenter = {};
    bottomCenter.position = glm::vec3(0.0f, -halfHeight, 0.0f);
    bottomCenter.normal = glm::vec3(0.0f, -1.0f, 0.0f);
    bottomCenter.texCoord = glm::vec2(0.5f, 0.5f);
    vertices.push_back(bottomCenter);
    unsigned int bottomCenterIndex = 0;

    // 顶点（锥尖）
    Vertex apex = {};
    apex.position = glm::vec3(0.0f, halfHeight, 0.0f);
    apex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
    apex.texCoord = glm::vec2(0.5f, 1.0f);
    vertices.push_back(apex);
    unsigned int apexIndex = 1;

    // 生成底部圆盘和侧面顶点
    std::vector<unsigned int> bottomRing, sideVertices;

    for (int i = 0; i <= segments; ++i) {
        float theta = 2.0f * glm::pi<float>() * static_cast<float>(i) / static_cast<float>(segments);
        float x = radius * cos(theta);
        float z = radius * sin(theta);

        // 底部圆盘顶点
        Vertex bottomVertex = {};
        bottomVertex.position = glm::vec3(x, -halfHeight, z);
        bottomVertex.normal = glm::vec3(0.0f, -1.0f, 0.0f);
        bottomVertex.texCoord = glm::vec2(
            0.5f + 0.5f * cos(theta),
            0.5f + 0.5f * sin(theta)
        );
        vertices.push_back(bottomVertex);
        bottomRing.push_back(static_cast<unsigned int>(vertices.size() - 1));

        // 侧面底部顶点
        Vertex sideVertex = {};
        sideVertex.position = glm::vec3(x, -halfHeight, z);
        // 法向量需要计算（从底部边缘指向顶点）
        glm::vec3 edgeToApex = apex.position - sideVertex.position;
        glm::vec3 tangent = glm::vec3(-sin(theta), 0.0f, cos(theta));
        sideVertex.normal = glm::normalize(glm::cross(edgeToApex, tangent));
        sideVertex.texCoord = glm::vec2(static_cast<float>(i) / static_cast<float>(segments), 0.0f);
        vertices.push_back(sideVertex);
        sideVertices.push_back(static_cast<unsigned int>(vertices.size() - 1));
    }

    // 生成底部圆盘三角形
    for (int i = 0; i < segments; ++i) {
        AddTriangle(indices, bottomCenterIndex, bottomRing[i], bottomRing[i + 1]);
    }

    // 生成侧面三角形
    for (int i = 0; i < segments; ++i) {
        AddTriangle(indices, apexIndex, sideVertices[i], sideVertices[i + 1]);
    }
}

void GeometryGenerator::CreatePrism(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, int sides, float radius, float height) {
    vertices.clear();
    indices.clear();

    if (sides < 3) sides = 3;

    float halfHeight = height * 0.5f;

    // 顶部圆盘中心
    Vertex topCenter = {};
    topCenter.position = glm::vec3(0.0f, halfHeight, 0.0f);
    topCenter.normal = glm::vec3(0.0f, 1.0f, 0.0f);
    topCenter.texCoord = glm::vec2(0.5f, 0.5f);
    vertices.push_back(topCenter);
    unsigned int topCenterIndex = 0;

    // 底部圆盘中心
    Vertex bottomCenter = {};
    bottomCenter.position = glm::vec3(0.0f, -halfHeight, 0.0f);
    bottomCenter.normal = glm::vec3(0.0f, -1.0f, 0.0f);
    bottomCenter.texCoord = glm::vec2(0.5f, 0.5f);
    vertices.push_back(bottomCenter);
    unsigned int bottomCenterIndex = 1;

    // 生成顶部和底部圆盘的顶点
    std::vector<unsigned int> topRing, bottomRing, sideTopRing, sideBottomRing;

    for (int i = 0; i <= sides; ++i) {
        float theta = 2.0f * glm::pi<float>() * static_cast<float>(i) / static_cast<float>(sides);
        float x = radius * cos(theta);
        float z = radius * sin(theta);

        // 顶部圆盘顶点
        Vertex topVertex = {};
        topVertex.position = glm::vec3(x, halfHeight, z);
        topVertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        topVertex.texCoord = glm::vec2(
            0.5f + 0.5f * cos(theta),
            0.5f + 0.5f * sin(theta)
        );
        vertices.push_back(topVertex);
        topRing.push_back(static_cast<unsigned int>(vertices.size() - 1));

        // 底部圆盘顶点
        Vertex bottomVertex = {};
        bottomVertex.position = glm::vec3(x, -halfHeight, z);
        bottomVertex.normal = glm::vec3(0.0f, -1.0f, 0.0f);
        bottomVertex.texCoord = glm::vec2(
            0.5f + 0.5f * cos(theta),
            0.5f + 0.5f * sin(theta)
        );
        vertices.push_back(bottomVertex);
        bottomRing.push_back(static_cast<unsigned int>(vertices.size() - 1));

        // 侧面顶点
        Vertex sideTop = {}, sideBottom = {};
        sideTop.position = glm::vec3(x, halfHeight, z);
        sideBottom.position = glm::vec3(x, -halfHeight, z);

        // 棱柱的侧面法向量是每个面的法向量
        float nextTheta = 2.0f * glm::pi<float>() * (static_cast<float>(i) + 0.5f) / static_cast<float>(sides);
        glm::vec3 normal = glm::normalize(glm::vec3(cos(nextTheta), 0.0f, sin(nextTheta)));
        sideTop.normal = normal;
        sideBottom.normal = normal;

        sideTop.texCoord = glm::vec2(static_cast<float>(i) / static_cast<float>(sides), 1.0f);
        sideBottom.texCoord = glm::vec2(static_cast<float>(i) / static_cast<float>(sides), 0.0f);

        vertices.push_back(sideTop);
        vertices.push_back(sideBottom);
        sideTopRing.push_back(static_cast<unsigned int>(vertices.size() - 2));
        sideBottomRing.push_back(static_cast<unsigned int>(vertices.size() - 1));
    }

    // 生成顶部圆盘三角形
    for (int i = 0; i < sides; ++i) {
        AddTriangle(indices, topCenterIndex, topRing[i + 1], topRing[i]);
    }

    // 生成底部圆盘三角形
    for (int i = 0; i < sides; ++i) {
        AddTriangle(indices, bottomCenterIndex, bottomRing[i], bottomRing[i + 1]);
    }

    // 生成侧面四边形
    for (int i = 0; i < sides; ++i) {
        AddQuad(indices, sideTopRing[i], sideTopRing[i + 1], sideBottomRing[i + 1], sideBottomRing[i]);
    }
}

void GeometryGenerator::CreateFrustum(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, int sides, float bottomRadius, float topRadius, float height) {
    vertices.clear();
    indices.clear();

    if (sides < 3) sides = 3;

    float halfHeight = height * 0.5f;

    // 顶部圆盘中心
    Vertex topCenter = {};
    topCenter.position = glm::vec3(0.0f, halfHeight, 0.0f);
    topCenter.normal = glm::vec3(0.0f, 1.0f, 0.0f);
    topCenter.texCoord = glm::vec2(0.5f, 0.5f);
    vertices.push_back(topCenter);
    unsigned int topCenterIndex = 0;

    // 底部圆盘中心
    Vertex bottomCenter = {};
    bottomCenter.position = glm::vec3(0.0f, -halfHeight, 0.0f);
    bottomCenter.normal = glm::vec3(0.0f, -1.0f, 0.0f);
    bottomCenter.texCoord = glm::vec2(0.5f, 0.5f);
    vertices.push_back(bottomCenter);
    unsigned int bottomCenterIndex = 1;

    // 生成顶部和底部圆盘的顶点
    std::vector<unsigned int> topRing, bottomRing, sideTopRing, sideBottomRing;

    for (int i = 0; i <= sides; ++i) {
        float theta = 2.0f * glm::pi<float>() * static_cast<float>(i) / static_cast<float>(sides);

        // 顶部圆盘顶点
        float topX = topRadius * cos(theta);
        float topZ = topRadius * sin(theta);
        Vertex topVertex = {};
        topVertex.position = glm::vec3(topX, halfHeight, topZ);
        topVertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        topVertex.texCoord = glm::vec2(
            0.5f + 0.5f * cos(theta) * (topRadius / bottomRadius),
            0.5f + 0.5f * sin(theta) * (topRadius / bottomRadius)
        );
        vertices.push_back(topVertex);
        topRing.push_back(static_cast<unsigned int>(vertices.size() - 1));

        // 底部圆盘顶点
        float bottomX = bottomRadius * cos(theta);
        float bottomZ = bottomRadius * sin(theta);
        Vertex bottomVertex = {};
        bottomVertex.position = glm::vec3(bottomX, -halfHeight, bottomZ);
        bottomVertex.normal = glm::vec3(0.0f, -1.0f, 0.0f);
        bottomVertex.texCoord = glm::vec2(
            0.5f + 0.5f * cos(theta),
            0.5f + 0.5f * sin(theta)
        );
        vertices.push_back(bottomVertex);
        bottomRing.push_back(static_cast<unsigned int>(vertices.size() - 1));

        // 侧面顶点
        Vertex sideTop = {}, sideBottom = {};
        sideTop.position = glm::vec3(topX, halfHeight, topZ);
        sideBottom.position = glm::vec3(bottomX, -halfHeight, bottomZ);

        // 计算侧面法向量（倾斜面的法向量）
        glm::vec3 edge = sideBottom.position - sideTop.position;
        glm::vec3 horizontal = glm::vec3(-sin(theta), 0.0f, cos(theta));
        glm::vec3 normal = glm::normalize(glm::cross(edge, horizontal));
        sideTop.normal = normal;
        sideBottom.normal = normal;

        sideTop.texCoord = glm::vec2(static_cast<float>(i) / static_cast<float>(sides), 1.0f);
        sideBottom.texCoord = glm::vec2(static_cast<float>(i) / static_cast<float>(sides), 0.0f);

        vertices.push_back(sideTop);
        vertices.push_back(sideBottom);
        sideTopRing.push_back(static_cast<unsigned int>(vertices.size() - 2));
        sideBottomRing.push_back(static_cast<unsigned int>(vertices.size() - 1));
    }

    // 生成顶部圆盘三角形
    for (int i = 0; i < sides; ++i) {
        AddTriangle(indices, topCenterIndex, topRing[i + 1], topRing[i]);
    }

    // 生成底部圆盘三角形
    for (int i = 0; i < sides; ++i) {
        AddTriangle(indices, bottomCenterIndex, bottomRing[i], bottomRing[i + 1]);
    }

    // 生成侧面四边形
    for (int i = 0; i < sides; ++i) {
        AddQuad(indices, sideTopRing[i], sideTopRing[i + 1], sideBottomRing[i + 1], sideBottomRing[i]);
    }
}

glm::vec3 GeometryGenerator::CalculateNormal(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3) {
    glm::vec3 edge1 = v2 - v1;
    glm::vec3 edge2 = v3 - v1;
    return glm::normalize(glm::cross(edge1, edge2));
}

void GeometryGenerator::AddTriangle(std::vector<unsigned int>& indices, unsigned int i0, unsigned int i1, unsigned int i2) {
    indices.push_back(i0);
    indices.push_back(i1);
    indices.push_back(i2);
}

void GeometryGenerator::AddQuad(std::vector<unsigned int>& indices, unsigned int i0, unsigned int i1, unsigned int i2, unsigned int i3) {
    // 第一个三角形
    indices.push_back(i0);
    indices.push_back(i1);
    indices.push_back(i2);

    // 第二个三角形
    indices.push_back(i0);
    indices.push_back(i2);
    indices.push_back(i3);
}
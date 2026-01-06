#pragma once
#include"mesh.h"
#include"material.h"
#include"collision_detector.h"
#include<glm/glm.hpp>
#include<vector>

#include"base/transform.h"

struct subMesh {
	Mesh* mesh;
	MixMaterial* material;
};

class Model {
public:
	std::vector<subMesh> meshes; // sub-meshes of the model
	std::vector<glm::vec3> pos_offset; // position offset for each mesh
	CollisionDetector localbounds; // local axis-aligned bounding box
	Model(CollisionDetector bound);
	void AddMesh(Mesh* mesh, MixMaterial* material, const glm::vec3& offset = glm::vec3(0.0f));
	void Draw(Transform t);
};
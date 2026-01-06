#include"model.h"
#include<glm/glm.hpp>
#include"mesh.h"
#include"material.h"
#include"collision_detector.h"
#include"base/transform.h"

Model::Model(CollisionDetector bound) : localbounds(bound) {
}

void Model::AddMesh(Mesh* mesh, MixMaterial* material, const glm::vec3& offset) {
	subMesh sm;
	sm.mesh = mesh;
	sm.material = material;
	meshes.push_back(sm);
	pos_offset.push_back(offset);
}

void Model::Draw(Transform t) {
	for (size_t i = 0; i < meshes.size(); ++i) {
		subMesh& sm = meshes[i];
		glm::vec3 offset = pos_offset[i];
		sm.material->Setup();
		glm::mat4 model = glm::translate(t.getLocalMatrix(), offset);
		sm.material->shader->setUniformMat4("model", model);
		sm.mesh->Draw();
	}
}

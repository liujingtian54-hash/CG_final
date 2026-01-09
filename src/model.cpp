#include"model.h"
#include<glm/glm.hpp>
#include"mesh.h"
#include"material.h"
#include"collision_detector.h"
#include"base/transform.h"
#include <glm/gtc/type_ptr.hpp>
#include <GL/gl.h> // for glGetIntegerv, glGetUniformLocation, glUniformMatrix4fv

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
        // Setup material (writes uniforms to currently active shader)
        if (sm.material) sm.material->Setup();

        glm::mat4 model = glm::translate(t.getLocalMatrix(), offset);

        // Set 'model' uniform on current shader program (do not assume material->shader is bound)
        GLint currentProgram = 0;
        glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
        if (currentProgram != 0) {
            GLint loc = glGetUniformLocation(currentProgram, "model");
            if (loc != -1) {
                glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(model));
            }
        }

        if (sm.mesh) {
            sm.mesh->Draw();
        }
    }
}

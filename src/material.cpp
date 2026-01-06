#include"material.h"
#include"base/glsl_program.h"
#include"base/texture2d.h"
#include <string>
#include <glm/glm.hpp>

MixMaterial::MixMaterial(GLSLProgram* shader, Texture2D* t1, Texture2D* t2) : Material(shader),
tex1(t1), tex2(t2), kds1(glm::vec3(1.0f, 1.0f, 1.0f)), kds2(glm::vec3(1.0f, 1.0f, 1.0f)) {
}

MixMaterial::~MixMaterial() {
}

void MixMaterial::Setup() {
	shader->use();
	shader->setUniformVec3("material.kds[0]", kds1);
	shader->setUniformVec3("material.kds[1]", kds2);
	shader->setUniformFloat("material.blend", blend);
	shader->setUniformVec3("material.ks", ks);
	shader->setUniformFloat("material.N", N);
	tex1->bind(0);
	shader->setUniformInt("mapKds[0]",0);
	tex2->bind(1);
	shader->setUniformInt("mapKds[1]",1);
}
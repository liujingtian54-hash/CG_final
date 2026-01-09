#include"material.h"
#include"base/glsl_program.h"
#include"base/texture2d.h"
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

MixMaterial::MixMaterial(GLSLProgram* shader, Texture2D* t1, Texture2D* t2) : Material(shader),
tex1(t1), tex2(t2), kds1(glm::vec3(1.0f, 1.0f, 1.0f)), kds2(glm::vec3(1.0f, 1.0f, 1.0f)) {
}

MixMaterial::~MixMaterial() {
}

static GLint getCurrentProgram() {
    GLint prog = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &prog);
    return prog;
}

static void setUniformVec3Current(const char* name, const glm::vec3& v) {
    GLint prog = getCurrentProgram();
    if (prog == 0) return;
    GLint loc = glGetUniformLocation(prog, name);
    if (loc != -1) glUniform3fv(loc, 1, glm::value_ptr(v));
}

static void setUniformFloatCurrent(const char* name, float v) {
    GLint prog = getCurrentProgram();
    if (prog == 0) return;
    GLint loc = glGetUniformLocation(prog, name);
    if (loc != -1) glUniform1f(loc, v);
}

static void setUniformIntCurrent(const char* name, int v) {
    GLint prog = getCurrentProgram();
    if (prog == 0) return;
    GLint loc = glGetUniformLocation(prog, name);
    if (loc != -1) glUniform1i(loc, v);
}

void MixMaterial::Setup() {
	//shader->use();
	GLint prog = 0;
	glGetIntegerv(GL_CURRENT_PROGRAM, &prog);
    setUniformVec3Current("material.kds[0]", kds1);
    setUniformVec3Current("material.kds[1]", kds2);
    setUniformFloatCurrent("material.blend", blend);
    setUniformVec3Current("material.ks", ks);
    setUniformFloatCurrent("material.N", N);
	tex1->bind(0);
	shader->setUniformInt("mapKds[0]",0);
	tex2->bind(1);
	shader->setUniformInt("mapKds[1]",1);
}
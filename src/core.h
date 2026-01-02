#pragma once

#include"base/glsl_program.h"
#include"base/application.h"
#include"base/camera.h"

//整个作业的核心逻辑
//TODO:定义需要补充，至少要补一个Scene
class Core : public Application {
public:
	Core();
	~Core();
private:
	GLSLProgram _shader;
	Camera _camera;
	void handleInput() override;
	void renderFrame() override;

};
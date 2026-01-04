#pragma once

#include"base/glsl_program.h"
#include"base/application.h"
#include"base/camera.h"
#include"base/light.h"
#include"scene.h"

class Core : public Application {
public:
	Core(const Options& options);
	~Core();
	void init();//do other steps in init
private:
	PerspectiveCamera _camera;
	Scene* _scene;
	DirectionalLight _light;
	void handleInput() override;
	void renderFrame() override;

	void SceneInitialize();

	void render();
	
	bool _isFirstMouse = true;
	float _lastX = 0.0f, _lastY = 0.0f;
};
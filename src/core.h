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
	void doFrame();

	void saveScreenshot(const std::string& filename);
	bool _ShouldSaveScreenshot = false;
	
	//player control parameters
	bool _isFirstMouse = true;
	float _lastX = 0.0f, _lastY = 0.0f;
	//light control parameters and functions
	bool _manuallycontrollight = false;
	float _yaw = -90.0f;
	float _pitch = -45.0f;
	void updateLightDirection(float yaw, float pitch) {
		glm::vec3 direction;
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		_light.transform.rotation = glm::normalize(direction);
	}
};
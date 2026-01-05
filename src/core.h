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

	void UpdateCharacterPosition(); // 添加人物位置更新函数
	GameObject* _character; // 人物对象指针
	float _characterDistance; // 跟随距离
	float _characterHeight; // 高度偏移

	void syncCameraAngles(); // 同步摄像机角度到控制变量

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

	// 新增：第三人称控制参数
	float _cameraYaw = -90.0f;    // 摄像机水平旋转
	float _cameraPitch = 0.0f;    // 摄像机垂直旋转
	float _characterYaw = -90.0f; // 人物水平旋转

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

	// 新增：更新摄像机旋转
	void updateCameraRotation() {
		glm::quat rotation = glm::quat(glm::vec3(glm::radians(_cameraPitch), glm::radians(_cameraYaw), 0.0f));
		_camera.transform.rotation = rotation;
	}

	// 新增：更新人物旋转
	void updateCharacterRotation() {
		glm::quat rotation = glm::quat(glm::vec3(0.0f, glm::radians(_characterYaw), 0.0f));
		_character->SetRotation(rotation);
	}

};
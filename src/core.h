#pragma once

#include"base/glsl_program.h"
#include"base/application.h"
#include"base/camera.h"
#include"base/light.h"
#include"scene.h"
#include "collision_detector.h"

class Core : public Application {
public:
	Core(const Options& options);
	~Core();
	void init();//do other steps in init
private:
	// 添加碰撞检测相关成员
	std::vector<CollisionDetector::BoundingBox> _buildingColliders;
	CollisionDetector::BoundingBox _characterCollider;
	float _collisionScaleFactor = 0.8f; // 碰撞箱缩放因子

	void updateBuildingColliders();
	bool checkCharacterCollision(const glm::vec3& newPosition);

	
	GameObject* _character; // 人物对象指针
	float _characterDistance; // 跟随距离
	float _characterHeight; // 高度偏移

	void syncCameraAngles(); // 同步摄像机角度到控制变量
	void UpdateCharacterPosition() {
		if (!_character) return;

		glm::vec3 cameraOffset = -_camera.transform.getFront() * _characterDistance;
		cameraOffset.y = _characterHeight;
		_camera.transform.position = _character->GetPosition() + cameraOffset;
	} // 只声明，不定义

	PerspectiveCamera _camera;
	Scene* _scene;
	DirectionalLight _light;
	void handleInput() override;
	void renderFrame() override;

	void SceneInitialize();
	void New(ObjectGroup ObjGroup, Mesh* mesh, MixMaterial* material, float posx, float posy, float posz, float rotx, float roty, float rotz, float sx, float sy, float sz);

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
#include<glm/glm.hpp>
#include<ctime>
#include <iomanip>
#include <iostream>
#include <string>
#include"core.h"
#include"scene.h"
#include"game_object.h"
#include"mesh.h"
#include"material.h"
#include"model.h"
#include"resource_manager.h"
#include"shader_source.h"

#include"base/skybox.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

void Core::updateBuildingColliders() {
    _buildingColliders.clear();

    auto buildings = _scene->GetObjectsByGroup(ObjectGroup::Building);
    for (auto building : buildings) {
        auto collider = CollisionDetector::CalculateBoundingBox(building, _collisionScaleFactor);
        _buildingColliders.push_back(collider);
    }
}

bool Core::checkCharacterCollision(const glm::vec3& newPosition) {
    // 计算人物在新位置的碰撞箱
    glm::vec3 oldPosition = _character->GetPosition();
    _character->SetPosition(newPosition);

    _characterCollider = CollisionDetector::CalculateBoundingBox(_character, _collisionScaleFactor);

    // 恢复原位置
    _character->SetPosition(oldPosition);

    // 检查与所有建筑物的碰撞
    for (const auto& buildingCollider : _buildingColliders) {
        if (CollisionDetector::CheckCollision(_characterCollider, buildingCollider)) {
            return true; // 发生碰撞
        }
    }

    return false; // 无碰撞
}



Core::Core(const Options& options) : Application(options), 
_camera(glm::radians(60.0f), 1.0f * _windowWidth / _windowHeight, 0.1f, 10000.0f), 
_scene(nullptr), _light() {
	_scene = new Scene();
    _light.transform = Transform();
    //_light.transform.rotation = glm::vec3(-1.0f, -1.0f, -1.0f);
    updateLightDirection(_yaw, _pitch);
	_light.intensity = 1.0f;

    _characterDistance = 0.4f; // 人物在摄像机前方0.3个单位
    _characterHeight = 0.175f;   // 人物在摄像机下方0.25个单位
	init();
}

Core::~Core() {
	delete _scene;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    ResourceManager::Clear();
}

void Core::init() {
	//input initialize
    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    _input.mouse.move.xNow = _input.mouse.move.xOld = 0.5f * _windowWidth;
    _input.mouse.move.yNow = _input.mouse.move.yOld = 0.5f * _windowHeight;
    glfwSetCursorPos(_window, _input.mouse.move.xNow, _input.mouse.move.yNow);
	//imgui initialize
    IMGUI_CHECKVERSION();
	ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(_window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

	//resource initialize
    ResourceManager::LoadShader("Blinn_Phongshader",
        ShaderSource::Blinn_PhongVertexShader,
		ShaderSource::Blinn_PhongFragmentShader);
	//顺序应该是先加载贴图再加材质，然后才能导入mesh
    ResourceManager::LoadTexture("white", "../media/texture/white.png");

    ResourceManager::AddMaterial("white_material", new MixMaterial(ResourceManager::GetShader("Blinn_Phongshader"),
        ResourceManager::GetTexture("white"), ResourceManager::GetTexture("white")));

    ResourceManager::LoadModel("building1", "../media/obj/fangjian1.obj");
//    ResourceManager::LoadMesh("building1_exported", "../media/obj/building1_exported.obj");
    ResourceManager::LoadModel("building2", "../media/obj/fangjian2.obj");
    ResourceManager::LoadModel("building3", "../media/obj/fangjian3.obj");
    ResourceManager::LoadModel("building4", "../media/obj/fangjian4.obj");

    ResourceManager::LoadModel("tree1", "../media/obj/shu1.obj");
    ResourceManager::LoadModel("tree2", "../media/obj/shu2.obj");
    ResourceManager::LoadModel("tree3", "../media/obj/shu3.obj");

    ResourceManager::LoadModel("road1", "../media/obj/dizhuan1.obj");
    ResourceManager::LoadModel("road2", "../media/obj/dizhuan2.obj");
    ResourceManager::LoadModel("road3", "../media/obj/dizhuan3.obj");

    ResourceManager::LoadModel("character", "../media/obj/character_walk_01.obj");
	//只是表明我们做了导出功能
	//实际上对于这个项目来说，并不需要导出功能
//	ResourceManager::ExportMesh("building1", "../media/obj/building1_exported.obj");

    std::vector<std::string> paths = {
        "../media/texture/skybox/Right_Tex.jpg",
        "../media/texture/skybox/Left_Tex.jpg",
        "../media/texture/skybox/Up_Tex.jpg",
        "../media/texture/skybox/Down_Tex.jpg",
        "../media/texture/skybox/Front_Tex.jpg",
        "../media/texture/skybox/Back_Tex.jpg"
	};
    skybox[0] = new SkyBox(paths);
	paths[0] = "../media/texture/skyboxrt/right.jpg";
	paths[1] = "../media/texture/skyboxrt/left.jpg";
	paths[2] = "../media/texture/skyboxrt/top.jpg";
	paths[3] = "../media/texture/skyboxrt/bottom.jpg";
	paths[4] = "../media/texture/skyboxrt/front.jpg";
	paths[5] = "../media/texture/skyboxrt/back.jpg";
	skybox[1] = new SkyBox(paths);
	//scene initialize
    // 加载角色模型和动画帧
    ResourceManager::LoadAnimationFrames("character_walk",
        "../media/obj/character_walk_", 2, 21);

    // 批量加载行走动画帧
    for (int i = 2; i <= 21; ++i) {
        std::string frameNum = (i < 10 ? "0" : "") + std::to_string(i);
        std::string modelName = "character_walk_" + frameNum;
        std::string filePath = "../media/obj/character_walk_" + frameNum + ".obj";
        ResourceManager::LoadModel(modelName, filePath);
    }
    loadCharacterAnimations();
    SceneInitialize();
}

//TODO:处理输入
//这个输入是昨天晚上临时写的，正式版本需要重新设计输入处理
void Core::handleInput() {
    constexpr float cameraMoveSpeed = 0.01f;
    constexpr float cameraRotateSpeed = 0.01f;
    
    static bool move_state = true;
    static bool key_state = true;
    static bool pPressed = false;

    static bool oldState = true;


    PerspectiveCamera* camera = &_camera;
    if (_input.keyboard.keyStates[GLFW_KEY_X] == GLFW_RELEASE) {
        if (key_state) {
            oldState = move_state;
			move_state = !move_state;
            _manuallycontrollight = !_manuallycontrollight;

            // 当从灯光控制切换回人物控制时，重置鼠标位置
            if (oldState == false && move_state == true) {
                // 同步当前摄像机和人物的实际角度到控制变量
                syncCameraAngles();

                // 重置鼠标位置，避免瞬间旋转
                _input.mouse.move.xOld = _input.mouse.move.xNow;
                _input.mouse.move.yOld = _input.mouse.move.yNow;

                // 可选：将鼠标位置设置到窗口中心
                glfwSetCursorPos(_window, _windowWidth * 0.5f, _windowHeight * 0.5f);
                _input.mouse.move.xNow = _windowWidth * 0.5f;
                _input.mouse.move.yNow = _windowHeight * 0.5f;

                // 重置第一次鼠标移动标志
                _isFirstMouse = true;
            }

            // 当从人物控制切换到灯光控制时，也同步一次角度
            if (oldState == true && move_state == false) {
                syncCameraAngles();
            }

        }
        key_state = false;
    }
    else {
		key_state = true;
    }

    if (_input.keyboard.keyStates[GLFW_KEY_P] != GLFW_RELEASE) {
        if (!pPressed) {
			_ShouldSaveScreenshot = true;
            pPressed = true;
        }
        else {
			_ShouldSaveScreenshot = false; 
        }
    }
    else {
        pPressed = false;
        _ShouldSaveScreenshot = false;
    }

    if (_input.keyboard.keyStates[GLFW_KEY_ESCAPE] != GLFW_RELEASE) {
        glfwSetWindowShouldClose(_window, true);
        return;
    }

    if (move_state) {
        glm::vec3 newPosition = _character->GetPosition();
        glm::vec3 moveDelta(0.0f);

        bool wasMoving = _isMoving;
        _isMoving = false; // 先重置移动状态

        // 检测移动输入
        if (_input.keyboard.keyStates[GLFW_KEY_W] != GLFW_RELEASE ||
            _input.keyboard.keyStates[GLFW_KEY_A] != GLFW_RELEASE ||
            _input.keyboard.keyStates[GLFW_KEY_S] != GLFW_RELEASE ||
            _input.keyboard.keyStates[GLFW_KEY_D] != GLFW_RELEASE) {

            _isMoving = true;
        }

        // 更新动画状态
        if (_isMoving && !wasMoving) {
            playAnimation();
        }
        else if (!_isMoving && wasMoving) {
            stopAnimation();
        }

        // W - 向前移动
        if (_input.keyboard.keyStates[GLFW_KEY_W] != GLFW_RELEASE) {
            glm::vec3 moveDirection = _character->GetFront();
            moveDirection.y = 0;
            moveDirection = glm::normalize(moveDirection);
            moveDelta += moveDirection * cameraMoveSpeed;
        }

        // A - 向左移动
        if (_input.keyboard.keyStates[GLFW_KEY_A] != GLFW_RELEASE) {
            glm::vec3 moveDirection = -_character->GetRight(); // 使用人物的右向
            moveDirection.y = 0;
            moveDirection = glm::normalize(moveDirection);
            moveDelta += moveDirection * cameraMoveSpeed;
        }

        // S - 向后移动
        if (_input.keyboard.keyStates[GLFW_KEY_S] != GLFW_RELEASE) {
            glm::vec3 moveDirection = -_character->GetFront();
            moveDirection.y = 0;
            moveDirection = glm::normalize(moveDirection);
            moveDelta += moveDirection * cameraMoveSpeed;
        }

        // D - 向右移动
        if (_input.keyboard.keyStates[GLFW_KEY_D] != GLFW_RELEASE) {
            glm::vec3 moveDirection = _character->GetRight();
            moveDirection.y = 0;
            moveDirection = glm::normalize(moveDirection);
            moveDelta += moveDirection * cameraMoveSpeed;
        }

       
        // 如果有移动输入，检查碰撞
        if (moveDelta != glm::vec3(0.0f)) {
            glm::vec3 proposedPosition = newPosition + moveDelta;

            // 检查碰撞
            if (!checkCharacterCollision(proposedPosition)) {
                // 无碰撞，允许移动
                _character->SetPosition(proposedPosition);
            }
            else {
                // 发生碰撞，尝试分轴移动（提供更好的移动体验）

                // 只移动X轴
                glm::vec3 xOnlyPosition = newPosition + glm::vec3(moveDelta.x, 0.0f, 0.0f);
                if (!checkCharacterCollision(xOnlyPosition)) {
                    _character->SetPosition(xOnlyPosition);
                }
                // 只移动Z轴
                else {
                    glm::vec3 zOnlyPosition = newPosition + glm::vec3(0.0f, 0.0f, moveDelta.z);
                    if (!checkCharacterCollision(zOnlyPosition)) {
                        _character->SetPosition(zOnlyPosition);
                    }
                }
                // 如果分轴移动也碰撞，则完全阻止移动
            }
        }

        // 处理鼠标旋转（保持不变）
        float deltaX = _input.mouse.move.xNow - _input.mouse.move.xOld;
        float deltaY = _input.mouse.move.yNow - _input.mouse.move.yOld;

        if (deltaX != 0.0f) {
            _cameraYaw -= deltaX * cameraRotateSpeed;
            _characterYaw -= deltaX * cameraRotateSpeed;

            if (_cameraYaw > 180.0f) _cameraYaw -= 360.0f;
            if (_cameraYaw < -180.0f) _cameraYaw += 360.0f;
            if (_characterYaw > 180.0f) _characterYaw -= 360.0f;
            if (_characterYaw < -180.0f) _characterYaw += 360.0f;

            updateCameraRotation();
            updateCharacterRotation();
        }

        if (deltaY != 0.0f) {
            _cameraPitch -= deltaY * cameraRotateSpeed;

            if (_cameraPitch > 89.0f) _cameraPitch = 89.0f;
            if (_cameraPitch < -89.0f) _cameraPitch = -89.0f;

            updateCameraRotation();
        }
    }


    _input.forwardState();
    UpdateCharacterPosition(); // 摄像机跟随人物
}

void Core::renderFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // 人物设置窗口
    ImGui::Begin("Character Settings");
    ImGui::SliderFloat("Follow Distance", &_characterDistance, 0.0f, 0.5f);
    ImGui::SliderFloat("Height Offset", &_characterHeight, -1.0f, 1.0f);
    ImGui::End();

    // 光照设置窗口
    ImGui::Begin("Light Settings");

    if (!_manuallycontrollight)
        ImGui::BeginDisabled();

    ImGui::ColorEdit3("Light Color", &_light.color.x);
    ImGui::SliderFloat("Intensity", &_light.intensity, 0.0f, 5.0f);

    ImGui::Text("Sun Position");
    bool updated = false;
    updated |= ImGui::SliderFloat("Yaw", &_yaw, -180.0f, 180.0f);
    updated |= ImGui::SliderFloat("Pitch", &_pitch, -89.0f, 89.0f);

    if (updated) {
        updateLightDirection(_yaw, _pitch);
    }

    if (!_manuallycontrollight)
        ImGui::EndDisabled();

    // 摄像机信息显示（保持在光照设置窗口内）
    ImGui::Separator();
    ImGui::Text("=== Camera Information ===");

    // 从四元数转换为欧拉角
    glm::vec3 euler = glm::eulerAngles(_camera.transform.rotation);
    // 将弧度转换为角度，并确保角度范围正确
    glm::vec3 degrees = glm::degrees(euler);

    degrees.x = fmod(degrees.x + 180.0f, 360.0f) - 180.0f;
    degrees.y = fmod(degrees.y + 180.0f, 360.0f) - 180.0f;
    degrees.z = fmod(degrees.z + 180.0f, 360.0f) - 180.0f;

    // 获取摄像机的前向向量
    glm::vec3 front = _camera.transform.getFront();

    ImGui::Text("Camera Position: (%.2f, %.2f, %.2f)",
        _camera.transform.position.x,
        _camera.transform.position.y,
        _camera.transform.position.z);

    ImGui::Text("Camera Rotation (degrees):");
    ImGui::Text("  X(Pitch): %.2f°", degrees.x);
    ImGui::Text("  Y(Yaw):   %.2f°", degrees.y);
    ImGui::Text("  Z(Roll):  %.2f°", degrees.z);

    ImGui::Text("Forward Vector: (%.2f, %.2f, %.2f)", front.x, front.y, front.z);

    // 显示控制模式状态
    ImGui::Separator();
    ImGui::Text("Control Mode: %s", _manuallycontrollight ? "Light Control" : "Character Control");
    ImGui::Text("Mouse State: %s",
        glfwGetInputMode(_window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED ?
        "Locked" : "Free");

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
        1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::End(); // 结束光照设置窗口

	ImGui::Begin("SkyBox Settings");
	if (ImGui::Button("SkyBox 1")) {
		skyboxIndex = 0;
	}
	ImGui::SameLine();
    if (ImGui::Button("SkyBox 2")) {
        skyboxIndex = 1;
    }
    ImGui::SameLine();
	ImGui::Text("Current SkyBox: %d", skyboxIndex + 1);
    ImGui::End();
    // 添加动画控制窗口
    ImGui::Begin("Animation Settings");
    ImGui::Text("Animation State: %s", _isMoving ? "Walking" : "Idle");
    ImGui::Text("Current Frame: %d/%d", _currentAnimationFrame,
        _characterAnimations.size() - 1);
    ImGui::SliderFloat("Animation Speed", &_animationSpeed, 0.01f, 0.5f);
    ImGui::End();
    glClearColor(_clearColor.r, _clearColor.g, _clearColor.b, _clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    render();
    doFrame();
}

void Core::render() {
	skybox[skyboxIndex]->draw(_camera.getProjectionMatrix(), glm::mat4(glm::mat3(_camera.getViewMatrix())));
    GLSLProgram* s = ResourceManager::GetShader("Blinn_Phongshader");
    s->use();
    s->setUniformMat4("projection", _camera.getProjectionMatrix());
    s->setUniformMat4("view", _camera.getViewMatrix());
	s->setUniformVec3("viewPos", _camera.transform.position);
    glm::quat rot = _light.transform.rotation;
    glm::vec3 direction = glm::vec3(rot.x, rot.y, rot.z);
    s->setUniformVec3("light.direction", direction);
	s->setUniformVec3("light.Ambientcolor", glm::vec3(0.2f,0.2f,0.25f));
    s->setUniformVec3("light.Diffusecolor", _light.color);
	s->setUniformVec3("light.Specularcolor", glm::vec3(1.0f, 1.0f, 1.0f));
    s->setUniformFloat("light.intensity", _light.intensity);

    _scene->Render();

    if (_ShouldSaveScreenshot) {
        // 生成带时间戳的文件名：screenshot_20231027_123055.png
        std::time_t t = std::time(nullptr);
        std::tm tm = *std::localtime(&t);

        std::ostringstream oss;
        oss << "screenshot_" << std::put_time(&tm, "%Y%m%d_%H%M%S") << ".png";

        saveScreenshot(oss.str());
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Core::doFrame() {
    if (!_manuallycontrollight)
        glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    else
        glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    if (!_manuallycontrollight) {
        _yaw += 0.01f;
        if (_yaw > 180.0f) _yaw -= 360.0f;
        updateLightDirection(_yaw, _pitch);
    }
    // 更新动画
    updateAnimation(_deltaTime);
    // 注意：人物位置更新已经在handleInput()中完成
    _scene->Update(_deltaTime);
}


GameObject* Core::New(ObjectGroup ObjGroup, Model* model, float posx, float posy, float posz, float rotx, float roty, float rotz, float sx, float sy, float sz) {
    GameObject* obj = _scene->CreateObject(ObjGroup);
    obj->AddModel(model);
    obj->SetPosition(glm::vec3(posx, posy, posz));
    obj->SetRotation(glm::vec3(rotx, roty, rotz));
    obj->SetScale(glm::vec3(sx, sy, sz));
    return obj;
}
void Core::SceneInitialize() {
    //建筑
    New(ObjectGroup::Building, ResourceManager::GetModel("building1"), 
        -0.8f, 0.0f, -1.5f, 0.0f, 0.0f, 0.0f, 0.25f, 0.25f, 0.25f);
    New(ObjectGroup::Building, ResourceManager::GetModel("building2"),
		0.8f, -0.0f, -2.2f, 0.0f, 0.0f, 0.0f, 0.25f, 0.25f, 0.25f);
	New(ObjectGroup::Building, ResourceManager::GetModel("building3"),
		0.1f, 0.0f, 0.0f, 0.0f, -4.71f, 0.0f, 0.25f, 0.25f, 0.25f);
	New(ObjectGroup::Building, ResourceManager::GetModel("building4"), 
		-0.7f, 0.0f, -0.75f, 0.0f, 0.0f, 0.0f, 0.25f, 0.25f, 0.25f);
	//道路
    New(ObjectGroup::Object, ResourceManager::GetModel("road1"),
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.25f, 0.25f, 0.25f);
	New(ObjectGroup::Object, ResourceManager::GetModel("road2"),
		0.75f, -0.024f, 0.0f, 0.0f, 0.0f, 0.0f, 0.25f, 0.25f, 0.25f);
	New(ObjectGroup::Object, ResourceManager::GetModel("road3"),
		-0.75f, -0.024f, 0.0f, 0.0f, 0.0f, 0.0f, 0.25f, 0.25f, 0.25f);
    New(ObjectGroup::Object, ResourceManager::GetModel("road1"),
		0.0f, 0.0f, -0.75f, 0.0f, 0.0f, 0.0f, 0.25f, 0.25f, 0.25f);
	New(ObjectGroup::Object, ResourceManager::GetModel("road2"), 
		0.75f, -0.024f, -0.75f, 0.0f, 0.0f, 0.0f, 0.25f, 0.25f, 0.25f);
	New(ObjectGroup::Object, ResourceManager::GetModel("road2"), 
		-0.75f, -0.024f, -0.75f, 0.0f, 0.0f, 0.0f, 0.25f, 0.25f, 0.25f);
	New(ObjectGroup::Object, ResourceManager::GetModel("road1"), 
		0.0f, 0.0f, -1.5f, 0.0f, 0.0f, 0.0f, 0.25f, 0.25f, 0.25f);
	New(ObjectGroup::Object, ResourceManager::GetModel("road2"),
		0.75f, -0.024f, -1.5f, 0.0f, 0.0f, 0.0f, 0.25f, 0.25f, 0.25f);
	New(ObjectGroup::Object, ResourceManager::GetModel("road3"), 
		-0.75f, -0.024f, -1.5f, 0.0f, 0.0f, 0.0f, 0.25f, 0.25f, 0.25f);
	New(ObjectGroup::Object, ResourceManager::GetModel("road1"), 
		0.0f, 0.0f, -2.25f, 0.0f, 0.0f, 0.0f, 0.25f, 0.25f, 0.25f);
	New(ObjectGroup::Object, ResourceManager::GetModel("road3"), 
		0.75f, -0.024f, -2.25f, 0.0f, 0.0f, 0.0f, 0.25f, 0.25f, 0.25f);
	New(ObjectGroup::Object, ResourceManager::GetModel("road2"), 
		-0.75f, -0.024f, -2.25f, 0.0f, 0.0f, 0.0f, 0.25f, 0.25f, 0.25f);
	New(ObjectGroup::Object, ResourceManager::GetModel("road1"), 
		0.0f, 0.0f, -3.0f, 0.0f, 0.0f, 0.0f, 0.25f, 0.25f, 0.25f);
	New(ObjectGroup::Object, ResourceManager::GetModel("road2"),
		0.75f, -0.024f, -3.0f, 0.0f, 0.0f, 0.0f, 0.25f, 0.25f, 0.25f);
	New(ObjectGroup::Object, ResourceManager::GetModel("road3"), 
		-0.75f, -0.024f, -3.0f, 0.0f, 0.0f, 0.0f, 0.25f, 0.25f, 0.25f);
	//树木
    New(ObjectGroup::Building, ResourceManager::GetModel("tree1"),
		-0.6f, 0.0f, -0.4f, 0.0f, 0.0f, 0.0f, 0.25f, 0.25f, 0.25f);
	New(ObjectGroup::Building, ResourceManager::GetModel("tree2"), 
		1.2f, 0.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.25f, 0.25f, 0.25f);
	New(ObjectGroup::Building, ResourceManager::GetModel("tree3"), 
		-1.2f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.25f, 0.25f, 0.25f);

    // 创建人物对象并保存指针
    _character = _scene->CreateObject(ObjectGroup::Player);
    //_character->ApplyMesh(ResourceManager::GetMesh("character"));
    //_character->ApplyMaterial(ResourceManager::GetMaterial("white_material"));
    // 初始设置为静止状态模型
    if (_animationsLoaded && _characterAnimations.size() > 0) {
        _character->AddModel(_characterAnimations[0]);
    }
    else {
        // 回退到原始加载方式
        _character->AddModel(ResourceManager::GetModel("character"));
    }
    _character->SetScale(glm::vec3(0.23f, 0.23f, 0.23f));

    // 初始化建筑物碰撞箱
    updateBuildingColliders();

    // 初始化人物碰撞箱
    _characterCollider = CollisionDetector::CalculateBoundingBox(_character, _collisionScaleFactor);
}

void Core::saveScreenshot(const std::string& filename) {
    int width = _windowWidth, height = _windowHeight;
    std::vector<unsigned char> pixels(width * height * 3);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
    // Flip the image vertically
    std::vector<unsigned char> flippedPixels(width * height * 3);
    for (int y = 0; y < height; ++y) {
        memcpy(&flippedPixels[y * width * 3],
               &pixels[(height - 1 - y) * width * 3],
               width * 3);
    }
    stbi_write_png(filename.c_str(), width, height, 3, flippedPixels.data(), width * 3);
}

void Core::syncCameraAngles() {
    if (!_character) return;

    // 从摄像机的四元数旋转转换为欧拉角
    glm::vec3 euler = glm::eulerAngles(_camera.transform.rotation);
    glm::vec3 degrees = glm::degrees(euler);

    // 规范化角度到 [-180, 180] 范围
    degrees.x = fmod(degrees.x + 180.0f, 360.0f) - 180.0f;
    degrees.y = fmod(degrees.y + 180.0f, 360.0f) - 180.0f;
    degrees.z = fmod(degrees.z + 180.0f, 360.0f) - 180.0f;

    // 同步摄像机角度
    _cameraYaw = degrees.y;
    _cameraPitch = degrees.x;

    // 同步人物角度（只同步Y轴旋转）
    glm::vec3 characterEuler = glm::eulerAngles(_character->GetRotation());
    glm::vec3 characterDegrees = glm::degrees(characterEuler);
    characterDegrees.y = fmod(characterDegrees.y + 180.0f, 360.0f) - 180.0f;
    _characterYaw = characterDegrees.y;
}
void Core::loadCharacterAnimations() {
    if (_animationsLoaded) return;

    _characterAnimations.clear();

    // 加载静止状态（第1帧）
    Model* idleModel = ResourceManager::GetModel("character");
    if (idleModel) {
        _characterAnimations.push_back(idleModel);
    }

    // 加载行走动画帧（02-21）
    for (int i = 2; i <= 21; ++i) {
        std::string frameNum = (i < 10 ? "0" : "") + std::to_string(i);
        std::string modelName = "character_walk_" + frameNum;
        std::string filePath = "../media/obj/character_walk_" + frameNum + ".obj";

        Model* walkModel = ResourceManager::GetModel(modelName);
        if (walkModel) {
            _characterAnimations.push_back(walkModel);
        }
        else {
            std::cerr << "Warning: Failed to load animation frame: " << modelName << std::endl;
        }
    }

    _animationsLoaded = !_characterAnimations.empty();
    if (_animationsLoaded) {
        std::cout << "Loaded " << _characterAnimations.size() << " animation frames" << std::endl;
    }
    else {
        std::cerr << "Error: No animation frames loaded!" << std::endl;
    }
}
void Core::updateAnimation(float deltaTime) {
    if (!_animationsLoaded || _characterAnimations.size() <= 1) return;

    if (_isMoving) {
        _animationTimer += deltaTime;
        if (_animationTimer >= _animationSpeed) {
            _animationTimer = 0.0f;
            _currentAnimationFrame++;

            if (_currentAnimationFrame >= _characterAnimations.size()) {
                _currentAnimationFrame = 1; // 从第一个行走帧开始
            }

            if (_character && _currentAnimationFrame < _characterAnimations.size()) {
                _character->AddModel(_characterAnimations[_currentAnimationFrame]);
            }
        }
    }
    else {
        if (_currentAnimationFrame != 0) {
            _currentAnimationFrame = 0;
            _animationTimer = 0.0f;

            if (_character && _characterAnimations.size() > 0) {
                _character->AddModel(_characterAnimations[0]);
            }
        }
    }
}

void Core::playAnimation() {
    _isMoving = true;
    // 确保从第一个行走帧开始
    if (_currentAnimationFrame == 0 && _characterAnimations.size() > 1) {
        _currentAnimationFrame = 1;
    }
}


void Core::stopAnimation() {
    _isMoving = false;
    // 重置到静止帧
    if (_character && _characterAnimations.size() > 0) {
        _currentAnimationFrame = 0;
        _character->AddModel(_characterAnimations[0]);
    }
}
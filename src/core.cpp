#include<glm/glm.hpp>
#include<ctime>
#include <iomanip>

#include"core.h"
#include"scene.h"
#include"game_object.h"
#include"resource_manager.h"
#include"shader_source.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

Core::Core(const Options& options) : Application(options), _camera(glm::radians(60.0f), 1.0f * _windowWidth / _windowHeight, 0.1f, 10000.0f), _scene(nullptr), _light() {
	_scene = new Scene();
    _light.transform = Transform();
    //_light.transform.rotation = glm::vec3(-1.0f, -1.0f, -1.0f);
    updateLightDirection(_yaw, _pitch);
	_light.intensity = 1.0f;

    _characterDistance = 0.3f; // 人物在摄像机前方0.3个单位
    _characterHeight = 0.25f;   // 人物在摄像机下方0.25个单位
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
   /* ResourceManager::LoadShader("phong_shader",
        "#version 330 core\n"
        "layout(location = 0) in vec3 aPosition;\n"
        "layout(location = 1) in vec3 aNormal;\n"
        "layout(location = 2) in vec2 aTexCoord;\n"

        "out vec3 fPosition;\n"
        "out vec3 fNormal;\n"
        "out vec2 fTexCoord;\n"

        "uniform mat4 projection;\n"
        "uniform mat4 view;\n"
        "uniform mat4 model;\n"

        "void main() {\n"
        "    fPosition = vec3(model * vec4(aPosition, 1.0f));\n"
        "    fNormal = mat3(transpose(inverse(model))) * aNormal;\n"
        "    fTexCoord = aTexCoord;\n"
        "    gl_Position = projection * view * model * vec4(aPosition, 1.0f);\n"
        "}\n",

        "#version 330 core\n"
        "in vec3 fPosition;\n"
        "in vec3 fNormal;\n"
        "in vec2 fTexCoord;\n"
        "out vec4 color;\n"

        "struct DirectionalLight {\n"
        "    vec3 direction;\n"
        "    vec3 color;\n"
        "    float intensity;\n"
        "};\n"

        "struct Material {\n"
        "    vec3 kds[2];\n"
        "    float blend;\n"
        "};\n"

        "uniform Material material;\n"
        "uniform DirectionalLight light;\n"
        "uniform sampler2D mapKds[2];\n"

        "vec3 calcDirectionalLight(vec3 normal, vec3 kds) {\n"
        "    vec3 lightDir = normalize(-light.direction);\n"
        "    vec3 diffuse = light.color * max(dot(lightDir, normal), 0.0f);\n"
        "    return light.color * light.intensity * kds * diffuse;\n"
        "}\n"

        "void main() {\n"
        "    vec3 color1 = texture(mapKds[0], fTexCoord).rgb * material.kds[0];\n"
        "    vec3 color2 = texture(mapKds[1], fTexCoord).rgb * material.kds[1];\n"
        "    vec3 localkds = mix(color1,color2,material.blend);\n"
        "    vec3 normal = normalize(fNormal);\n"
        "    vec3 diffuse = calcDirectionalLight(normal, localkds);\n"
        "    color = vec4(diffuse , 1.0f);\n"
        "}\n");*/
    ResourceManager::LoadShader("Blinn_Phongshader",
        ShaderSource::Blinn_PhongVertexShader,
		ShaderSource::Blinn_PhongFragmentShader);
	//顺序应该是先加载贴图再加材质，然后才能导入mesh
    ResourceManager::LoadTexture("white", "../media/texture/white.png");

    ResourceManager::AddMaterial("white_material", new MixMaterial(ResourceManager::GetShader("Blinn_Phongshader"),
        ResourceManager::GetTexture("white"), ResourceManager::GetTexture("white")));

    ResourceManager::LoadMesh("building1", "../media/obj/fangjian1.obj");
//    ResourceManager::LoadMesh("building1_exported", "../media/obj/building1_exported.obj");
    ResourceManager::LoadMesh("building2", "../media/obj/fangjian2.obj");
    ResourceManager::LoadMesh("building3", "../media/obj/fangjian3.obj");
    ResourceManager::LoadMesh("building4", "../media/obj/fangjian4.obj");

    ResourceManager::LoadMesh("tree1", "../media/obj/shu1.obj");
    ResourceManager::LoadMesh("tree2", "../media/obj/shu2.obj");
    ResourceManager::LoadMesh("tree3", "../media/obj/shu3.obj");

    ResourceManager::LoadMesh("road1", "../media/obj/dizhuan1.obj");
    ResourceManager::LoadMesh("road2", "../media/obj/dizhuan2.obj");
    ResourceManager::LoadMesh("road3", "../media/obj/dizhuan3.obj");

    ResourceManager::LoadMesh("character", "../media/obj/character_walk_01.obj");
	//只是表明我们做了导出功能
	//实际上对于这个项目来说，并不需要导出功能
//	ResourceManager::ExportMesh("building1", "../media/obj/building1_exported.obj");
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
        if (_input.keyboard.keyStates[GLFW_KEY_W] != GLFW_RELEASE) {
            glm::vec3 moveDirection = _character->GetFront(); // 使用人物的前向
            moveDirection.y = 0;
            moveDirection = glm::normalize(moveDirection);
            _character->SetPosition(_character->GetPosition() + moveDirection * cameraMoveSpeed);
        }

        if (_input.keyboard.keyStates[GLFW_KEY_A] != GLFW_RELEASE) {
            glm::vec3 moveDirection = -_character->GetRight(); // 使用人物的右向
            moveDirection.y = 0;
            moveDirection = glm::normalize(moveDirection);
            _character->SetPosition(_character->GetPosition() + moveDirection * cameraMoveSpeed);
        }

        if (_input.keyboard.keyStates[GLFW_KEY_S] != GLFW_RELEASE) {
            glm::vec3 moveDirection = -_character->GetFront();
            moveDirection.y = 0;
            moveDirection = glm::normalize(moveDirection);
            _character->SetPosition(_character->GetPosition() + moveDirection * cameraMoveSpeed);
        }

        if (_input.keyboard.keyStates[GLFW_KEY_D] != GLFW_RELEASE) {
            glm::vec3 moveDirection = _character->GetRight();
            moveDirection.y = 0;
            moveDirection = glm::normalize(moveDirection);
            _character->SetPosition(_character->GetPosition() + moveDirection * cameraMoveSpeed);
        }
        // 添加鼠标移动阈值检查，避免瞬间大角度旋转
        float deltaX = _input.mouse.move.xNow - _input.mouse.move.xOld;
        float deltaY = _input.mouse.move.yNow - _input.mouse.move.yOld;


        // 处理鼠标旋转
        if (deltaX != 0.0f) {
            // 修改3：使用更平滑的旋转计算
            _cameraYaw -= deltaX * cameraRotateSpeed;
            _characterYaw -= deltaX * cameraRotateSpeed;

            // 限制角度
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

//TODO:处理render和frame。建议通过辅助函数，分为render和frame两部分处理，把渲染逻辑和帧逻辑分开。
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

    glClearColor(_clearColor.r, _clearColor.g, _clearColor.b, _clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    render();
    doFrame();
}

void Core::render() {
    GLSLProgram* s = ResourceManager::GetShader("Blinn_Phongshader");
    s->setUniformMat4("projection", _camera.getProjectionMatrix());
    s->setUniformMat4("view", _camera.getViewMatrix());
	s->setUniformVec3("viewPos", _camera.transform.position);
    glm::quat rot = _light.transform.rotation;
    glm::vec3 direction = glm::vec3(rot.x, rot.y, rot.z);
    s->setUniformVec3("light.direction", direction);
    s->setUniformVec3("light.color", _light.color);
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

    // 注意：人物位置更新已经在handleInput()中完成
    _scene->Update(_deltaTime);
}

void Core::UpdateCharacterPosition() {
    if (!_character) return;

    // 计算摄像机在人物身后的位置
    glm::vec3 cameraOffset = -_camera.transform.getFront() * _characterDistance;
    cameraOffset.y = _characterHeight; // 设置高度偏移

    // 设置摄像机位置：人物位置 + 偏移
    _camera.transform.position = _character->GetPosition() + cameraOffset;

    // 现在摄像机位置是由人物位置决定的
    // 人物移动通过WASD控制，摄像机跟随人物
}

void Core::New(ObjectGroup ObjGroup, Mesh* mesh, MixMaterial* material, float posx, float posy, float posz, float rotx, float roty, float rotz, float sx, float sy, float sz) {
    GameObject* obj = _scene->CreateObject(ObjGroup);
    obj->ApplyMesh(mesh);
    obj->ApplyMaterial(material);
    obj->SetPosition(glm::vec3(posx, posy, posz));
    obj->SetRotation(glm::vec3(rotx, roty, rotz));
    obj->SetScale(glm::vec3(sx, sy, sz));
}
void Core::SceneInitialize() {
    GameObject* obj;

    //建筑
    obj = _scene->CreateObject(ObjectGroup::Building);
    obj->ApplyMesh(ResourceManager::GetMesh("building1"));
    obj->ApplyMaterial(ResourceManager::GetMaterial("white_material"));
    obj->SetPosition(glm::vec3(-0.8f, 0.0f, -1.5f));
    obj->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    obj->SetScale(glm::vec3(0.25f, 0.25f, 0.25f));

    obj = _scene->CreateObject(ObjectGroup::Building);
    obj->ApplyMesh(ResourceManager::GetMesh("building2"));
    obj->ApplyMaterial(ResourceManager::GetMaterial("white_material"));
    obj->SetPosition(glm::vec3(0.8f, -0.0f, -2.2f));
    obj->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    obj->SetScale(glm::vec3(0.25f, 0.25f, 0.25f));

    obj = _scene->CreateObject(ObjectGroup::Building);
    obj->ApplyMesh(ResourceManager::GetMesh("building3"));
    obj->ApplyMaterial(ResourceManager::GetMaterial("white_material"));
    obj->SetPosition(glm::vec3(0.1f, 0.0f, 0.0f)); 
    obj->SetRotation(glm::vec3(0.0f, -4.71f, 0.0f));  
    obj->SetScale(glm::vec3(0.25f, 0.25f, 0.25f));

    obj = _scene->CreateObject(ObjectGroup::Building);
    obj->ApplyMesh(ResourceManager::GetMesh("building4"));
    obj->ApplyMaterial(ResourceManager::GetMaterial("white_material"));
    obj->SetPosition(glm::vec3(0.7f, 0.0f, -0.75f));
    obj->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    obj->SetScale(glm::vec3(0.25f, 0.25f, 0.25f));

	//道路
    obj = _scene->CreateObject(ObjectGroup::Object);
    obj->ApplyMesh(ResourceManager::GetMesh("road1"));
    obj->ApplyMaterial(ResourceManager::GetMaterial("white_material"));
    obj->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    obj->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    obj->SetScale(glm::vec3(0.25f, 0.25f, 0.25f));

    obj = _scene->CreateObject(ObjectGroup::Object);
    obj->ApplyMesh(ResourceManager::GetMesh("road2"));
    obj->ApplyMaterial(ResourceManager::GetMaterial("white_material"));
    obj->SetPosition(glm::vec3(0.75f, -0.024f, 0.0f));
    obj->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    obj->SetScale(glm::vec3(0.25f, 0.25f, 0.25f));

    obj = _scene->CreateObject(ObjectGroup::Object);
    obj->ApplyMesh(ResourceManager::GetMesh("road3"));
    obj->ApplyMaterial(ResourceManager::GetMaterial("white_material"));
    obj->SetPosition(glm::vec3(-0.75f, -0.024f, 0.0f));
    obj->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    obj->SetScale(glm::vec3(0.25f, 0.25f, 0.25f));

    obj = _scene->CreateObject(ObjectGroup::Object);
    obj->ApplyMesh(ResourceManager::GetMesh("road1"));
    obj->ApplyMaterial(ResourceManager::GetMaterial("white_material"));
    obj->SetPosition(glm::vec3(0.0f, 0.0f, -0.75f));
    obj->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    obj->SetScale(glm::vec3(0.25f, 0.25f, 0.25f));

    obj = _scene->CreateObject(ObjectGroup::Object);
    obj->ApplyMesh(ResourceManager::GetMesh("road3"));
    obj->ApplyMaterial(ResourceManager::GetMaterial("white_material"));
    obj->SetPosition(glm::vec3(0.75f, -0.024f, -0.75f));
    obj->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    obj->SetScale(glm::vec3(0.25f, 0.25f, 0.25f));

    obj = _scene->CreateObject(ObjectGroup::Object);
    obj->ApplyMesh(ResourceManager::GetMesh("road2"));
    obj->ApplyMaterial(ResourceManager::GetMaterial("white_material"));
    obj->SetPosition(glm::vec3(-0.75f, -0.024f, -0.75f));
    obj->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    obj->SetScale(glm::vec3(0.25f, 0.25f, 0.25f));

    obj = _scene->CreateObject(ObjectGroup::Object);
    obj->ApplyMesh(ResourceManager::GetMesh("road1"));
    obj->ApplyMaterial(ResourceManager::GetMaterial("white_material"));
    obj->SetPosition(glm::vec3(0.0f, 0.0f, -1.5f));
    obj->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    obj->SetScale(glm::vec3(0.25f, 0.25f, 0.25f));

    obj = _scene->CreateObject(ObjectGroup::Object);
    obj->ApplyMesh(ResourceManager::GetMesh("road2"));
    obj->ApplyMaterial(ResourceManager::GetMaterial("white_material"));
    obj->SetPosition(glm::vec3(0.75f, -0.024f, -1.5f));
    obj->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    obj->SetScale(glm::vec3(0.25f, 0.25f, 0.25f));

    obj = _scene->CreateObject(ObjectGroup::Object);
    obj->ApplyMesh(ResourceManager::GetMesh("road3"));
    obj->ApplyMaterial(ResourceManager::GetMaterial("white_material"));
    obj->SetPosition(glm::vec3(-0.75f, -0.024f, -1.5f));
    obj->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    obj->SetScale(glm::vec3(0.25f, 0.25f, 0.25f));

    obj = _scene->CreateObject(ObjectGroup::Object);
    obj->ApplyMesh(ResourceManager::GetMesh("road1"));
    obj->ApplyMaterial(ResourceManager::GetMaterial("white_material"));
    obj->SetPosition(glm::vec3(0.0f, 0.0f, -2.25f));
    obj->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    obj->SetScale(glm::vec3(0.25f, 0.25f, 0.25f));

    obj = _scene->CreateObject(ObjectGroup::Object);
    obj->ApplyMesh(ResourceManager::GetMesh("road3"));
    obj->ApplyMaterial(ResourceManager::GetMaterial("white_material"));
    obj->SetPosition(glm::vec3(0.75f, -0.024f, -2.25f));
    obj->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    obj->SetScale(glm::vec3(0.25f, 0.25f, 0.25f));

    obj = _scene->CreateObject(ObjectGroup::Object);
    obj->ApplyMesh(ResourceManager::GetMesh("road2"));
    obj->ApplyMaterial(ResourceManager::GetMaterial("white_material"));
    obj->SetPosition(glm::vec3(-0.75f, -0.024f, -2.25f));
    obj->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    obj->SetScale(glm::vec3(0.25f, 0.25f, 0.25f));

    obj = _scene->CreateObject(ObjectGroup::Object);
    obj->ApplyMesh(ResourceManager::GetMesh("road1"));
    obj->ApplyMaterial(ResourceManager::GetMaterial("white_material"));
    obj->SetPosition(glm::vec3(0.0f, 0.0f, -3.0f));
    obj->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    obj->SetScale(glm::vec3(0.25f, 0.25f, 0.25f));

    obj = _scene->CreateObject(ObjectGroup::Object);
    obj->ApplyMesh(ResourceManager::GetMesh("road2"));
    obj->ApplyMaterial(ResourceManager::GetMaterial("white_material"));
    obj->SetPosition(glm::vec3(0.75f, -0.024f, -3.0f));
    obj->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    obj->SetScale(glm::vec3(0.25f, 0.25f, 0.25f));

    obj = _scene->CreateObject(ObjectGroup::Object);
    obj->ApplyMesh(ResourceManager::GetMesh("road3"));
    obj->ApplyMaterial(ResourceManager::GetMaterial("white_material"));
    obj->SetPosition(glm::vec3(-0.75f, -0.024f, -3.0f));
    obj->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    obj->SetScale(glm::vec3(0.25f, 0.25f, 0.25f));

	//树木
    obj = _scene->CreateObject(ObjectGroup::Object);
    obj->ApplyMesh(ResourceManager::GetMesh("tree1"));
    obj->ApplyMaterial(ResourceManager::GetMaterial("white_material"));
    obj->SetPosition(glm::vec3(0.6f, 0.0f, -0.4f));
    obj->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    obj->SetScale(glm::vec3(0.25f, 0.25f, 0.25f));

    obj = _scene->CreateObject(ObjectGroup::Object);
    obj->ApplyMesh(ResourceManager::GetMesh("tree2"));
    obj->ApplyMaterial(ResourceManager::GetMaterial("white_material"));
    obj->SetPosition(glm::vec3(-1.2f, 0.0f, -1.0f));
    obj->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    obj->SetScale(glm::vec3(0.25f, 0.25f, 0.25f));

    obj = _scene->CreateObject(ObjectGroup::Object);
    obj->ApplyMesh(ResourceManager::GetMesh("tree3"));
    obj->ApplyMaterial(ResourceManager::GetMaterial("white_material"));
    obj->SetPosition(glm::vec3(1.0f, 0.0f, -2.0f));
    obj->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    obj->SetScale(glm::vec3(0.25f, 0.25f, 0.25f));

    // 创建人物对象并保存指针
    _character = _scene->CreateObject(ObjectGroup::Player);
    _character->ApplyMesh(ResourceManager::GetMesh("character"));
    _character->ApplyMaterial(ResourceManager::GetMaterial("white_material"));
    _character->SetScale(glm::vec3(0.1f, 0.1f, 0.1f));
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

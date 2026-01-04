#include<glm/glm.hpp>
#include"core.h"
#include"scene.h"
#include"game_object.h"
#include"resource_manager.h"

Core::Core(const Options& options) : Application(options), _camera(glm::radians(60.0f), 1.0f * _windowWidth / _windowHeight, 0.1f, 10000.0f), _scene(nullptr), _light() {
	_scene = new Scene();
    _light.transform = Transform();
    _light.transform.rotation = glm::vec3(-1.0f, -1.0f, -1.0f);
	init();
}

Core::~Core() {
	delete _scene;
}

void Core::init() {
    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    _input.mouse.move.xNow = _input.mouse.move.xOld = 0.5f * _windowWidth;
    _input.mouse.move.yNow = _input.mouse.move.yOld = 0.5f * _windowHeight;
    glfwSetCursorPos(_window, _input.mouse.move.xNow, _input.mouse.move.yNow);
    ResourceManager::LoadShader("phong_shader", 
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
        "}\n");

    ResourceManager::LoadMesh("building1", "../media/obj/fangjian1.obj");
    ResourceManager::LoadMesh("building2", "../media/obj/fangjian2.obj");
    ResourceManager::LoadMesh("bunny", "../media/obj/bunny.obj");

    ResourceManager::LoadTexture("white", "../media/texture/white.png");

    ResourceManager::AddMaterial("white_material", new MixMaterial(ResourceManager::GetShader("phong_shader"),
        ResourceManager::GetTexture("white"), ResourceManager::GetTexture("white")));

    SceneInitialize();
}

//TODO:处理输入
void Core::handleInput() {
    constexpr float cameraMoveSpeed = 0.2f;
    constexpr float cameraRotateSpeed = 0.01f;

    PerspectiveCamera* camera = &_camera;

    if (_input.keyboard.keyStates[GLFW_KEY_ESCAPE] != GLFW_RELEASE) {
        glfwSetWindowShouldClose(_window, true);
        return;
    }
    if (_input.keyboard.keyStates[GLFW_KEY_W] != GLFW_RELEASE) {
        std::cout << "W" << std::endl;
        glm::vec3 MoveSpeed = glm::vec3(0.0f, 0.0f, -cameraMoveSpeed)*camera->transform.rotation;
        camera->transform.position = camera->transform.position + MoveSpeed;
    }

    if (_input.keyboard.keyStates[GLFW_KEY_A] != GLFW_RELEASE) {
        std::cout << "A" << std::endl;
        glm::vec3 MoveSpeed = glm::vec3(- cameraMoveSpeed, 0.0f, 0.0f)* camera->transform.rotation;
        camera->transform.position = camera->transform.position + MoveSpeed;
    }

    if (_input.keyboard.keyStates[GLFW_KEY_S] != GLFW_RELEASE) {
        std::cout << "S" << std::endl;
        glm::vec3 MoveSpeed = glm::vec3(0.0f,0.0f,cameraMoveSpeed) * camera->transform.rotation;
        camera->transform.position = camera->transform.position + MoveSpeed;
    }

    if (_input.keyboard.keyStates[GLFW_KEY_D] != GLFW_RELEASE) {
        std::cout << "D" << std::endl;
        glm::vec3 MoveSpeed = glm::vec3(cameraMoveSpeed,0.0f,0.0f);
        camera->transform.position = camera->transform.position + MoveSpeed;
    }

    if (_input.mouse.move.xNow != _input.mouse.move.xOld) {
        std::cout << "mouse move in x direction" << std::endl;
        float mousemove_x = _input.mouse.move.xNow - _input.mouse.move.xOld;
        glm::vec3 up = { 0.0f,1.0f,0.0f };
        glm::quat move = glm::angleAxis(-mousemove_x * cameraRotateSpeed, up);
        camera->transform.rotation = move * camera->transform.rotation;
    }

    if (_input.mouse.move.yNow != _input.mouse.move.yOld) {
        std::cout << "mouse move in y direction" << std::endl;
        float mousemove_y = _input.mouse.move.yNow - _input.mouse.move.yOld;
        glm::quat move = glm::angleAxis(-mousemove_y * cameraRotateSpeed, camera->transform.getRight());
        camera->transform.rotation = move * camera->transform.rotation;
    }

    _input.forwardState();
}

//TODO:处理render和frame。建议通过辅助函数，分为render和frame两部分处理，把渲染逻辑和帧逻辑分开。
void Core::renderFrame() {
    glClearColor(_clearColor.r, _clearColor.g, _clearColor.b, _clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    render();
}

void Core::render() {
    GLSLProgram* s = ResourceManager::GetShader("phong_shader");
    s->setUniformMat4("projection", _camera.getProjectionMatrix());
    s->setUniformMat4("view", _camera.getViewMatrix());
    s->setUniformVec3("light.direction", _light.transform.getFront());
    s->setUniformVec3("light.color", _light.color);
    s->setUniformFloat("light.intensity", _light.intensity);

    _scene->Render();
}

void Core::SceneInitialize() {
    GameObject* obj;
    obj = _scene->CreateObject(ObjectGroup::Building);
    obj->ApplyMesh(ResourceManager::GetMesh("building1"));
    obj->ApplyMaterial(ResourceManager::GetMaterial("white_material"));
    obj->SetPosition(glm::vec3(-0.5f, 0.0f, -15.0f));
    obj->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    obj->SetScale(glm::vec3(0.25f, 0.25f, 0.25f));

    obj = _scene->CreateObject(ObjectGroup::Building);
    obj->ApplyMesh(ResourceManager::GetMesh("building2"));
    obj->ApplyMaterial(ResourceManager::GetMaterial("white_material"));
    obj->SetPosition(glm::vec3(0.5f, 0.0f, -15.0f));
    obj->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    obj->SetScale(glm::vec3(0.25f, 0.25f, 0.25f));

    obj = _scene->CreateObject(ObjectGroup::Building);
    obj->ApplyMesh(ResourceManager::GetMesh("bunny"));
    obj->ApplyMaterial(ResourceManager::GetMaterial("white_material"));
    obj->SetPosition(glm::vec3(0.0f, 0.0f, -15.0f));
    obj->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    obj->SetScale(glm::vec3(0.25f, 0.25f, 0.25f));
}
// scene.cpp
#include "scene.h"
#include "game_object.h"
#include <iostream>

// 构造函数
Scene::Scene() : nextObjectId(1) {  // 使用初始化列表
}

// 析构函数
Scene::~Scene() {
    ClearAll();
}

// 添加已有对象
void Scene::AddObject(GameObject* obj) {  // 对应头文件的AddObject
    if (obj) {
        // 检查是否已存在相同ID的对象
        if (gameObjects.find(obj->id) != gameObjects.end()) {
            std::cout << "Warning: GameObject with ID " << obj->id << " already exists!" << std::endl;
            return;
        }
        gameObjects[obj->id] = obj;
    }
}

// 创建新对象
GameObject* Scene::CreateObject(ObjectGroup group) {  // 对应头文件的CreateObject
    GameObject* obj = new GameObject(nextObjectId++, group);
    gameObjects[obj->id] = obj;
    std::cout << "Created GameObject with ID: " << obj->id << std::endl;
    return obj;
}

// 通过ID获取对象
GameObject* Scene::GetObjectById(unsigned int id) {  // 对应头文件的GetObjectById
    auto it = gameObjects.find(id);
    if (it != gameObjects.end()) {
        return it->second;
    }
    return nullptr;
}

// 通过组获取所有对象
std::vector<GameObject*> Scene::GetObjectsByGroup(ObjectGroup group) {
    std::vector<GameObject*> result;
    for (const auto& pair : gameObjects) {
        if (pair.second->group == group) {
            result.push_back(pair.second);
        }
    }
    return result;
}

// 移除对象
void Scene::RemoveObject(unsigned int id) {
    auto it = gameObjects.find(id);
    if (it != gameObjects.end()) {
        delete it->second;
        gameObjects.erase(it);
    }
}

// 清空所有对象
void Scene::ClearAll() {
    for (auto& obj : gameObjects) {
        delete obj.second;
    }
    gameObjects.clear();
    nextObjectId = 1;  // 重置ID
}

// 更新场景
void Scene::Update(float deltaTime) {
    // 实现更新逻辑
    for (auto& pair : gameObjects) {
        GameObject* obj = pair.second;
        // 更新逻辑...
    }
}

// 渲染场景
void Scene::Render() {
    for (auto& pair : gameObjects) {
        GameObject* obj = pair.second;
        if (obj->GetMesh() && obj->GetMaterial()) {
            obj->Draw();
        }
    }
}

// 按组渲染
void Scene::RenderGroup(ObjectGroup group, GLSLProgram* shader) {
    for (auto& pair : gameObjects) {
        GameObject* obj = pair.second;
        if (obj->group == group && obj->GetMesh() && obj->GetMaterial()) {
            if (shader) {
                // 使用指定的shader
            }
            obj->Draw();
        }
    }
}

// 获取对象数量
size_t Scene::GetObjectCount() const {
    return gameObjects.size();
}

// 门开关逻辑
void Scene::UpdateDoors(float deltaTime) {
    auto doors = GetObjectsByGroup(ObjectGroup::Building);
    GameObject* player = GetPlayer();

    if (!player) return;

    for (auto door : doors) {
        if (!door->isDoor) continue;

        glm::vec3 playerPos = player->GetPosition();
        glm::vec3 doorPos = door->GetPosition();
        float distance = glm::distance(playerPos, doorPos);

        if (distance < 3.0f) {
            // 这里可以添加门交互逻辑
            // 例如：door->canInteract = true;
        }
    }
}

// 批量控制路灯
void Scene::ToggleStreetLights(bool on) {
    auto lamps = GetObjectsByGroup(ObjectGroup::StreetLamp);
    for (auto lamp : lamps) {
        // 这里可以控制路灯的材质/光照状态
        // 例如：lamp->SetLightEnabled(on);
    }
}

// 获取玩家对象
GameObject* Scene::GetPlayer() {
    for (auto& pair : gameObjects) {
        if (pair.second->group == ObjectGroup::Player) {
            return pair.second;
        }
    }
    return nullptr;
}

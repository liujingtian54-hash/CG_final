// scene.cpp
#include "scene.h"
#include "game_object.h"
#include <algorithm>
#include <iostream>

// 构造函数
Scene::Scene() {
    nextObjectId = 1;
}

// 析构函数
Scene::~Scene() {
    for (auto& obj : gameObjects) {
        delete obj.second;
    }
    gameObjects.clear();
}

// 添加游戏对象
GameObject* Scene::AddGameObject(ObjectGroup group) {
    GameObject* obj = new GameObject(nextObjectId++, group);
    gameObjects[obj->id] = obj;
    std::cout << "Created GameObject with ID: " << obj->id << std::endl;
    return obj;
}

// 通过ID获取对象
GameObject* Scene::GetGameObject(unsigned int id) {
    auto it = gameObjects.find(id);
    if (it != gameObjects.end()) {
        return it->second;
    }
    return nullptr;
}

// 移除游戏对象
bool Scene::RemoveGameObject(unsigned int id) {
    auto it = gameObjects.find(id);
    if (it != gameObjects.end()) {
        delete it->second;
        gameObjects.erase(it);
        return true;
    }
    return false;
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

// 更新场景中所有对象
void Scene::Update(float deltaTime) {
    // 更新所有对象
    for (auto& pair : gameObjects) {
        GameObject* obj = pair.second;

        // 这里可以添加特定类型的逻辑
        switch (obj->group) {
        case ObjectGroup::StreetLamp:
            // 路灯开关逻辑
            break;
        case ObjectGroup::Building:
            // 建筑特定逻辑，如门开关
            if (obj->isDoor) {
                obj->UpdateDoor(deltaTime);
            }
            break;
        default:
            break;
        }
    }

    // 门开关逻辑
    UpdateDoors(deltaTime);
}

// 渲染所有对象
void Scene::Render() {
    for (auto& pair : gameObjects) {
        GameObject* obj = pair.second;
        if (obj->GetMesh() && obj->GetMaterial()) {
            obj->Draw();
        }
    }
}

// 门开关逻辑
void Scene::UpdateDoors(float deltaTime) {
    auto doors = GetObjectsByGroup(ObjectGroup::Building);
    GameObject* player = GetPlayer();

    if (!player) return;

    for (auto door : doors) {
        // 检查是否是可以交互的门
        if (!door->isDoor) continue;

        // 使用GameObject的GetPosition()方法
        glm::vec3 playerPos = player->GetPosition(); 
        glm::vec3 doorPos = door->GetPosition();     

        float distance = glm::distance(playerPos, doorPos);

        // 如果玩家在门附近
        if (distance < 3.0f) {
            // 这里可以设置门为可交互状态
            // 当玩家按E时，在handleInput中触发门的开关
            // 例如：door->isInteractable = true;
        }
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

// 批量控制路灯
void Scene::ToggleStreetLights(bool on) {
    auto lamps = GetObjectsByGroup(ObjectGroup::StreetLamp);
    for (auto lamp : lamps) {
        // 这里可以控制路灯的材质/光照状态
        // 例如：可以设置一个lightOn属性，在材质中控制
    }
}
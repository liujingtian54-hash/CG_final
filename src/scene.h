// scene.h
#pragma once
#include <vector>
#include <unordered_map>
#include "game_object.h"

class Scene {
private:
    std::unordered_map<unsigned int, GameObject*> gameObjects;
    unsigned int nextObjectId = 1;  // 给默认值

    // 门开关逻辑
    void UpdateDoors(float deltaTime);

public:
    Scene();
    ~Scene();

    // 使用Scene.cpp中实现的接口
    void AddObject(GameObject* obj);                       // 与.cpp一致
    GameObject* CreateObject(ObjectGroup group = ObjectGroup::Object);  // 与.cpp一致
    GameObject* GetObjectById(unsigned int id);           // 与.cpp一致
    std::vector<GameObject*> GetObjectsByGroup(ObjectGroup group);  // 与.cpp一致
    void RemoveObject(unsigned int id);                    // 与.cpp一致
    void ClearAll();                                       // 与.cpp一致
    size_t GetObjectCount() const;                         // 与.cpp一致

    // 按组渲染（可选添加）
    void RenderGroup(ObjectGroup group, GLSLProgram* shader = nullptr);

    // 获取特定对象
    GameObject* GetPlayer();

    // 特殊功能
    void ToggleStreetLights(bool on);

    // 更新和渲染
    void Update(float deltaTime);
    void Render();

    // 获取所有对象（用于迭代）
    const std::unordered_map<unsigned int, GameObject*>& GetAllObjects() const {
        return gameObjects;
    }
};
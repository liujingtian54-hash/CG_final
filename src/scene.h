#pragma once
#include <vector>
#include <unordered_map>
#include "game_object.h"
#include "base/glsl_program.h"

class Scene {
private:
    std::vector<GameObject*> objects;
    std::unordered_map<ObjectGroup, std::vector<GameObject*>> objectGroups;
    unsigned int nextId;

    // 辅助更新方法
    void UpdateVehicles(float deltaTime);
    void UpdateStreetLamps(float deltaTime);

public:
    Scene();
    ~Scene();

    // 对象管理
    void AddObject(GameObject* obj);
    GameObject* CreateObject(ObjectGroup group = ObjectGroup::Object);
    GameObject* GetObjectById(unsigned int id);
    std::vector<GameObject*> GetObjectsByGroup(ObjectGroup group);
    void RemoveObject(unsigned int id);
    void ClearAll();

    // 更新和渲染
    void Update(float deltaTime);
    void Render();
    void RenderGroup(ObjectGroup group, GLSLProgram* shader = nullptr);

    // 查询
    size_t GetObjectCount() const;
    size_t GetGroupCount(ObjectGroup group) const;

    // 获取所有对象（只读）
    const std::vector<GameObject*>& GetAllObjects() const { return objects; }
};
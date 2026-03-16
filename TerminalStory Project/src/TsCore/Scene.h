#pragma once

#include "Base.h"
#include "Command.h"

class Scene {
public:
    // 从JSON数组加载指令和标签
    bool loadFromJson(const nlohmann::json& j);

    std::vector<std::unique_ptr<Command>> commands;
    std::map<std::string, size_t> labelToIndex; // 标签名 -> 指令索引
};

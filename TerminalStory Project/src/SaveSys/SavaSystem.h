#pragma once

#include "Base.h"

#include "../SaveSys/SavaSystem.h"
#include "../Function/GameState.h"

struct GameState; // 前向声明 GameState.h

struct SaveData {
    int id = 0;                         // 存档编号(自动分配)
    std::string name;                   // 玩家自定义名称
    std::string sceneId;                // 当前场景ID
    std::map<std::string, int> flags;   // 剧情变量(存档时保存)

    size_t commandIndex = 0;
};

int saveGame(const GameState& state, const std::string& saveName);

void ensureSaveDir();

std::vector<SaveData> listSaves();

SaveData loadGame(int id);
#pragma once

#include "Base.h"

struct GameState {
    std::string sceneId;                 // 当前场景ID（对应JSON文件名）
    size_t commandIndex = 0;             // 当前执行的指令索引
    std::map<std::string, int> flags;    // 剧情变量
};

#pragma once

#define NOMINMAX
#include <limits>
#include "Base.h"

#include "Scene.h"
#include "../Function/GameState.h"
#include "../../TerminalCore/src/BaseFunction/ConsoleUtils.h"
#include "../SaveSys/SavaSystem.h"
#include "../Function/GameTypeText.h"

class JsonEngine {
public:
    // 加载所有剧本文件[从Chapters文件夹]
    bool loadAllChapters(const std::string& folderPath);

    // 注册自定义函数[供call指令使用]
    void registerFunction(const std::string& name, std::function<void()> func);

    // 运行指定场景 从startIndex开始[用于读档]
    void runScene(const std::string& sceneId, size_t startIndex = 0);

    // 获取/设置当前状态[用于存档]
    GameState getCurrentState() const { return currentState; }
    void setState(const GameState& state) { currentState = state; }

    // 保存当前游戏[触发存档界面]
    void saveCurrent();

    // 调试打印标志
    void debugPrintFlags();

private:
    std::map<std::string, std::unique_ptr<Scene>> scenes;
    std::map<std::string, std::function<void()>> functions;
    GameState currentState;

    void executeCommand(Command* cmd);
    int showOptions(const std::vector<OptionCommand::Choice>& choices);
    void jumpToLabel(const std::string& label, const std::optional<std::string>& scene);
    bool checkCondition(const std::string& cond, const GameState& state);
};
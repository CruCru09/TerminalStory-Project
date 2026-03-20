#include "JsonEngine.h"


// 加载所有章节 [.json]
bool JsonEngine::loadAllChapters(const std::string& folderPath) {
    namespace fs = std::filesystem;
    bool anyLoaded = false;
    if (!fs::exists(folderPath)) return false;
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.path().extension() == ".json") {
            std::ifstream file(entry.path());
            if (!file.is_open()) continue;
            nlohmann::json j;
            try {
                file >> j;
            }
            catch (...) {
                continue;
            }
            std::string sceneId = entry.path().stem().string();
            auto scene = std::make_unique<Scene>();
            if (scene->loadFromJson(j)) {
                scenes[sceneId] = std::move(scene);
                anyLoaded = true;
            }
        }
    }
    return anyLoaded;
}


// 注册函数
void JsonEngine::registerFunction(const std::string& name, std::function<void()> func) {
    functions[name] = func;
}


// 运行场景
void JsonEngine::runScene(const std::string& sceneId, size_t startIndex) {
    currentState.sceneId = sceneId;
    currentState.commandIndex = startIndex;

    while (true) {
        auto it = scenes.find(currentState.sceneId);
        if (it == scenes.end()) {
            std::cerr << "场景不存在: " << currentState.sceneId << std::endl;
            break;
        }
        Scene* scene = it->second.get();
        if (currentState.commandIndex >= scene->commands.size()) {
            break; // 场景结束
        }
        Command* cmd = scene->commands[currentState.commandIndex].get();
        executeCommand(cmd);
    }
}


// 执行命令
void JsonEngine::executeCommand(Command* cmd) {
    // 先检查指令自身的条件
    if (cmd->condition.has_value()) {
        if (!checkCondition(cmd->condition.value(), currentState)) {
            // 条件不满足 跳过
            currentState.commandIndex++;
            return;
        }
    }

    switch (cmd->type()) {
    case CommandType::Text: {
        auto t = static_cast<TextCommand*>(cmd);
        typeTextWithSave(t->text, t->color, t->speed, t->waitAtEnd, this);
        currentState.commandIndex++;
        break;
    }
    case CommandType::Option: {
        auto o = static_cast<OptionCommand*>(cmd);
        int choice = showOptions(o->choices);
        if (choice >= 0) {
            const auto& chosen = o->choices[choice];
            jumpToLabel(chosen.targetLabel, chosen.targetScene);
        }
        else {
            // 没有可用选项 跳过当前指令
            currentState.commandIndex++;
        }
        break;
    }
    case CommandType::Call: {
        auto c = static_cast<CallCommand*>(cmd);
        auto fit = functions.find(c->funcName);
        if (fit != functions.end()) {
            fit->second();
        }
        else {
            std::cerr << "未注册的函数: " << c->funcName << std::endl;
        }
        currentState.commandIndex++;
        break;
    }
    case CommandType::Jump: {
        auto j = static_cast<JumpCommand*>(cmd);
        jumpToLabel(j->targetLabel, j->targetScene);
        break;
    }
    case CommandType::SetFlag: {
        auto s = static_cast<SetFlagCommand*>(cmd);
        currentState.flags[s->flagName] = s->value;
        currentState.commandIndex++;
        break;
    }
    case CommandType::End: {
        currentState.commandIndex = scenes[currentState.sceneId]->commands.size(); // 结束循环
        break;
    }
    }
}


// 显示选项
int JsonEngine::showOptions(const std::vector<OptionCommand::Choice>& choices) {
    // 收集符合条件的选项
    std::vector<OptionCommand::Choice> validChoices;
    std::vector<size_t> originalIndices; // 存储原始索引

    for (size_t i = 0; i < choices.size(); ++i) {
        const auto& ch = choices[i];
        if (ch.condition.has_value()) {
            if (!checkCondition(ch.condition.value(), currentState)) {
                continue; // 不满足条件
            }
        }
        validChoices.push_back(ch);
        originalIndices.push_back(i);
    }

    if (validChoices.empty()) {
        // 没有可用选项，返回 -1 让上层跳过
        return -1;
    }

    int choice = -1;
    while (choice < 0) {
        system("cls");
        std::cout << "请选择：" << std::endl;
        for (size_t i = 0; i < validChoices.size(); ++i) {
            std::cout << i + 1 << ". " << validChoices[i].text << std::endl;
        }
        int key = _getch();
        if (key == 0 || key == 224) {
            key = _getch();
            if (key == 63) { // F5
                saveCurrent();
                continue;
            }
        }
        else if (key >= '1' && key <= '9') {
            int idx = key - '1';
            // 将 idx 转换为 size_t 进行比较 避免警告
            if (static_cast<size_t>(idx) < validChoices.size()) {
                // 将 originalIndices 中的 size_t 显式转换为 int
                choice = static_cast<int>(originalIndices[static_cast<size_t>(idx)]);
                break;
            }
        }
    }
    return choice;
}


// 跳转到标签
void JsonEngine::jumpToLabel(const std::string& label, const std::optional<std::string>& scene) {
    std::string targetSceneId = scene.value_or(currentState.sceneId);
    auto it = scenes.find(targetSceneId);
    if (it == scenes.end()) {
        std::cerr << "场景不存在: " << targetSceneId << std::endl;
        currentState.commandIndex++; // 跳过当前指令
        return;
    }
    Scene* targetScene = it->second.get();
    auto lit = targetScene->labelToIndex.find(label);
    if (lit != targetScene->labelToIndex.end()) {
        currentState.sceneId = targetSceneId;   // 切换场景
        currentState.commandIndex = lit->second;
    }
    else {
        std::cerr << "标签不存在: " << label << " 在场景 " << targetSceneId << std::endl;
        currentState.commandIndex++;
    }
}


// 保存当前状态
void JsonEngine::saveCurrent() {
    system("cls");
    std::string name;

    while (true) {
        std::cout << " ===== 保存游戏 =====\n\n";

        std::cout << " 输入存档名称: ";

        // 清空输入缓冲区
        std::cin.clear();

        // 读取整行输入
        std::getline(std::cin, name);

        if (!name.empty()) {
            break;
        }
        std::cout << "\n 名称不能为空哦" << std::endl;
        (void)_getch();
        system("cls");
    }

    int newId = saveGame(currentState, name);

    if (newId != -1) {
        std::cout << " \n存好了喵 编号: " << newId << std::endl;
    }
    else {
        std::cout << " \n存档失败!" << std::endl;
    }

    std::cout << "按任意键继续..." << std::endl;
    (void)_getch();
}


void JsonEngine::debugPrintFlags() {
    system("cls");
    setColor(14); // 黄色
    std::cout << " ===== 当前变量列表 =====" << std::endl;
    resetColor();

    if (currentState.flags.empty()) {
        std::cout << "\n [没有设置任何变量]" << std::endl;
    }
    else {
        for (const auto& [key, val] : currentState.flags) {
            std::cout << std::left << std::setw(15) << key << " = " << val << std::endl;
        }
    }
    std::cout << "\n按任意键继续..." << std::endl;
    (void)_getch();
}

// 检查条件
bool JsonEngine::checkCondition(const std::string& cond, const GameState& state) {
    std::string s = cond;
    // 去除首尾空格
    s.erase(0, s.find_first_not_of(" \t\n\r"));
    s.erase(s.find_last_not_of(" \t\n\r") + 1);

    // 正则匹配：变量名 操作符 数字
    std::regex pattern(R"(^\s*([a-zA-Z_][a-zA-Z0-9_]*)\s*(==|!=|>|<|>=|<=)\s*(-?\d+)\s*$)");
    std::smatch match;
    if (std::regex_match(s, match, pattern)) {
        std::string flag = match[1];
        std::string op = match[2];
        int value = std::stoi(match[3]);

        int flagValue = 0;
        auto it = state.flags.find(flag);
        if (it != state.flags.end()) {
            flagValue = it->second;
        }

        if (op == "==") return flagValue == value;
        if (op == "!=") return flagValue != value;
        if (op == ">")  return flagValue > value;
        if (op == "<")  return flagValue < value;
        if (op == ">=") return flagValue >= value;
        if (op == "<=") return flagValue <= value;
    }
    // 格式错误 输出警告并返回 false[视为不满足]
    std::cerr << "警告：条件格式错误，视为不满足: " << cond << std::endl;
    return false;
}
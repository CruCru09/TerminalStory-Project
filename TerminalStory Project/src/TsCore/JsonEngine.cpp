#include "JsonEngine.h"

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

void JsonEngine::registerFunction(const std::string& name, std::function<void()> func) {
    functions[name] = func;
}

void JsonEngine::runScene(const std::string& sceneId, size_t startIndex) {
    auto it = scenes.find(sceneId);
    if (it == scenes.end()) {
        std::cerr << "场景不存在: " << sceneId << std::endl;
        return;
    }
    Scene* scene = it->second.get();
    currentState.sceneId = sceneId;
    currentState.commandIndex = startIndex;

    while (currentState.commandIndex < scene->commands.size()) {
        Command* cmd = scene->commands[currentState.commandIndex].get();
        executeCommand(cmd);
    }
}

void JsonEngine::executeCommand(Command* cmd) {
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
            jumpToLabel(o->choices[choice].targetLabel);
        }
        else {
            currentState.commandIndex++; // 如果用户取消(暂不支持) 跳过
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
            std::cerr << "未注册函数: " << c->funcName << std::endl;
        }
        currentState.commandIndex++;
        break;
    }
    case CommandType::Jump: {
        auto j = static_cast<JumpCommand*>(cmd);
        jumpToLabel(j->targetLabel);
        break;
    }
    case CommandType::SetFlag: {
        auto s = static_cast<SetFlagCommand*>(cmd);
        currentState.flags[s->flagName] = s->value;
        currentState.commandIndex++;
        break;
    }
    case CommandType::End: {
        currentState.commandIndex = scenes[currentState.sceneId]->commands.size(); // 结束
        break;
    }
    }
}

int JsonEngine::showOptions(const std::vector<OptionCommand::Choice>& choices) {
    int choice = -1;
    while (choice < 0) {
        system("cls");
        std::cout << "请选择: \n" << std::endl;
        for (size_t i = 0; i < choices.size(); ++i) {
            std::cout << i + 1 << ". " << choices[i].text << std::endl;
        }
        int key = _getch();
        if (key == 0 || key == 224) {
            key = _getch();
            if (key == 63) { // F5
                saveCurrent();
                continue; // 保存后继续显示选项
            }
        }
        else if (key >= '1' && key <= '9') {
            int idx = key - '1';
            if (idx < choices.size()) {
                choice = idx;
                break;
            }
        }
    }
    return choice;
}

void JsonEngine::jumpToLabel(const std::string& label) {
    auto it = scenes.find(currentState.sceneId);
    if (it == scenes.end()) return;
    Scene* scene = it->second.get();
    auto lit = scene->labelToIndex.find(label);
    if (lit != scene->labelToIndex.end()) {
        currentState.commandIndex = lit->second;
    }
    else {
        std::cerr << "标签不存在: " << label << std::endl;
        currentState.commandIndex++; // 找不到标签 跳过当前指令
    }
}

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
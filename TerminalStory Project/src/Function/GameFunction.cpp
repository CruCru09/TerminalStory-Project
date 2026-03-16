#pragma comment(lib, "winmm.lib")
#include "GameFunction.h"

JsonEngine g_JSengine; // JSON引擎对象

void GameFunction::StartGame() {

    system("cls");

    // 初始化游戏状态：第一章开始
    GameState initialState;
    initialState.sceneId = "Chapter1"; // 对应 Chapters/Chapter1.json
    initialState.commandIndex = 0;
    initialState.flags.clear();
    g_JSengine.setState(initialState);
    g_JSengine.runScene("Chapter1", 0);

    system("cls");
    system("pause");
}


void GameFunction::loadGameMenu() {

    // 获取所有存档列表
    auto saves = listSaves();
    if (saves.empty()) {
        system("cls");
        std::cout << " 没有存档文件。" << std::endl;
        system("pause");
        return;
    }

    system("cls");

    std::vector<MenuItem> items;
    for (const auto& save : saves) {
        std::string display = "No." + std::to_string(save.id) + " - " + save.name;
        items.emplace_back(display, [save]() {
            // 恢复存档状态
            GameState state;
            state.sceneId = save.sceneId;
            state.commandIndex = save.commandIndex;
            state.flags = save.flags;
            g_JSengine.setState(state);
            system("cls");
            // 从保存点继续运行
            g_JSengine.runScene(save.sceneId, save.commandIndex);
            });
    }
    items.emplace_back(" 返回", []() {});

    Menu saveMenu(" 选择存档", items);
    saveMenu.Run();
}


void GameFunction::Settings() {

    system("cls");
    std::cout << "设置界面..." << std::endl;
    system("pause");
}


void GameFunction::QuitGame() {

    closeBGM();
    std::cout << "\n";
    exit(0);
}


void GameFunction::playBGM(const std::string& filename, bool loop) {
    //先关闭之前的音乐
    stopBGM();

    //构建 MCI 命令字符串
    std::string cmdOpen = "open \"" + filename + "\" type mpegvideo alias bgm";
    std::string cmdPlay = "play bgm" + std::string(loop ? " repeat" : "");

    //执行命令
    mciSendStringA(cmdOpen.c_str(), NULL, 0, NULL);
    mciSendStringA(cmdPlay.c_str(), NULL, 0, NULL);
}

void GameFunction::stopBGM() {
    mciSendStringA("stop bgm", NULL, 0, NULL);
    //注意：不立即 close，以便后续可继续播放
}

void GameFunction::closeBGM() {
    mciSendStringA("close bgm", NULL, 0, NULL);
}
#include "Base.h"

#include "../../TerminalCore/src/Core/TerminalCore.h"
#include "Function/GameFunction.h"
#include "Function/MiniGame.h"

// 获取当前exe文件所在的目录
std::string getExeDirectory() {
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH); // 获取exe完整路径
    std::string fullPath(buffer);
    size_t pos = fullPath.find_last_of("\\/");
    return fullPath.substr(0, pos);
}

int main() {

    SetConsoleOutputCP(CP_UTF8);   // 设置控制台输出编码 UTF-8

    GameFunction GameObj;

    // 注册函数
    g_JSengine.registerFunction("playBaseball", playBaseballGame);
    g_JSengine.registerFunction("playGuessNumber", playGuessNumberGame);
    g_JSengine.registerFunction("playMemory", playMemoryGame);
    g_JSengine.registerFunction("playTyping", playTypingGame);

    // 将exe目录与其他文件夹拼接
    std::string exeDir = getExeDirectory();
    std::string chaptersPath = exeDir + "\\Chapters";
    std::string moveSoundPath = exeDir + "\\Sounds\\Move.wav";
    std::string selectSoundPath = exeDir + "\\Sounds\\Select.wav";
    std::string bgmPath = exeDir + "\\Sounds\\Menu_BGM.mp3";

    // 加载所有剧本文件
    if (!g_JSengine.loadAllChapters(chaptersPath)) {
        std::cerr << "错误:无法加载剧本文件 请确保以下路径存在且包含JSON文件:" << std::endl;
        std::cerr << chaptersPath << std::endl;
        system("pause");
        return 1;
    }

    // 构建主菜单
    std::vector<MenuItem> mainItems = {
        {"开始游戏",[&]() { GameObj.StartGame();}},
        {"加载存档",[&]() { GameObj.loadGameMenu();}},
        {"游戏设置",[&]() { GameObj.Settings();}},
        {"退出游戏",[&]() { GameObj.QuitGame();}}
    };

    // 菜单播放背景音乐
    GameObj.playBGM(bgmPath, true);

    // 创建菜单对象 指定音效文件
    Menu mainMenu("终端物语", mainItems, moveSoundPath, selectSoundPath);
    mainMenu.Run();

}
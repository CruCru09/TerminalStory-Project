#pragma once

#include "Base.h"

#include "Menu.h"
#include "../TsCore/JsonEngine.h"

class GameFunction {
public:
	void StartGame();
	void loadGameMenu();
	void Settings();
	void QuitGame();

	// 播放背景音乐(MP3文件)
	void playBGM(const std::string& filename, bool loop = true);
	// 停止当前背景音乐
	void stopBGM();
	// 关闭背景音乐(释放资源)
	void closeBGM();
};

extern JsonEngine g_JSengine; // 外部访问


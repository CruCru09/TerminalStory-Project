#include "../Base.h"

#pragma comment(lib, "Winmm.lib")

#include "ConsoleUtils.h"

/*
    ConsoleUtils.cpp
    控制台工具函数
*/


// 移动光标到指定位置
void GotoXY(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// 设置控制台颜色(0-15)
void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// 重置颜色为默认值
void resetColor() {
    setColor(7);
}

// 异步播放WAV音效[文件不存在时静默忽略]
void playSoundIfExists(const char* fileName) {
    PlaySoundA(fileName, NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
}
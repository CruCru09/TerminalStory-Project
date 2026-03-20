#include "GameTypeText.h"

// 输出文本[带保存功能]
void typeTextWithSave(const std::string& text, int color, int delayMs, bool waitAtEnd, JsonEngine* JSengine) {
    setColor(color);
    if (delayMs == 0) {
        std::cout << text;
    }
    else {
        for (char c : text) {
            std::cout << c << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
        }
    }
    resetColor();

    if (waitAtEnd) {
        // 清空输入缓冲区 避免之前的多余按键影响
        while (_kbhit()) {
            (void)_getch();
        }

        while (true) {
            int key = _getch();
            if (key == 0 || key == 224) {
                key = _getch();
                if (key == 63 && JSengine) { // F5保存功能
                    JSengine->saveCurrent();
                    system("cls");
                    setColor(color);
                    std::cout << text;
                    resetColor();
                    continue;
                }
                if (key == 66 && JSengine) { // F8变量调试
                    JSengine->debugPrintFlags();
                    system("cls");
                    setColor(color);
                    std::cout << text;
                    resetColor();
                    continue;
                }
            }
            else {
                break;
            }
        }
        std::cout << std::endl;
    }
}
#include "GameTypeText.h"

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
        while (true) {
            int key = _getch();
            if (key == 0 || key == 224) {
                key = _getch();
                if (key == 63 && JSengine) { // F5
                    JSengine->saveCurrent();
                    // 保存后重新显示当前文本(因为可能清屏)
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
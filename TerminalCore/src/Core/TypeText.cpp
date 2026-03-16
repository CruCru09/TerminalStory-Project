#include "TypeText.h"

/*
    打字机效果文本输出函数

    @param text            要显示的文本内容
    @param color        文字颜色
    @param charDelayMs    每个字符之间的延迟毫秒数
    @param waitAtEnd    是否在文本结束后等待玩家按键
*/

/*
    Windows控制台16色前景色(背景黑色)配色建议：

    0=黑色(隐藏)   1=深蓝(冷静)   2=深绿(平和)   3=深青(神秘)
    4=深红(危险)   5=深紫(高贵)   6=深黄(陈旧)   7=浅灰(旁白/NPC)
    8=深灰(系统)   9=亮蓝(聪明)  10=亮绿(活力)  11=亮青(科技)
    12=亮红(傲娇)  13=亮紫(温柔)  14=亮黄(元气)  15=亮白(强调)
*/

void TypeText(const std::string& text, int color = 7, int charDelayMs = 50, bool waitAtEnd = true) {

    HANDLE Console = GetStdHandle(STD_OUTPUT_HANDLE);    // 获取控制台句柄

    // 遍历每个字符
    for (char c : text) {

        //设置颜色
        SetConsoleTextAttribute(Console, color);

        //输出当前字符，并立即刷新缓冲区，确保立刻显示
        std::cout << c << std::flush;

        //等待指定的毫秒数
        std::this_thread::sleep_for(std::chrono::milliseconds(charDelayMs));

    }

    //恢复默认颜色[7 是默认浅灰色]
    SetConsoleTextAttribute(Console, 7);

    if (waitAtEnd) {
        std::cout << std::endl; //换行
        //(void)_getch(); 等待按键[由外层控制]
    }

}
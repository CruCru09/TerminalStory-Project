#pragma once

#include "Base.h"

#include "../../TerminalCore/src/BaseFunction/ConsoleUtils.h"

// 菜单项结构体
struct MenuItem {
    std::string m_Text;
    std::function<void()> m_Action;
    bool m_Enabled;

    MenuItem(const std::string& t, std::function<void()> f, bool e = true);
};

class Menu {
public:
    // 构造函数：标题 选项列表 切换音效路径 确认音效路径
    Menu(const std::string& title,
        const std::vector<MenuItem>& items,
        const std::string& moveSound = "sounds\\move.wav",
        const std::string& selectSound = "sounds\\select.wav");

    // 运行菜单循环
    void Run();

private:
    std::string m_Title;
    std::vector<MenuItem> m_Items;
    size_t m_Selected;          // 当前选中索引
    std::string m_moveSound;
    std::string m_selectSound;

    void draw();                // 绘制菜单
    void moveUp();              // 上移
    void moveDown();            // 下移
};
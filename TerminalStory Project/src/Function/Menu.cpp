#include "Menu.h"


// 菜单项构造函数
MenuItem::MenuItem(const std::string& t, std::function<void()> f, bool e)
    : m_Text(t), m_Action(f), m_Enabled(e) {
}

Menu::Menu(const std::string& title,
    const std::vector<MenuItem>& items,
    const std::string& moveSound,
    const std::string& selectSound)
    : m_Title(title), m_Items(items), m_Selected(0),
    m_moveSound(moveSound), m_selectSound(selectSound) {
}



// 运行菜单
void Menu::Run() {
    bool running = true;
    while (running) {
        draw();

        int key = _getch();
        if (key == 224) { //扩展键(方向键)
            key = _getch();
            switch (key) {
            case 72: // 上
                moveUp();
                playSoundIfExists(m_moveSound.c_str());
                break;
            case 80: // 下
                moveDown();
                playSoundIfExists(m_moveSound.c_str());
                break;
            }
        }

        else if (key == 13) { //回车
            if (m_Items[m_Selected].m_Enabled) {
                playSoundIfExists(m_selectSound.c_str());
                running = false; //先退出菜单循环 再执行动作
                if (m_Items[m_Selected].m_Action) {
                    m_Items[m_Selected].m_Action();
                }
            }
        }
    }
}



// 绘制菜单
void Menu::draw() {
    GotoXY(0, 0); //回到左上角覆盖

    //标题
    setColor(15); //亮白
    std::cout << " ========= " << m_Title << " =========" << std::endl;
    resetColor();

    //选项
    for (size_t i = 0; i < m_Items.size(); ++i) {
        GotoXY(0, 2 + static_cast<int>(i)); // 定位到每行起始

        if (!m_Items[i].m_Enabled) {
            setColor(8); //灰色
            std::cout << "          " << m_Items[i].m_Text;
            resetColor();
            continue;
        }

        if (i == m_Selected) {
            setColor(14); //亮黄
            std::cout << "        → " << m_Items[i].m_Text; //选中项加箭头
            resetColor();
        }
        else {
            std::cout << "          " << m_Items[i].m_Text << "          ";
        }
        std::cout << std::flush;
    }

    //底部提示
    GotoXY(0, 2 + static_cast<int>(m_Items.size()) + 1);
    setColor(7);
    std::cout << " 使用 ↑ ↓ 移动 回车确认" << std::flush;
}



void Menu::moveUp() {
    do {
        if (m_Selected == 0)
            m_Selected = m_Items.size() - 1;
        else
            --m_Selected;
    } while (!m_Items[m_Selected].m_Enabled);
}

void Menu::moveDown() {
    do {
        m_Selected = (m_Selected + 1) % m_Items.size();
    } while (!m_Items[m_Selected].m_Enabled);
}
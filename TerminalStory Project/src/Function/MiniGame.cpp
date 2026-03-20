#include "MiniGame.h"

// ==================== 公共辅助函数 ====================
// 从游戏状态中获取轮数[默认为1]
static int getGameRounds() {
    GameState state = g_JSengine.getCurrentState();
    auto it = state.flags.find("game_rounds");
    if (it != state.flags.end()) {
        return it->second;
    }
    return 1; // 默认1轮
}


// 辅助函数：获取当前好感度
static int getAffection() {
    GameState state = g_JSengine.getCurrentState();
    auto it = state.flags.find("affection");
    return (it != state.flags.end()) ? it->second : 0;
}

// 辅助函数：设置好感度
static void setAffection(int value) {
    GameState state = g_JSengine.getCurrentState();
    state.flags["affection"] = value;
    g_JSengine.setState(state);
}

// 辅助函数：清屏并输出游戏标题
static void showGameTitle(const std::string& title) {
    system("cls");
    setColor(14); // 黄色
    std::cout << "===== " << title << " =====" << std::endl;
    resetColor();
    std::cout << std::endl;
}

// 清空输入缓冲区(解决cin和getch混用的问题)
static void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// ==================== 1. 棒球(简化版) ====================
// 玩法：投手投球，玩家选择挥棒时机。随机数决定结果：本垒打（+3）、安打（+1）、出局（-1）。
void playBaseballGame() {
    int totalRounds = getGameRounds();
    int totalGain = 0;

    for (int round = 1; round <= totalRounds; ++round) {
        showGameTitle("棒球小游戏");
        std::cout << "第 " << round << " 轮 / 共 " << totalRounds << " 轮" << std::endl;
        std::cout << std::endl;

        std::cout << "投手准备投球..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));

        int ballSpeed = rand() % 100;
        std::cout << "球速: " << ballSpeed << " km/h" << std::endl;

        std::cout << "按空格键挥棒，按其他键放弃..." << std::endl;
        char ch = _getch();

        int result = 0;
        if (ch == ' ') {
            int timing = rand() % 100;
            std::cout << "挥棒时机: " << timing << std::endl;

            if (timing > 80) result = 3;
            else if (timing > 40) result = 1;
            else result = -1;
        }
        else {
            std::cout << "你放弃了挥棒，算一次好球。" << std::endl;
            result = 0;
        }

        totalGain += result;

        // 显示本轮结果
        if (result == 3) std::cout << "本垒打！本轮好感度 +3" << std::endl;
        else if (result == 1) std::cout << "安打！本轮好感度 +1" << std::endl;
        else if (result == -1) std::cout << "出局... 本轮好感度 -1" << std::endl;
        else std::cout << "本轮无变化" << std::endl;

        std::cout << "----------------------" << std::endl;
        if (round < totalRounds) {
            std::cout << "按任意键进行下一轮..." << std::endl;
            (void)_getch();
        }
    }

    // 更新好感度
    int currentAffection = getAffection();
    setAffection(currentAffection + totalGain);

    system("cls");
    std::cout << "===== 游戏结束 =====" << std::endl;
    std::cout << "本轮总好感度变化: " << totalGain << std::endl;
    std::cout << "当前好感度: " << getAffection() << std::endl;
    std::cout << "按任意键返回..." << std::endl;
    (void)_getch();
}

// ==================== 2. 猜数字 ====================
void playGuessNumberGame() {
    int totalRounds = getGameRounds();
    int totalGain = 0;

    for (int round = 1; round <= totalRounds; ++round) {
        showGameTitle("猜数字");
        std::cout << "第 " << round << " 轮 / 共 " << totalRounds << " 轮" << std::endl;
        std::cout << std::endl;

        srand((unsigned int)time(nullptr) + round); // 不同轮次不同随机种子
        int secret = rand() % 100 + 1;
        int guess = 0;
        int attempts = 0;

        std::cout << "我已经想好了一个1到100之间的数字。" << std::endl;

        while (true) {
            std::cout << "请输入你的猜测: ";
            clearInputBuffer(); // 清除可能的残留
            std::cin >> guess;
            attempts++;

            if (guess == secret) {
                std::cout << "恭喜你猜对了！用了" << attempts << "次。" << std::endl;
                int gain = (attempts <= 3) ? 5 : (attempts <= 6) ? 3 : 1;
                totalGain += gain;
                std::cout << "本轮好感度 +" << gain << std::endl;
                break;
            }
            else if (guess < secret) {
                std::cout << "小了，再试试。" << std::endl;
            }
            else {
                std::cout << "大了，再试试。" << std::endl;
            }
        }

        if (round < totalRounds) {
            std::cout << "按任意键进行下一轮..." << std::endl;
            (void)_getch();
        }
    }

    setAffection(getAffection() + totalGain);
    system("cls");
    std::cout << "===== 游戏结束 =====" << std::endl;
    std::cout << "本轮总好感度变化: " << totalGain << std::endl;
    std::cout << "当前好感度: " << getAffection() << std::endl;
    std::cout << "按任意键返回..." << std::endl;
   (void)_getch();
}


// ==================== 3. 记忆数字 ====================
void playMemoryGame() {
    int totalRounds = getGameRounds();
    int totalGain = 0;

    for (int round = 1; round <= totalRounds; ++round) {
        showGameTitle("记忆数字");
        std::cout << "第 " << round << " 轮 / 共 " << totalRounds << " 轮" << std::endl;
        std::cout << std::endl;

        const int DIGIT_COUNT = 5;
        std::vector<int> digits(DIGIT_COUNT);
        for (int i = 0; i < DIGIT_COUNT; ++i) {
            digits[i] = rand() % 10;
        }

        std::cout << "记住下面这串数字（5秒后消失）:" << std::endl;
        for (int d : digits) std::cout << d << " ";
        std::cout << std::endl;

        for (int i = 5; i > 0; --i) {
            std::cout << "\r" << i << " 秒后消失..." << std::flush;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        std::cout << "\r                    \r" << std::flush;

        system("cls");
        showGameTitle("记忆数字");
        std::cout << "第 " << round << " 轮 / 共 " << totalRounds << " 轮" << std::endl;
        std::cout << std::endl;
        std::cout << "现在按顺序输入刚才看到的数字（用空格分隔）: " << std::endl;
        clearInputBuffer();

        std::vector<int> answer(DIGIT_COUNT);
        for (int i = 0; i < DIGIT_COUNT; ++i) {
            std::cin >> answer[i];
        }

        bool correct = true;
        for (int i = 0; i < DIGIT_COUNT; ++i) {
            if (answer[i] != digits[i]) {
                correct = false;
                break;
            }
        }

        if (correct) {
            std::cout << "完全正确！本轮好感度 +3" << std::endl;
            totalGain += 3;
        }
        else {
            std::cout << "很遗憾，记错了。答案是: ";
            for (int d : digits) std::cout << d << " ";
            std::cout << std::endl;
        }

        if (round < totalRounds) {
            std::cout << "按任意键进行下一轮..." << std::endl;
            (void)_getch();
        }
    }

    setAffection(getAffection() + totalGain);
    system("cls");
    std::cout << "===== 游戏结束 =====" << std::endl;
    std::cout << "本轮总好感度变化: " << totalGain << std::endl;
    std::cout << "当前好感度: " << getAffection() << std::endl;
    std::cout << "按任意键返回..." << std::endl;
    (void)_getch();
}


// ==================== 4. 打字挑战 ====================
void playTypingGame() {
    int totalRounds = getGameRounds();
    int totalGain = 0;

    for (int round = 1; round <= totalRounds; ++round) {
        showGameTitle("打字挑战");
        std::cout << "第 " << round << " 轮 / 共 " << totalRounds << " 轮" << std::endl;
        std::cout << std::endl;

        std::vector<std::string> words = { "apple", "banana", "cat", "dog", "book", "computer", "game", "hello", "world" };
        int index = rand() % words.size();
        std::string target = words[index];

        std::cout << "请在5秒内输入以下单词:" << std::endl;
        std::cout << ">>> " << target << " <<<" << std::endl;

        auto start = std::chrono::steady_clock::now();
        clearInputBuffer();
        std::string input;
        std::cin >> input;
        auto end = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();

        bool success = (input == target) && (elapsed <= 5);
        if (success) {
            std::cout << "正确！本轮好感度 +2" << std::endl;
            totalGain += 2;
        }
        else {
            if (input != target) {
                std::cout << "输入错误。" << std::endl;
            }
            else {
                std::cout << "超时了[用了" << elapsed << "秒]。" << std::endl;
            }
        }

        if (round < totalRounds) {
            std::cout << "按任意键进行下一轮..." << std::endl;
            (void)_getch();
        }
    }

    setAffection(getAffection() + totalGain);
    system("cls");
    std::cout << "===== 游戏结束 =====" << std::endl;
    std::cout << "本轮总好感度变化: " << totalGain << std::endl;
    std::cout << "当前好感度: " << getAffection() << std::endl;
    std::cout << "按任意键返回..." << std::endl;
    (void)_getch();
}
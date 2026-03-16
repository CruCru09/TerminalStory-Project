#pragma once

#include "Base.h"

enum class CommandType {
    Text,       // 显示文本(可设置颜色、速度、是否等待)
    Option,     // 选项分支(Choices数组，每个包含text和target标签)
    Call,       // 调用C++函数(function字段指定函数名)
    Jump,       // 无条件跳转到指定标签
    SetFlag,    // 设置剧情变量(flag, value)
    End         // 章节结束
};

// 指令基类
struct Command {
    virtual ~Command() = default;
    virtual CommandType type() const = 0;
};

// 文本指令
struct TextCommand : Command {
    CommandType type() const override { return CommandType::Text; }
    std::string text;
    int color = 7;
    int speed = 50;
    bool waitAtEnd = true;
};

// 选项指令
struct OptionCommand : Command {
    CommandType type() const override { return CommandType::Option; }
    struct Choice {
        std::string text;
        std::string targetLabel;
    };
    std::vector<Choice> choices;
};

// 函数调用指令
struct CallCommand : Command {
    CommandType type() const override { return CommandType::Call; }
    std::string funcName;
};

// 跳转指令
struct JumpCommand : Command {
    CommandType type() const override { return CommandType::Jump; }
    std::string targetLabel;
};

// 设置变量指令
struct SetFlagCommand : Command {
    CommandType type() const override { return CommandType::SetFlag; }
    std::string flagName;
    int value = 0;
};

// 章节结束指令
struct EndCommand : Command {
    CommandType type() const override { return CommandType::End; }
};
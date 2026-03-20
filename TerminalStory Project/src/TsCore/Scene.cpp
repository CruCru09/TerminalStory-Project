#include "Scene.h"

bool Scene::loadFromJson(const nlohmann::json& j) {
    if (!j.contains("commands") || !j["commands"].is_array()) return false;
    const auto& cmdArray = j["commands"];

    // 第一遍扫描：收集所有标签及其在原始数组中的位置
    std::map<std::string, size_t> rawLabelPos;
    for (size_t i = 0; i < cmdArray.size(); ++i) {
        const auto& item = cmdArray[i];
        if (item.contains("type") && item["type"] == "label") {
            std::string name = item["name"];
            rawLabelPos[name] = i;
        }
    }

    // 第二遍扫描：构建指令列表 同时将标签位置转换为指令索引
    commands.clear();
    for (size_t i = 0; i < cmdArray.size(); ++i) {
        const auto& item = cmdArray[i];
        std::string type = item["type"];

        if (type == "label") {
            // 标签不生成指令 但需要记录其对应的指令索引(即下一个指令的索引)
            // 由于指令尚未添加 当前 commands.size() 就是该标签后第一个指令的索引
            // 注意：如果标签后面没有指令 这个索引可能超出范围 但跳转到文件末尾的标签通常由end处理
            labelToIndex[item["name"]] = commands.size();
            continue;
        }

        // 根据类型创建指令
        std::unique_ptr<Command> cmd;
        if (type == "text") {
            auto t = std::make_unique<TextCommand>();
            t->text = item["text"];
            if (item.contains("color")) t->color = item["color"];
            if (item.contains("speed")) t->speed = item["speed"];
            if (item.contains("wait")) t->waitAtEnd = item["wait"];
            cmd = std::move(t);
        }

        else if (type == "option") {
            auto cmd = std::make_unique<OptionCommand>();
            for (const auto& ch : item["choices"]) {
                OptionCommand::Choice choice;
                choice.text = ch["text"];
                choice.targetLabel = ch["target"];
                if (ch.contains("scene") && ch["scene"].is_string()) {
                    choice.targetScene = ch["scene"].get<std::string>();
                }
                if (ch.contains("condition") && ch["condition"].is_string()) {
                    choice.condition = ch["condition"].get<std::string>();
                }
                cmd->choices.push_back(choice);
            }
            // 读取指令自身的条件（可选）
            if (item.contains("condition") && item["condition"].is_string()) {
                cmd->condition = item["condition"].get<std::string>();
            }
            commands.push_back(std::move(cmd));
        }

        else if (type == "call") {
            auto c = std::make_unique<CallCommand>();
            c->funcName = item["function"];
            cmd = std::move(c);
        }

        else if (type == "jump") {
            auto cmd = std::make_unique<JumpCommand>();
            cmd->targetLabel = item["target"];
            if (item.contains("scene") && item["scene"].is_string()) {
                cmd->targetScene = item["scene"].get<std::string>();
            }
            // 读取通用条件[如果有]
            if (item.contains("condition") && item["condition"].is_string()) {
                cmd->condition = item["condition"].get<std::string>();
            }
            commands.push_back(std::move(cmd));
        }

        else if (type == "setflag") {
            auto s = std::make_unique<SetFlagCommand>();
            s->flagName = item["flag"];
            s->value = item["value"];
            cmd = std::move(s);
        }

        else if (type == "end") {
            cmd = std::make_unique<EndCommand>();
        }

        else {
            // 未知类型 抛出异常
            throw std::runtime_error("Unknown command type: " + type);
        }

        // 读取指令自身的条件[如果存在且非空]
        if (item.contains("condition") && item["condition"].is_string() && !item["condition"].get<std::string>().empty()) {
            cmd->condition = item["condition"].get<std::string>();
        }

        commands.push_back(std::move(cmd));
    }
    return true;
}
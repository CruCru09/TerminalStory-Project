#include "SavaSystem.h"

namespace fs = std::filesystem;

std::string getSaveDir() { return "Saves/"; }
void ensureSaveDir() { fs::create_directories(getSaveDir()); }

int saveGame(const GameState& state, const std::string& saveName) {
    ensureSaveDir();
    int newId = 1;
    while (fs::exists(getSaveDir() + "Save_" + std::to_string(newId) + ".dat")) {
        ++newId;
    }
    std::string filename = getSaveDir() + "Save_" + std::to_string(newId) + ".dat";
    std::ofstream file(filename, std::ios::binary);
    if (!file) return -1;

    // 写入ID
    file.write(reinterpret_cast<const char*>(&newId), sizeof(newId));
    // 写入名称
    int nameLen = static_cast<int>(saveName.size());
    file.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
    file.write(saveName.c_str(), nameLen);
    // 写入场景ID
    int sceneLen = static_cast<int>(state.sceneId.size());
    file.write(reinterpret_cast<const char*>(&sceneLen), sizeof(sceneLen));
    file.write(state.sceneId.c_str(), sceneLen);
    // 写入指令索引
    file.write(reinterpret_cast<const char*>(&state.commandIndex), sizeof(state.commandIndex));
    // 写入变量数量
    int flagCount = static_cast<int>(state.flags.size());
    file.write(reinterpret_cast<const char*>(&flagCount), sizeof(flagCount));
    for (const auto& [key, val] : state.flags) {
        int keyLen = static_cast<int>(key.size());
        file.write(reinterpret_cast<const char*>(&keyLen), sizeof(keyLen));
        file.write(key.c_str(), keyLen);
        file.write(reinterpret_cast<const char*>(&val), sizeof(val));
    }
    file.close();
    return newId;
}

SaveData loadGame(int id) {
    SaveData data;
    data.id = id;
    std::string filename = getSaveDir() + "Save_" + std::to_string(id) + ".dat";
    std::ifstream file(filename, std::ios::binary);
    if (!file) return data;

    int savedId;
    file.read(reinterpret_cast<char*>(&savedId), sizeof(savedId));
    int nameLen;
    file.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
    data.name.resize(nameLen);
    file.read(&data.name[0], nameLen);
    int sceneLen;
    file.read(reinterpret_cast<char*>(&sceneLen), sizeof(sceneLen));
    data.sceneId.resize(sceneLen);
    file.read(&data.sceneId[0], sceneLen);
    file.read(reinterpret_cast<char*>(&data.commandIndex), sizeof(data.commandIndex));
    int flagCount;
    file.read(reinterpret_cast<char*>(&flagCount), sizeof(flagCount));
    for (int i = 0; i < flagCount; ++i) {
        int keyLen;
        file.read(reinterpret_cast<char*>(&keyLen), sizeof(keyLen));
        std::string key(keyLen, '\0');
        file.read(&key[0], keyLen);
        int val;
        file.read(reinterpret_cast<char*>(&val), sizeof(val));
        data.flags[key] = val;
    }
    return data;
}

// 获取所有存档列表
std::vector<SaveData> listSaves() {
    std::vector<SaveData> saves;
    ensureSaveDir();
    for (const auto& entry : fs::directory_iterator(getSaveDir())) {
        std::string filename = entry.path().filename().string();
        if (filename.rfind("Save_", 0) == 0 && filename.size() > 5 && filename.substr(filename.size() - 4) == ".dat") {
            int id = std::stoi(filename.substr(5, filename.size() - 9));
            SaveData data = loadGame(id);
            if (data.id == id) saves.push_back(data);
        }
    }
    std::sort(saves.begin(), saves.end(), [](const SaveData& a, const SaveData& b) {
        return a.id < b.id;
        });
    return saves;
}
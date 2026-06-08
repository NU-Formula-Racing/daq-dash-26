#include "car_config.hpp"

#include <okay/okay.hpp>

#include <filesystem>
#include <fstream>
#include <vendor/json.hpp>

namespace fs = std::filesystem;
using Json = nlohmann::json;

namespace dash {

CarConfig& CarConfig::get() {
    static CarConfig instance;
    return instance;
}

CarConfig::CarConfig() {
    loadFromFile();
}

void CarConfig::save() {
    saveToFile();
}

void CarConfig::loadFromFile() {
    if (!fs::exists(SAVE_FILE)) {
        saveToFile();
        return;
    }

    std::ifstream file{SAVE_FILE};
    if (!file.is_open()) {
        okay::Engine.logger.error("Failed to open car config file!");
        return;
    }

    try {
        Json data = Json::parse(file);

        maxCurrentRequest = data.value("maxCurrentRequest", maxCurrentRequest);
        launchControlKD = data.value("launchControlKD", launchControlKD);
        launchControlKP = data.value("launchControlKP", launchControlKP);
        launchControlSlipRatio = data.value("launchControlSlipRatio", launchControlSlipRatio);

    } catch (const std::exception& e) {
        okay::Engine.logger.debug("Failed to parse car config: {}", e.what());
    }
}

void CarConfig::saveToFile() {
    Json data;

    data["maxCurrentRequest"] = maxCurrentRequest;
    data["launchControlKD"] = launchControlKD;
    data["launchControlKP"] = launchControlKP;
    data["launchControlSlipRatio"] = launchControlSlipRatio;

    std::ofstream file{SAVE_FILE};
    if (!file.is_open()) {
        okay::Engine.logger.error("Failed to save car configuration to file!");
        return;
    }

    file << data.dump(4);
}

}  // namespace dash

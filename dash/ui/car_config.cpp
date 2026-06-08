#include "car_config.hpp"

#include <okay/okay.hpp>

#include <can/can_dbc.hpp>
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

        maxCurrentRequestRear = data.value("maxCurrentRequestRear", maxCurrentRequestRear);
        maxCurrentRequestRear = data.value("maxCurrentRequestFL", maxCurrentRequestFL);
        maxCurrentRequestRear = data.value("maxCurrentRequestFR", maxCurrentRequestFR);
        launchControlKD = data.value("launchControlKD", launchControlKD);
        launchControlKP = data.value("launchControlKP", launchControlKP);

    } catch (const std::exception& e) {
        okay::Engine.logger.debug("Failed to parse car config: {}", e.what());
    }
}

void CarConfig::saveToFile() {
    Json data;

    data["maxCurrentRequestRear"] = maxCurrentRequestRear;
    data["maxCurrentRequestFL"] = maxCurrentRequestFL;
    data["maxCurrentRequestFR"] = maxCurrentRequestFR;
    data["launchControlKD"] = launchControlKD;
    data["launchControlKP"] = launchControlKP;

    std::ofstream file{SAVE_FILE};
    if (!file.is_open()) {
        okay::Engine.logger.error("Failed to save car configuration to file!");
        return;
    }

    file << data.dump(4);
}

void CarConfig::transmitConfig() {
    const uint8_t validTranmissionValue = 26;  // hehe, like the car

    dbc::dashMaxCurrentRequestRear::maxCurrent->set(maxCurrentRequestRear);
    dbc::dashMaxCurrentRequestRear::useMax->set(validTranmissionValue);

    dbc::dashMaxCurrentRequestFl::maxCurrent->set(maxCurrentRequestFL);
    dbc::dashMaxCurrentRequestFl::useMax->set(validTranmissionValue);

    dbc::dashMaxCurrentRequestFr::maxCurrent->set(maxCurrentRequestFR);
    dbc::dashMaxCurrentRequestFr::useMax->set(validTranmissionValue);

    dbc::dashLaunchControlConfig::lcKp->set(launchControlKP);
    dbc::dashLaunchControlConfig::lcKd->set(launchControlKD);
    dbc::dashLaunchControlConfig::lcEnable->set(enableLaunchControl);
    dbc::dashLaunchControlConfig::useConfigSignature->set(validTranmissionValue);

    // ignore the fact that this is called drive bus
    // this is just an artifact of NFR25 and the autogen code
    dbc::driveBus.send(dbc::dashMaxCurrentRequestRear::message);
    dbc::driveBus.send(dbc::dashMaxCurrentRequestFl::message);
    dbc::driveBus.send(dbc::dashMaxCurrentRequestFr::message);
    dbc::driveBus.send(dbc::dashLaunchControlConfig::message);
}

}  // namespace dash

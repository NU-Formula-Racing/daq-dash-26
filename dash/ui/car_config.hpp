#ifndef __CAR_CONFIG_H__
#define __CAR_CONFIG_H__

#include "json.hpp"

#include <okay/okay.hpp>

#include <cstdint>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;
using Json = nlohmann::json;

#define CAR_CONFIG_FIELDS(X)            \
    X(uint64_t, maxCurrentRequest, 360) \
    X(float, launchControlKD, 0.1f)     \
    X(float, launchControlKP, 0.5f)     \
    X(float, launchControlSlipRatio, 0.5f)

namespace dash {

class CarConfig {
   public:
    static constexpr const char* SAVE_FILE = "car_config.json";

    static CarConfig& get() {
        static CarConfig instance;
        return instance;
    }

#define DECLARE_FIELD(type, name, defaultValue) type name{defaultValue};
    CAR_CONFIG_FIELDS(DECLARE_FIELD)
#undef DECLARE_FIELD

    CarConfig() {
        loadFromFile();
    }

    void save() {
        saveToFile();
    }

   private:
    void loadFromFile() {
        if (!fs::exists(SAVE_FILE)) {
            saveToFile();
            return;
        }

        std::ifstream file{SAVE_FILE};
        if (!file.is_open()) {
            // okay::Engine.logger.error("Failed to open car config file!");
            return;
        }

        try {
            Json data = Json::parse(file);

#define LOAD_FIELD(type, name, defaultValue) name = data.value(#name, name);

            CAR_CONFIG_FIELDS(LOAD_FIELD)

#undef LOAD_FIELD

        } catch (const std::exception& e) {
            // okay::Engine.logger.debug("Failed to parse car config: {}", e.what());
        }
    }

    void saveToFile() {
        Json data;

#define SAVE_FIELD(type, name, defaultValue) data[#name] = name;

        CAR_CONFIG_FIELDS(SAVE_FIELD)

#undef SAVE_FIELD

        std::ofstream file{SAVE_FILE};
        if (!file.is_open()) {
            okay::Engine.logger.error("Failed to save car configuration to file!");
            return;
        }

        file << data.dump(4);
    }
};

}  // namespace dash

#endif  // __CAR_CONFIG_H__

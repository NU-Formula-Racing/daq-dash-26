#pragma once

#include <cstdint>

namespace dash {

class CarConfig {
   public:
    static constexpr const char* SAVE_FILE = "car_config.json";

    static CarConfig& get();

    uint16_t maxCurrentRequest{360};
    uint16_t launchControlKP{8000};
    uint16_t launchControlKD{160};

    void save();

   private:
    CarConfig();

    void loadFromFile();
    void saveToFile();
};

}  // namespace dash

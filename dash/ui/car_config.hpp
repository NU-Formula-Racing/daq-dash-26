#pragma once

#include <cstdint>

namespace dash {

class CarConfig {
   public:
    static constexpr const char* SAVE_FILE = "car_config.json";

    static CarConfig& get();

    uint32_t maxCurrentRequestRear{360};
    uint32_t maxCurrentRequestFL{50};
    uint32_t maxCurrentRequestFR{50};
    uint16_t launchControlKP{8000};
    uint16_t launchControlKD{160};
    bool enableLaunchControl{false};

    void save();
    void transmitConfig();

   private:
    CarConfig();

    void loadFromFile();
    void saveToFile();
};

}  // namespace dash

#pragma once

#include <cstdint>

namespace dash {

class CarConfig {
   public:
    static constexpr const char* SAVE_FILE = "car_config.json";

    static CarConfig& get();

    uint64_t maxCurrentRequest{360};
    float launchControlKD{0.1f};
    float launchControlKP{0.5f};
    float launchControlSlipRatio{0.5f};

    void save();

   private:
    CarConfig();

    void loadFromFile();
    void saveToFile();
};

}  // namespace dash

#ifndef __LIGHTS_HPP__
#define __LIGHTS_HPP__

#include <okay/okay.hpp>

#include <can/can_dbc.hpp>
#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>
#include <platform/interfaces.hpp>
#include <stdint.h>
#include <ui/car_state.hpp>

namespace dash {

struct VirtualizedNeobar {
   public:
    VirtualizedNeobar() = default;

    VirtualizedNeobar(NeopixelStrip* strip, uint8_t numPixels, std::vector<uint8_t> mapping)
        : _strip(strip), _mapping(mapping), _numPixels(numPixels), _dirty(true) {
        for (int i = 0; i < numPixels; i++) {
            _currentColors.push_back(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
        }
    }

    void setColor(uint8_t virtIdx, glm::vec4 color) {
        // get the existing color and see if they are diffent enough
        float ep = 0.001f;
        if (glm::epsilonEqual(_currentColors[virtIdx].x, color.x, ep) &&
            glm::epsilonEqual(_currentColors[virtIdx].y, color.y, ep) &&
            glm::epsilonEqual(_currentColors[virtIdx].z, color.z, ep) &&
            glm::epsilonEqual(_currentColors[virtIdx].w, color.w, ep)) {
            // okay::Engine.logger.debug("Color is the same");
            return;
        }
        _currentColors[virtIdx] = color;
        _dirty = true;
    }

    uint8_t numPixels() const {
        return _numPixels;
    }
    const std::vector<glm::vec4>& currentColors() const {
        return _currentColors;
    }
    uint8_t toHardwareIndex(uint8_t virtIdx) const {
        return _mapping[virtIdx];
    }
    NeopixelStrip* strip() const {
        return _strip;
    }
    bool isDirty() const {
        return _dirty;
    }
    void clearDirty() {
        _dirty = false;
    }

   private:
    std::vector<uint8_t> _mapping;  // idx -> hwIdx
    std::vector<glm::vec4> _currentColors;
    NeopixelStrip* _strip;
    uint8_t _numPixels;
    bool _dirty{true};
};

class NeopixelManager : public okay::System<okay::SystemScope::GAME> {
   public:
    void initialize() {
        // create the strips
        _strips[0].init(19, 16);  // left
        _strips[1].init(13, 7);   // top
        _strips[2].init(18, 16);  // right

        // create the bars
        for (int i = 0; i < 5; i++) {
            _bars[i] = VirtualizedNeobar(
                &_strips[getHWIndexForBar(i)], numPixelsForBar(i), mappingAtBar(i));
        }

        // set all the bars to black
        glm::vec4 black = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < _bars[i].numPixels(); j++) {
                _bars[i].setColor(j, black);
            }
        }

        dbc::ecuDriveStatus::message.attach_rx_callback([this]() {
            onECUDriveStatus();
        });
        dbc::telemetryOdometer::message.attach_rx_callback([this]() {
            initializeOdometer();
        });

        startAnimation([this](float time) {
            idle(time);
        });
        updateDisplay();
    }

    void tick() {
        if (_animationFunction != nullptr) {
            float time = okay::Engine.time->timeSinceStartMs() - _animationStartTimeMs;
            _animationFunction(time);
        }
        checkErrorOccured();
        updateDisplay();
    }

    void shutdown() {
        // make all the colors black
        glm::vec4 black = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < _bars[i].numPixels(); j++) {
                _bars[i].setColor(j, black);
            }
        }

        updateDisplay();

        // now cleanup
        for (int i = 0; i < 3; i++) {
            _strips[i].cleanup();
        }
    }

    void updateDisplay() {
        for (int i = 0; i < 3; i++) {
            NeopixelStrip& strip = _strips[i];

            for (int j = 0; j < 5; j++) {
                if (i != getHWIndexForBar(j))
                    continue;

                for (int k = 0; k < _bars[j].numPixels(); k++) {
                    strip.setColor(_bars[j].toHardwareIndex(k), _bars[j].currentColors()[k]);
                }

                _bars[j].clearDirty();
            }

            strip.show();
        }
    }

    VirtualizedNeobar& getBar(uint8_t barNum) {
        return _bars[barNum];
    }

    void initializeOdometer() {
        if (odometerInitialized)
            return;
        odometerInitialized = true;
        startOdometer = dbc::telemetryOdometer::milesDriven->get();
    }

    void error(float time) {
        const float hardFaultPeriod = 250.0f;
        const float imdPeriod = 1000.0f;
        const int errorCode = 0x03;

        bool imdError = !(dbc::bmsStatus::imdState->get());

        const float period = (imdError) ? imdPeriod : hardFaultPeriod;
        float brightness = static_cast<int>(floor(time / period)) % 2;

        glm::vec4 color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        color *= brightness;
        // set the colors
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < getBar(i).numPixels(); j++) {
                getBar(i).setColor(j, color);
            }
        }
    }

    void checkErrorOccured() {
        // error  check if error state is not no error & error state is the same, then return
        bool errorValue = CarState::hardFaultPresent();
        if (errorValue) {
            if (!currentErrorState) {
                currentErrorState = true;
                startAnimation([this](float time) {
                    error(time);
                });
            }
            return;
        }

        if (currentErrorState) {
            errorReset = true;
        }

        currentErrorState = false;
        onECUDriveStatus();
    }

    void onECUDriveStatus() {
        if (currentErrorState) {
            return;
        }

        uint8_t state = dbc::ecuDriveStatus::driveState->get();

        if (errorReset == false && state == currentState)
            return;

        errorReset = false;

        currentState = state;

        switch (state) {
            case 0:  // idle
                startAnimation([this](float time) {
                    idle(time);
                });
                break;
            case 1:  // precharge
                startAnimation([this](float time) {
                    precharge(time);
                });
                break;
            case 2:  // neutral
                startAnimation([this](float time) {
                    neutral(time);
                });
                break;
            case 3:  // drive
                // do something
                startAnimation([this](float time) {
                    drive(time);
                });
                break;
        }
    }

   private:
    std::array<VirtualizedNeobar, 5> _bars;
    std::array<NeopixelStrip, 3> _strips;
    uint32_t _animationStartTimeMs{0};
    std::function<void(float)> _animationFunction;
    uint8_t currentState{0};
    bool currentErrorState{false};
    bool errorReset{false};
    float startOdometer;
    bool odometerInitialized{false};

    void startAnimation(std::function<void(float)> animationFunction) {
        _animationStartTimeMs = okay::Engine.time->timeSinceStartMs();
        _animationFunction = animationFunction;
    }

    uint8_t numPixelsForBar(uint8_t bar) {
        if (bar == 2) {
            return 7;
        } else {
            return 8;
        }
    }

    uint8_t getHWIndexForBar(uint8_t bar) {
        switch (bar) {
            case 0:
            case 1:
                return 0;
            case 2:
                return 1;
            case 3:
            case 4:
                return 2;
        }
        okay::Engine.logger.error("Invalid bar index {}", bar);
        return 0;
    }

    // master map
    std::vector<uint8_t> mappingAtBar(uint8_t bar) {
        static const std::vector<int> MASTER_MAP = {
            15,
            14,
            13,
            12,
            11,
            10,
            9,
            8,  // bar 0
            7,
            6,
            5,
            4,
            3,
            2,
            1,
            0,  // bar 1
            0,
            1,
            2,
            3,
            4,
            5,
            6,  // bar 2
            7,
            6,
            5,
            4,
            3,
            2,
            1,
            0,  // bar 3
            15,
            14,
            13,
            12,
            11,
            10,
            9,
            8,  // bar 4
        };

        std::vector<uint8_t> barMap;
        uint8_t num = numPixelsForBar(bar);

        // find starting point
        int offset = 0;
        for (int i = 0; i < bar; i++) {
            offset += numPixelsForBar(i);
        }

        for (int i = 0; i < num; i++)  // create the actual bars
        {
            int hwIdx = MASTER_MAP[offset + i];
            barMap.push_back((uint8_t)hwIdx);  // some funny mapping code
        }

        return barMap;
    }

    // ANIMATIONS

    static constexpr glm::vec4 colorFromHex(uint32_t hex) {
        float r = ((hex >> 16) & 0xFF) / 255.0f;
        float g = ((hex >> 8) & 0xFF) / 255.0f;
        float b = (hex & 0xFF) / 255.0f;
        return glm::vec4(r, g, b, 1.0f);
    }

    void bmsSocAnimation(float time) {
        float bmsSoc = dbc::bmsStatus::soc->get();

        glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

        if (bmsSoc > 0.50) {
            color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
        } else if (bmsSoc > 0.30) {
            color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
        } else {
            color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        }

        float batteryPercentage = static_cast<float>(dbc::bmsStatus::soc->get());

        if (batteryPercentage > 1.0) {
            batteryPercentage = 1.0;
        }

        int bar2NumPixels = getBar(2).numPixels();

        int numFull = static_cast<int>(floor(batteryPercentage * bar2NumPixels));

        // full bars
        for (int j = bar2NumPixels - 1; j > (bar2NumPixels - 1 - numFull); j--) {
            getBar(2).setColor(j, color);
        }

        // partial
        if (numFull != bar2NumPixels) {
            glm::vec4 partialColor = color * (batteryPercentage * getBar(2).numPixels() - numFull);
            getBar(2).setColor(bar2NumPixels - numFull - 1, partialColor);
        }
    }

    void odometerAnimation(float time) {  // bar 0
        glm::vec4 nuPurple = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
        float odmVal = static_cast<float>(dbc::telemetryOdometer::milesDriven->get());

        float odometerPercentage =
            (odmVal - (odometerInitialized ? startOdometer : odmVal)) * 1.60934 / 22;

        if (odometerPercentage > 1.0) {
            odometerPercentage = 1.0;
        }

        int numFull = static_cast<int>(floor(odometerPercentage * getBar(0).numPixels()));

        // full bars
        for (int j = 0; j < numFull; j++) {
            getBar(0).setColor(j, nuPurple);
        }

        // partial
        for (int j = 0; j < getBar(0).numPixels(); j++) {
            if (j == numFull) {
                glm::vec4 partialColor =
                    nuPurple * (odometerPercentage * getBar(0).numPixels() - numFull);
                getBar(0).setColor(j, partialColor);
            }
        }
    }

    void socChargeAnimation(float time) {                           // bar 1
        /* glm::vec4 color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f); */  // placeholder

        glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

        float lowerBound = 0.1;
        float upperBound = 0.9;

        float bmsSoc = static_cast<float>(dbc::bmsStatus::soc->get());
        float batteryPercentage = (1.0 - bmsSoc - lowerBound) / (upperBound - lowerBound);

        if (bmsSoc > 0.50) {
            color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
        } else if (bmsSoc > 0.30) {
            color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
        } else {
            color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        }

        if (batteryPercentage > 1.0) {
            batteryPercentage = 1.0;
        }

        int numFull = static_cast<int>(floor(batteryPercentage * getBar(1).numPixels()));

        // full bars
        for (int j = 0; j < numFull; j++) {
            getBar(1).setColor(j, color);
        }

        // partial
        for (int j = 0; j < getBar(1).numPixels(); j++) {
            if (j == numFull) {
                glm::vec4 partialColor =
                    color * (batteryPercentage * getBar(1).numPixels() - numFull);
                getBar(1).setColor(j, partialColor);
            }
        }
    }

    void igbtTempAnimation(float time) {                      // bar 3
        glm::vec4 color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);  // placeholder
        float lowerBound = 0.0;
        float upperBound = 120.0;

        float tempPercentage =
            (static_cast<float>(dbc::rearInverterTempStatus::igbtTemp->get()) - lowerBound) /
            (upperBound - lowerBound);

        if (tempPercentage > 1.0) {
            tempPercentage = 1.0;
        }

        int numFull = static_cast<int>(floor(tempPercentage * getBar(3).numPixels()));

        // full pixels
        for (int j = 0; j < numFull; j++) {
            getBar(3).setColor(j, color);
        }

        // partial
        for (int j = 0; j < getBar(3).numPixels(); j++) {
            if (j == numFull) {
                glm::vec4 partialColor = color * (tempPercentage * getBar(3).numPixels() - numFull);
                getBar(3).setColor(j, partialColor);
            }
        }
    }

    void batteryTempAnimation(float time) {                   // bar 4
        glm::vec4 color = glm::vec4(0.1f, 0.1f, 1.0f, 1.0f);  // placeholder
        float lowerBound = 20.0;
        float upperBound = 60.0;

        float tempPercentage =
            (static_cast<float>(dbc::bmsDaughterboard::batteryTemperature->get()) - lowerBound) /
            (upperBound - lowerBound);

        if (tempPercentage > 1.0) {
            tempPercentage = 1.0;
        }

        int numFull = static_cast<int>(floor(tempPercentage * getBar(4).numPixels()));

        // full pixels
        for (int j = 0; j < numFull; j++) {
            getBar(4).setColor(j, color);
        }

        // partial
        for (int j = 0; j < getBar(4).numPixels(); j++) {
            if (j == numFull) {
                glm::vec4 partialColor = color * (tempPercentage * getBar(4).numPixels() - numFull);
                getBar(4).setColor(j, partialColor);
            }
        }
    }

    void idle(float time) {
        const float breathePeriod = 2000.0f;
        float brightness = (std::sin(time / breathePeriod) + 1.0f) / 2.0f;  // +1 for normalize
        glm::vec4 purple = colorFromHex(0x4E2A84) * abs(brightness);
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < getBar(i).numPixels(); j++) {
                getBar(i).setColor(j, purple);
            }
        }

        bmsSocAnimation(0);
    }

    void neutral(float time) {
        static std::vector<glm::vec4> palette = {colorFromHex(0x4E2A84),
            colorFromHex(0x4E2A84),
            colorFromHex(0x00000),
            colorFromHex(0x00000)};
        const float moveSpeed = 1 / 200.0f;

        for (int i = 0; i < 5; i++) {  // for all 5 bars
            if (i == 2)
                continue;
            int barOffset = i * 2;
            for (int j = 0; j < getBar(i).numPixels(); j++) {  // this indexes the leds on each bar
                int colorIndex = static_cast<int>(time * moveSpeed + j + barOffset);
                glm::vec4 color = palette[colorIndex % palette.size()];
                int invert = getBar(i).numPixels() - j - 1;
                getBar(i).setColor(invert, color);
            }
        }

        bmsSocAnimation(0);
    }

    void precharge(float time) {
        glm::vec4 yellow = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
        glm::vec4 black = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

        float prechargePercentage =
            static_cast<float>(dbc::rearInverterMotorStatus::dcVoltage->get()) /
            static_cast<float>(dbc::bmsDaughterboard::batteryVoltage->get());
        for (int i = 0; i < 5; i++) {
            // probably something here
            float t = 0.9f / getBar(i).numPixels();
            for (int j = 0; j < getBar(i).numPixels(); j++) {
                if (prechargePercentage >= t * j) {
                    getBar(i).setColor(j, yellow);
                } else {
                    getBar(i).setColor(j, black);
                }
            }
        }

        bmsSocAnimation(0);
    }

    void drive(float time) {
        const float blinkTime = 500;
        const int numBlinks = 3;

        bool bppc = dbc::ecuImplausibility::bppcImp->get();

        if (time < blinkTime * 2 * numBlinks) {
            // we are still blinking
            float brightness = static_cast<int>(floor(time / blinkTime)) % 2;
            glm::vec4 color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
            color *= brightness;
            // set the colors
            for (int i = 0; i < 5; i++) {
                if (i == 2)
                    continue;
                for (int j = 0; j < getBar(i).numPixels(); j++) {
                    getBar(i).setColor(j, color);
                }
            }
        } else {
            // bppc error check
            if (bppc) {
                glm::vec4 yellow = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f);
                for (int i = 0; i < 5; i++) {
                    if (i == 2)
                        continue;
                    for (int j = 0; j < getBar(i).numPixels(); j++) {
                        getBar(i).setColor(j, yellow);
                    }
                }
                return;
            }

            glm::vec4 black = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

            for (int i = 0; i < 5; i++) {
                // probably something here
                for (int j = 0; j < getBar(i).numPixels(); j++) {
                    getBar(i).setColor(j, black);
                }
            }

            odometerAnimation(0);
            socChargeAnimation(0);
            igbtTempAnimation(0);
            batteryTempAnimation(0);

            /*

            // we are now in throttle light mode
            const float currentMax = 235;
            int32_t rawCurrent = dbc::ecuSetCurrentRearInverter::setCurrentRearInverter->get();
            int32_t rawThrottle = rawCurrent > currentMax ? currentMax : rawCurrent;
            float throttlePercentage = static_cast<float>(rawThrottle) / currentMax;
            glm::vec4 botColor = colorFromHex(0x00FF00);
            glm::vec4 topColor = colorFromHex(0xFFDD00);

            for (int i = 0; i < 5; i++) {
                if (i == 2)
                    continue;
                int numPixels = getBar(i).numPixels();
                int numFull = static_cast<int>(floor(throttlePercentage * numPixels));

                for (int j = 0; j < numFull; j++) {
                    glm::vec4 color = glm::mix(
                        botColor, topColor, static_cast<float>(j) / static_cast<float>(numPixels));
                    getBar(i).setColor(j, color);
                }
            }

            // we are now in throttle light mode

            for (int i = 0; i < 5; i++) {
                int numPixels = getBar(i).numPixels();
                int numFull = static_cast<int>(floor(throttlePercentage * numPixels));

                for (int j = 0; j < numFull; j++) {
                    glm::vec4 color = glm::mix(
                        botColor, topColor, static_cast<float>(j) / static_cast<float>(numPixels));
                    getBar(i).setColor(j, color);
                }

                // turn off the rest of the pixels
                for (int j = numFull; j < numPixels; j++) {
                    if (j == numFull) {
                        glm::vec4 color = glm::mix(botColor,
                            topColor,
                            static_cast<float>(j) / static_cast<float>(numPixels));
                        // set it to partial brightness to make a smoother transition
                        glm::vec4 partialColor = color * (throttlePercentage * numPixels - numFull);
                        getBar(i).setColor(j, partialColor);
                        continue;
                    }

                    getBar(i).setColor(j, glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
                }
            }

            */
        }
    }
};

}  // namespace dash

#endif

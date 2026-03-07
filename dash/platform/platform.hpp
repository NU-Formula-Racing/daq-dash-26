#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include <nfr_can/IClock.hpp>
#include <nfr_can/IGpio.hpp>
#include <nfr_can/ISpi.hpp>

#include <glm/glm.hpp>

#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <thread>
#include <iostream>
#include <functional>
#include <unordered_map>
#include <vector>

namespace dash::platform {

class GPIO : public IGpio {
public:
  enum class EdgeType {
    RISING,
    FALLING,
    BOTH
  };

  GPIO(uint8_t pin, bool output);
  ~GPIO();

  bool gpio_write(GpioLevel level) override;
  bool gpio_read(GpioLevel& out) override;

  void attachInterrupt(std::function<void()> callback, EdgeType edge);

  bool checkError();

private:
  struct GPIOImpl;
  std::unique_ptr<GPIOImpl> _impl;
};

class MockGPIO : public IGpio {
  bool gpio_write(GpioLevel level) override {return true; }
  bool gpio_read(GpioLevel& out) override { return true; }
};

class SPI : public ISpi {
   public:
    SPI(const std::string& device = "/dev/spidev0.0",
        uint32_t speedHz = 1'000'000,
        uint8_t mode = 0,
        uint8_t bitsPerWord = 8);
    ~SPI();

    bool ISpi_transfer(const uint8_t* tx, uint8_t* rx, size_t len) override;
    bool ISpi_write(const uint8_t* tx, size_t len) override;

   private:
    struct SPIImpl;
    std::unique_ptr<SPIImpl> _impl;
};

class Clock : public IClock {
   public:
    void sleepMs(uint32_t ms) override {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }

    uint32_t monotonicMs() override {
        using namespace std::chrono;
        return static_cast<uint32_t>(
            duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count());
    }
};

class NeopixelStrip {
   public:
    NeopixelStrip();
    ~NeopixelStrip();

    void init(const int& pin, const int& numLeds);
    void setColor(const int& ledIndex, const glm::vec4& color);
    void show();
    void cleanup();

   private:
    struct NeopixelImpl;
    std::unique_ptr<NeopixelImpl> _impl;
};

class Button {
public:
  enum class ButtonState {
    DOWN_THIS_FRAME,
    DOWN,
    UP_THIS_FRAME,
    UP
  };

  Button(uint8_t gpioPin);
  ~Button();

  void onDown(std::function<void()> callback);
  void onUp(std::function<void()> callback);

  bool isDownThisFrame();
  bool isUpThisFrame();
  bool isDown();

private:
  uint8_t _buttonID;
  std::unique_ptr<GPIO> _gpio;
};

// figure out how to implement this
class Encoder {
public:
  enum class EncoderState {
    LEFT_THIS_FRAME,
    RIGHT_THIS_FRAME,
    IDLE
  };

  Encoder(uint8_t leftPin, uint8_t rightPin);
  ~Encoder();

  void onRight(std::function<void()> callback);
  void onLeft(std::function<void()> callback);

  bool isIdle();
  bool isRightThisFrame();
  bool isLeftThisFrame();

private:
  constexpr uint16_t generateID(uint8_t left, uint8_t right);
  uint16_t _encoderID;
  std::unique_ptr<GPIO> _leftGPIO;
  std::unique_ptr<GPIO> _rightGPIO;
};

class InputManager {
public:
  static InputManager& instance();

  void registerButton(uint8_t buttonID);
  void unregisterButton(uint8_t buttonID);

  void attachDownCallback(uint8_t buttonID, std::function<void()> callback);
  void attachUpCallback(uint8_t buttonID, std::function<void()> callback);
  
  void executeDownCallbacks(uint8_t buttonID);
  void executeUpCallbacks(uint8_t buttonID);

  void registerEncoder(uint16_t encoderID, uint8_t leftPin, uint8_t rightPin);
  void unregisterEncoder(uint16_t encoderID);

  void attachLeftCallback(uint16_t encoderID, std::function<void()> callback);
  void attachRightCallback(uint16_t encoderID, std::function<void()> callback);
  
  void executeLeftCallbacks(uint16_t encoderID);
  void executeRightCallbacks(uint16_t encoderID);

  void onEncoderEdge(uint16_t encoderID);

  // add left and right callbacks, which take the left or right pin number

  bool isDownThisFrame(uint8_t buttonID);
  bool isUpThisFrame(uint8_t buttonID);
  bool isDown(uint8_t buttonID);

  bool isRightThisFrame(uint16_t encoderID) const;
  bool isLeftThisFrame(uint16_t encoderID) const;
  bool isIdle(uint16_t encoderID) const;

  void tick();

private:
  InputManager();
  InputManager(const InputManager&) = delete;
  InputManager& operator=(const InputManager&) = delete;

  std::unordered_map<uint8_t, bool> _buttonEventHappened;
  std::unordered_map<uint8_t, std::vector<std::function<void()>>> _downCallbacks;
  std::unordered_map<uint8_t, std::vector<std::function<void()>>> _upCallbacks;
  std::unordered_map<uint8_t, Button::ButtonState> _buttonStates;

  struct EncoderRuntime {
    uint8_t pinA = 0;
    uint8_t pinB = 0;
    uint8_t prevAB = 0;
    int8_t accum = 0;
    bool initialized = false;
    Encoder::EncoderState state = Encoder::EncoderState::IDLE;
  };

  std::unordered_map<uint16_t, std::vector<std::function<void()>>> _leftCallbacks;
  std::unordered_map<uint16_t, std::vector<std::function<void()>>> _rightCallbacks;
  std::unordered_map<uint16_t, EncoderRuntime> _encoderStates;
};

void tick();

} // namespace dash::platform

#endif  // __PLATFORM_H__

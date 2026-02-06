#include <platform/platform.hpp>

#include <gpiod.h>

#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <string>
#include <utility>

namespace dash::platform {

static std::string chipNameToPath(const std::string& chipName) {
  // Accept:
  //   "gpiochip0"  -> "/dev/gpiochip0"
  //   "/dev/gpiochip0" (unchanged)
  //   "0"          -> "/dev/gpiochip0"
  if (chipName.rfind("/dev/gpiochip", 0) == 0) {
    return chipName;
  }
  if (chipName.rfind("gpiochip", 0) == 0) {
    return "/dev/" + chipName;
  }
  return "/dev/gpiochip" + chipName;
}

struct GPIO::GPIOImpl {
  gpiod_chip* _chip = nullptr;
  gpiod_line_request* _request = nullptr;

  unsigned _offset = 0;
  bool _isOutput = false;

  GPIOImpl(const std::string& chipName, unsigned lineOffset, bool output)
      : _offset(lineOffset), _isOutput(output) {
    const std::string chipPath = chipNameToPath(chipName);

    _chip = gpiod_chip_open(chipPath.c_str());
    if (!_chip) {
      throw std::runtime_error("gpiod_chip_open failed (" + chipPath + "): " +
                               std::string(std::strerror(errno)));
    }

    gpiod_line_settings* settings = gpiod_line_settings_new();
    gpiod_line_config* lineConfig = gpiod_line_config_new();
    gpiod_request_config* reqConfig = gpiod_request_config_new();

    if (!settings || !lineConfig || !reqConfig) {
      if (reqConfig) gpiod_request_config_free(reqConfig);
      if (lineConfig) gpiod_line_config_free(lineConfig);
      if (settings) gpiod_line_settings_free(settings);
      gpiod_chip_close(_chip);
      _chip = nullptr;
      throw std::runtime_error("libgpiod: failed to allocate config objects");
    }

    gpiod_request_config_set_consumer(reqConfig, "dash-platform");

    if (_isOutput) {
      gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_OUTPUT);
      gpiod_line_settings_set_output_value(settings, GPIOD_LINE_VALUE_INACTIVE);
    } else {
      gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_INPUT);
    }

    const unsigned offsets[1] = {_offset};
    if (gpiod_line_config_add_line_settings(lineConfig, offsets, 1, settings) < 0) {
      gpiod_request_config_free(reqConfig);
      gpiod_line_config_free(lineConfig);
      gpiod_line_settings_free(settings);
      gpiod_chip_close(_chip);
      _chip = nullptr;
      throw std::runtime_error("gpiod_line_config_add_line_settings failed: " +
                               std::string(std::strerror(errno)));
    }

    _request = gpiod_chip_request_lines(_chip, reqConfig, lineConfig);

    gpiod_request_config_free(reqConfig);
    gpiod_line_config_free(lineConfig);
    gpiod_line_settings_free(settings);

    if (!_request) {
      gpiod_chip_close(_chip);
      _chip = nullptr;
      throw std::runtime_error("gpiod_chip_request_lines failed: " +
                               std::string(std::strerror(errno)));
    }
  }

  ~GPIOImpl() {
    if (_request) {
      gpiod_line_request_release(_request);
      _request = nullptr;
    }
    if (_chip) {
      gpiod_chip_close(_chip);
      _chip = nullptr;
    }
  }

  void write(GpioLevel level) {
    if (!_isOutput) {
      throw std::runtime_error("Attempted GPIO write on input line");
    }

    const gpiod_line_value v =
        (level == GpioLevel::G_HIGH) ? GPIOD_LINE_VALUE_ACTIVE : GPIOD_LINE_VALUE_INACTIVE;

    if (gpiod_line_request_set_value(_request, _offset, v) < 0) {
      throw std::runtime_error("gpiod_line_request_set_value failed: " +
                               std::string(std::strerror(errno)));
    }
  }

  GpioLevel read() {
    const gpiod_line_value v = gpiod_line_request_get_value(_request, _offset);

    if (v == GPIOD_LINE_VALUE_ERROR) {
      throw std::runtime_error("gpiod_line_request_get_value failed: " +
                               std::string(std::strerror(errno)));
    }

    return (v == GPIOD_LINE_VALUE_ACTIVE) ? GpioLevel::G_HIGH : GpioLevel::G_LOW;
  }
};

GPIO::GPIO(const std::string& chipName, unsigned lineOffset, bool output)
    : _impl(std::make_unique<GPIOImpl>(chipName, lineOffset, output)) {}

GPIO::~GPIO() = default;

void GPIO::gpio_write(GpioLevel level) {
  _impl->write(level);
}

GpioLevel GPIO::gpio_read() {
  return _impl->read();
}

} // namespace dash::platform

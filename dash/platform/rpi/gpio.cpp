#include <platform/platform.hpp>

#include <gpiod.h>

#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <string>
#include <utility>
#include <iostream>
#include <thread>
#include <atomic>
#include <map>
#include <vector>


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

  GPIOError errs;

  GPIOImpl(const std::string& chipName, unsigned lineOffset, bool output)
      : _offset(lineOffset), _isOutput(output) {
    const std::string chipPath = chipNameToPath(chipName);

    std::cout << "GPIOImpl::ctor()\n";
    _chip = gpiod_chip_open(chipPath.c_str());
    if (!_chip) {
      errs.chip_open_err = true;
      return;
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
      errs.config_alloc_err = true;
      return;
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
      errs.line_config_add_line_settings_err = true;
      return;
    }

    _request = gpiod_chip_request_lines(_chip, reqConfig, lineConfig);

    gpiod_request_config_free(reqConfig);
    gpiod_line_config_free(lineConfig);
    gpiod_line_settings_free(settings);

    if (!_request) {
      gpiod_chip_close(_chip);
      _chip = nullptr;
      errs.gpiod_chip_request_lines_err = true;
      return;
    }
  }

  GPIOImpl(const std::string &chipName, unsigned lineOffset, std::function<void()> callback, IEdgeEventMonitor& monitor){
    monitor.register
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

  bool write(GpioLevel level) {
    if (!_isOutput) {
      std::cerr << "attempt to write to input GPIO\n";
      return false;
    }

    const gpiod_line_value v =
        (level == GpioLevel::G_HIGH) ? GPIOD_LINE_VALUE_ACTIVE : GPIOD_LINE_VALUE_INACTIVE;

    if (gpiod_line_request_set_value(_request, _offset, v) < 0) {
      return false;
    }

    return true;
  }

  bool read(GpioLevel& out) {
    const gpiod_line_value v = gpiod_line_request_get_value(_request, _offset);

    if (v == GPIOD_LINE_VALUE_ERROR) {
      return false;
    }

    out = (v == GPIOD_LINE_VALUE_ACTIVE) ? GpioLevel::G_HIGH : GpioLevel::G_LOW;
    return true;
  }

  bool checkStatus() {
    return errs.checkError();
  }
  
};

GPIO::GPIO(const std::string& chipName, unsigned lineOffset, bool output)
    : _impl(std::make_unique<GPIOImpl>(chipName, lineOffset, output)) {}

GPIO::GPIO(const std::string &chipName, unsigned lineOffset, std::function<void()> callback, IEdgeEventMonitor& monitor)
    : _impl(std::make_unique<GPIOImpl>(const std::string &chipName, unsigned lineOffset, std::function<void()> callback, IEdgeEventMonitor& monitor)) {}

GPIO::~GPIO() = default;

bool GPIO::gpio_write(GpioLevel level) {
  return _impl->write(level);
}

bool GPIO::gpio_read(GpioLevel& out) {
  return _impl->read(out);
}

bool GPIO::checkError() {
  return _impl->checkStatus();
}





} // namespace dash::platform

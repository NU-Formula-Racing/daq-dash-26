#include <platform/interfaces.hpp>

namespace dash {

struct SPI::SPIImpl {
    // noop
};

SPI::SPI(const std::string&, uint32_t, uint8_t, uint8_t) : _impl(std::make_unique<SPIImpl>()) {
}

SPI::~SPI() = default;

bool SPI::ISpi_write(const uint8_t*, size_t) {
    return false;
}

bool SPI::ISpi_transfer(const uint8_t* tx, uint8_t* rx, size_t len) {
    return false;
}

};  // namespace dash
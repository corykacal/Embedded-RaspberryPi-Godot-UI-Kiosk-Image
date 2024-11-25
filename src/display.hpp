#pragma once
#include <gpiod.hpp>
#include <linux/spi/spidev.h>

class Display {
private:
    static constexpr int DC_PIN = 25;
    gpiod::chip chip{"gpiochip0"};
    gpiod::line dc_line;
    int spi_fd;

public:
    Display() : dc_line(chip.get_line(DC_PIN)) {
        dc_line.request({"dendrophone", gpiod::line_request::DIRECTION_OUTPUT});
        spi_fd = open("/dev/spidev0.0", O_RDWR);
    }

    void clear(uint16_t color = 0xFFFF) {
        dc_line.set_value(1);
        uint8_t high = color >> 8;
        uint8_t low = color & 0xFF;
        write(spi_fd, &high, 1);
        write(spi_fd, &low, 1);
    }
};

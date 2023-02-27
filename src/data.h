#ifndef DATA_H
#define DATA_H

#include <cstdint>

typedef struct {
    uint8_t instruction;
    uint8_t reserved;
    uint8_t mode;
    uint8_t speed;
    uint8_t brightness;
    uint8_t color;
    uint8_t offset;
    uint8_t checksum;
} packet;

enum instructions {
    RGB_MODE = 0x08,
    RGB_PROGRAM = 0x12
};

enum mode {
    STATIC = 0x01,
    BREATHING = 0x02,
    WAVE = 0x03,
    FADE = 0x04,
    MARQUEE = 0x05,
    RIPPLE = 0x06,
    FLASH = 0x07,
    NEON = 0x08,
    RAINBOW = 0x09,
    RAINDROP = 0x0A,
    CIRCLE = 0x0B,
    HEDGE = 0x0C,
    ROTATE = 0x0D,
    CUSTOM_ONE = 0x33,
    CUSTOM_TWO = 0x34,
    CUSTOM_THREE = 0x35,
    CUSTOM_FOUR = 0x36,
    CUSTOM_FIVE = 0x37
};

enum color {
    RED = 0x01,
    GREEN = 0x02,
    YELLOW = 0x03,
    BLUE = 0x04,
    ORANGE = 0x05,
    PURPLE = 0x06,
    WHITE = 0x07,
    RANDOM = 0x08
};

enum vendors {
    CHU_YUEN = 0x1044
};

#endif // DATA_H

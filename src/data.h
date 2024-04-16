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

void clear_packet(packet *packet) {
    packet->instruction = 0x00;
    packet->reserved = 0x00;
    packet->mode = 0x00;
    packet->speed = 0x00;
    packet->brightness = 0x00;
    packet->color = 0x00;
    packet->offset = 0x00;
    packet->checksum = 0x00;
}

enum instructions {
    RGB_MODE = 0x08,
    RGB_PROGRAM = 0x12,
    RGB_GETREPORT = 0x88,
    RGB_UNKNOWN = 0x8D,
    RGB_READCONFIG = 0x92
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

// Copied from https://github.com/martin31821/fusion-kbd-controller/blob/master/data.c
uint8_t m_white_data[512] = {
  0x00, 0x00, 0x00, 0x00, // no
  0x00, 0x00, 0x00, 0x00, // no
  0x00, 0x00, 0x00, 0x00, // no
  0x00, 0x00, 0x00, 0x00, // no
  0x00, 0x00, 0x00, 0x00, // no
  0x00, 0x00, 0x00, 0x00, // no
  0x00, 0x00, 0x00, 0x00, // ctrl left
  0x00, 0x00, 0x00, 0x00, // shift left
  0x00, 0x00, 0x00, 0x00, // capslock
  0x00, 0x00, 0x00, 0x00, // tab
  0x00, 0x00, 0x00, 0x00, // ^ key
  0x00, 0x00, 0x00, 0x00, // esc
  0x00, 0x00, 0x00, 0x00, // fn key
  0x00, 0x00, 0x00, 0x00, // < key
  0x00, 0x00, 0x00, 0x00, // a key
  0x00, 0x00, 0x00, 0x00, // q key
  0x00, 0x00, 0x00, 0x00, // 1 key
  0x00, 0x00, 0x00, 0x00, // f1 key
  0x00, 0x00, 0x00, 0x00, // win key
  0x00, 0x00, 0x00, 0x00, // y key
  0x00, 0x00, 0x00, 0x00, // s key
  0x00, 0x00, 0x00, 0x00, // w key
  0x00, 0x00, 0x00, 0x00, // 2 key
  0x00, 0x00, 0x00, 0x00, // f2 key
  0x00, 0x00, 0x00, 0x00, // alt left
  0x00, 0x00, 0x00, 0x00, // x key
  0x00, 0x00, 0x00, 0x00, // d key
  0x00, 0x00, 0x00, 0x00, // e key
  0x00, 0x00, 0x00, 0x00, // 3 key
  0x00, 0x00, 0x00, 0x00, // f3 key
  0x00, 0x00, 0x00, 0x00, // no
  0x00, 0x00, 0x00, 0x00, // c key
  0x00, 0x00, 0x00, 0x00, // f key
  0x00, 0x00, 0x00, 0x00, // r key
  0x00, 0x00, 0x00, 0x00, // 4 key
  0x00, 0x00, 0x00, 0x00, // f4 key
  0x00, 0x00, 0x00, 0x00, // no
  0x00, 0x00, 0x00, 0x00, // v key
  0x00, 0x00, 0x00, 0x00, // g key
  0x00, 0x00, 0x00, 0x00, // t key
  0x00, 0x00, 0x00, 0x00, // 5 key
  0x00, 0x00, 0x00, 0x00, // f5 key
  0x00, 0x00, 0x00, 0x00, // spacebar
  0x00, 0x00, 0x00, 0x00, // b key
  0x00, 0x00, 0x00, 0x00, // h key
  0x00, 0x00, 0x00, 0x00, // z key
  0x00, 0x00, 0x00, 0x00, // 6 key
  0x00, 0x00, 0x00, 0x00, // f6 key
  0x00, 0x00, 0x00, 0x00, // no
  0x00, 0x00, 0x00, 0x00, // n key
  0x00, 0x00, 0x00, 0x00, // j key
  0x00, 0x00, 0x00, 0x00, // y key
  0x00, 0x00, 0x00, 0x00, // 7 key
  0x00, 0x00, 0x00, 0x00, // f7 key
  0x00, 0x00, 0x00, 0x00, // no
  0x00, 0x00, 0x00, 0x00, // m key
  0x00, 0x00, 0x00, 0x00, // k key
  0x00, 0x00, 0x00, 0x00, // i key
  0x00, 0x00, 0x00, 0x00, // 8 key
  0x00, 0x00, 0x00, 0x00, // f8 key
  0x00, 0x00, 0x00, 0x00, // alt right
  0x00, 0x00, 0x00, 0x00, // , key
  0x00, 0x00, 0x00, 0x00, // l key
  0x00, 0x00, 0x00, 0x00, // o key
  0x00, 0x00, 0x00, 0x00, // 9 key
  0x00, 0x00, 0x00, 0x00, // f9 key
  0x00, 0x00, 0x00, 0x00, // menu key
  0x00, 0x00, 0x00, 0x00, // . key
  0x00, 0x00, 0x00, 0x00, // ö key
  0x00, 0x00, 0x00, 0x00, // p key
  0x00, 0x00, 0x00, 0x00, // 0 key
  0x00, 0x00, 0x00, 0x00, // f10 key
  0x00, 0x00, 0x00, 0x00, // ctrl right
  0x00, 0x00, 0x00, 0x00, // - key
  0x00, 0x00, 0x00, 0x00, // ä key
  0x00, 0x00, 0x00, 0x00, // ü key
  0x00, 0x00, 0x00, 0x00, // ß key
  0x00, 0x00, 0x00, 0x00, // f11 key
  0x00, 0x00, 0x00, 0x00, // no
  0x00, 0x00, 0x00, 0x00, // no
  0x00, 0x00, 0x00, 0x00, // no
  0x00, 0x00, 0x00, 0x00, // + key
  0x00, 0x00, 0x00, 0x00, // ` key
  0x00, 0x00, 0x00, 0x00, // f12 key
  0x00, 0x00, 0x00, 0x00, // arrow left
  0x00, 0x00, 0x00, 0x00, // shift right
  0x00, 0x00, 0x00, 0x00, // # key
  0x00, 0x00, 0xff, 0x00, // no
  0x00, 0xff, 0x00, 0x00, // no
  0x00, 0x00, 0x00, 0xff, // break
  0x00, 0x00, 0x00, 0x00, // arrow down
  0x00, 0x00, 0x00, 0x00, // arrow up
  0x00, 0x00, 0x00, 0x00, // enter
  0x00, 0x00, 0x00, 0x00, // no
  0x00, 0x00, 0x00, 0x00, // backspace
  0x00, 0x00, 0x00, 0x00, // delete
  0x00, 0x00, 0x00, 0x00, // arrow right
  0x00, 0x00, 0x00, 0x00, // num 1
  0x00, 0x00, 0x00, 0x00, // num 4
  0x00, 0x00, 0x00, 0x00, // num 7
  0x00, 0x00, 0x00, 0x00, // numlk
  0x00, 0x00, 0x00, 0x00, // home
  0x00, 0x00, 0x00, 0x00, // num 0
  0x00, 0x00, 0x00, 0x00, // num 2
  0x00, 0x00, 0x00, 0x00, // num 5
  0x00, 0x00, 0x00, 0x00, // num 8
  0x00, 0x00, 0x00, 0x00, // num /
  0x00, 0x00, 0x00, 0x00, // pageup
  0x00, 0x00, 0x00, 0x00, // num ,
  0x00, 0x00, 0x00, 0x00, // num 3
  0x00, 0x00, 0x00, 0x00, // num 6
  0x00, 0x00, 0x00, 0x00, // num 9
  0x00, 0x00, 0x00, 0x00, // num *
  0x00, 0x00, 0x00, 0x00, // pagedown
  0x00, 0x00, 0x00, 0x00, // num enter
  0x00, 0x00, 0x00, 0x00, // no
  0x00, 0x00, 0x00, 0x00, // num plus
  0x00, 0x00, 0x00, 0x00, // no
  0x00, 0x00, 0x00, 0x00, // num -
  0x00, 0x00, 0x00, 0x00, // end
  0x00, 0x00, 0x00, 0x00, // no
  0x00, 0x00, 0x00, 0x00, // no
  0x00, 0x00, 0x00, 0x00, // no
  0x00, 0x00, 0x00, 0x00, // no
  0x00, 0x00, 0x00, 0x00, // no
  0x00, 0x00, 0x00, 0x00, // no
  0x00, 0x00, 0x00, 0x00, // no
  0x00, 0x00, 0x00, 0x00  // no
};

#endif // DATA_H

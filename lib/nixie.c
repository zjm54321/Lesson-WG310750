#include <stc51.h>
#include <stdint.h>
#include <softdelay.h>

#include <nixie.h>

inline void nixie_display(uint8_t location, uint8_t digit) {
    P26 = 1;
    P27 = 1;

    // 选择位置
    P25 = 1;
    P0 = 0xFF;
    P25 = 0;
    P0 = (1 << (7 - location));
    P25 = 1;

    // 显示数字
    P0 = nixie_table[digit];
}

void nixie_display_u8(uint8_t number) {
    uint8_t hundred_digit = number / 100;
    uint8_t ten_digit = (number / 10) % 10;
    uint8_t unit_digit = number % 10;

    nixie_display(0, unit_digit);
    nixie_display(1, ten_digit);
    nixie_display(2, hundred_digit);
}

void nixie_display_fpu8(float number) {
    uint32_t int_number =
        (uint32_t)(number * 10000000); // 放大100000倍以保留小数部分

    nixie_display(7, 10); // 显示小数点
    nixie_display(0, int_number % 10);
    nixie_display(1, (int_number / 10) % 10);
    nixie_display(2, (int_number / 100) % 10);
    nixie_display(3, (int_number / 1000) % 10);
    nixie_display(4, (int_number / 10000) % 10);
    nixie_display(5, (int_number / 100000) % 10);
    nixie_display(6, (int_number / 1000000) % 10);
    nixie_display(7, (int_number / 10000000) % 10);
}
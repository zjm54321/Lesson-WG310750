#include <stc51.h>
#include <stdint.h>
#include <softdelay.h>

#include <nixie.h>

void nixie_display_number(uint8_t number) {
    uint8_t hundred_digit = number / 100;
    uint8_t ten_digit = (number / 10) % 10;
    uint8_t unit_digit = number % 10;

    P26 = 1;
    P27 = 1;

    // 显示个位
    P25 = 1;
    P0 = 0xFF;
    P25 = 0;
    P0 = 0b10000000;
    P25 = 1;
    P0 = nixie_table[unit_digit];
    delay5ms();

    // 显示十位
    P25 = 1;
    P0 = 0xFF;
    P25 = 0;
    P0 = 0b01000000;
    P25 = 1;
    P0 = nixie_table[ten_digit];
    delay5ms();

    // 显示百位
    P25 = 1;
    P0 = 0xFF;
    P25 = 0;
    P0 = 0b00100000;
    P25 = 1;
    P0 = nixie_table[hundred_digit];
    delay5ms();

}

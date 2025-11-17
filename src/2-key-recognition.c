/**
 * @author zjm54321
 * @brief 设计按键识别系统
 */

#include <utility.h>
#include <stc51.h>
#include <stdint.h>
#include <softdelay.h>

#include <exit.h>

int32_t main(void) {

    P2 = ((P2 & 0x1f) | 0x80);

    exti_int0_init();
    exti_int1_init();
    EA = 1; // 全局中断使能

    for (;;) {
        P42 = 0;
        if (P30 == 0) {
            delay5ms();
            if (P30 == 0)
                P0 = 0b11101111;
        } else if (P31) {
            delay5ms();
            if (P31 == 0)
                P0 = 0b11011111;
        }
        P42 = 1;

        P35 = 0;
        if (P30 == 0) {
            delay5ms();
            if (P30 == 0)
                P0 = 0b10111111;
        } else if (P31) {
            delay5ms();
            if (P31 == 0)
                P0 = 0b01111111;
        }
        P35 = 1;
    }
}

void exti0_isr(void) interrupt(0) {
    delay5ms();
    if (P32 == 0)
        P0 = 0b11111100;
}

void exti1_isr(void) interrupt(2) {
    delay5ms();
    if (P33 == 0) {
        P0 = 0b11110011;
    }
}
/**
 * @author zjm54321
 * @brief 信号灯控制系统
 */

#include <utility.h>
#include <stc51.h>
#include <stdint.h>
#include <stdbool.h>
#include <softdelay.h>

#include <timer.h>
#include <exit.h>

static volatile bool flag = false;
static volatile bool running = false;

int32_t main(void) {

    bool state = false;

    timer1_init_10ms();
    exti_int0_init();
    exti_int1_init();
    EA = 1; // 全局中断使能

    for (;;) {
        if (!running)
            continue;

        if (flag) {
            state = !state;
            P2 = ((P2 & 0x1f) | 0x80);
            P0 = state ? 0x00 : 0xFF;
            P2 = (P2 & 0x1F) | 0xA0;
            P06 = state ? 1 : 0;
            flag = false;
        }
    }
}

void timer1_isr(void) interrupt(3) {
    static uint8_t count = 0;
    if (!running)
        return;

    if (++count >= 100) {
        count = 0;
        flag = true;
    }
}

void exti0_isr(void) interrupt(0) {
    delay5ms();
    if (P32 == 0)
        running = true;
}

void exti1_isr(void) interrupt(2) {
    delay5ms();
    if (P33 == 0) {
        running = false;
        P2 = (P2 & 0x1F) | 0xA0;
        P06 = 0;
        P2 = ((P2 & 0x1f) | 0x80);
        P0 = 0xFF;
    }
}